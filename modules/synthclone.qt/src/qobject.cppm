export module synthclone.qt:qobject;

import std;

import synthclone.external.boost.callable_traits;
import synthclone.external.boost.core;
import synthclone.external.boost.mp11;
import synthclone.external.qt.core;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::qobject_connectable
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    // This is somewhat stricter than Qt's implementation.  In particular, if
    // the signal return type is non-void, then the functor return type needs
    // to be non-void and convertible to the signal's return type.

    template<class Functor, class SignalReturnType, class... Args>
    concept qobject_connection_compatible =
        std::invocable<Functor, Args...> &&
        (
            std::same_as<SignalReturnType, void> ||
            (
                (
                    ! std::same_as<
                        std::invoke_result_t<Functor, Args...>,
                        void
                    >
                ) &&
                std::convertible_to<
                    std::invoke_result_t<Functor, Args...>,
                    SignalReturnType
                >
            )
        );

    // "The signature of a signal must match the signature of the receiving
    // slot. (In fact a slot may have a shorter signature than the signal it
    // receives because it can ignore extra arguments.)"
    //
    // https://doc.qt.io/qt-6/signalsandslots.html

    template<class Functor, class SignalReturnType, class Args>
    struct qobject_connectable_ {};

    template<class Functor, class SignalReturnType>
    struct qobject_connectable_<
        Functor,
        SignalReturnType,
        boost::mp11::mp_list<>
    >: std::bool_constant<
        qobject_connection_compatible<Functor, SignalReturnType>
    > {};

    template<class Functor, class SignalReturnType, class Arg, class... Args>
    struct qobject_connectable_<
        Functor,
        SignalReturnType,
        boost::mp11::mp_list<Arg, Args...>
    >: std::conditional_t<
        qobject_connection_compatible<Functor, SignalReturnType, Arg, Args...>,
        std::true_type,
        qobject_connectable_<
            Functor,
            SignalReturnType,
            boost::mp11::mp_pop_back<boost::mp11::mp_list<Arg, Args...>>
        >
    > {};

    export
    template<class T, class Signal, class Functor>
    concept qobject_connectable =
        std::derived_from<std::remove_cvref_t<T>, ::QObject> &&
        std::is_member_function_pointer_v<Signal> &&
        std::same_as<
            std::remove_cvref_t<T>,
            std::remove_cvref_t<boost::callable_traits::class_of_t<Signal>>
        > &&
        qobject_connectable_<
            Functor,
            boost::callable_traits::return_type_t<Signal>,
            // The first argument is the class type, which isn't included when
            // invoking slots.
            boost::mp11::mp_pop_front<
                boost::callable_traits::args_t<
                    Signal,
                    boost::mp11::mp_list
                >
            >
        >::value;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::connect()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    const char*
    get_class_name(const ::QObject* obj)
    {
        assume(obj != nullptr, "`obj` is set to NULL");

        const auto* meta_obj = obj->metaObject();
        assume(meta_obj != nullptr, "meta object is set to NULL");

        const auto* class_name = meta_obj->className();
        assume(class_name != nullptr, "meta object class name is set to NULL");

        return class_name;
    }

    export
    template<class T, class Signal, class Functor>
    requires (qobject_connectable<T, Signal, Functor>)
    ::QMetaObject::Connection
    connect(
        const T* sender,
        Signal signal,
        const ::QObject* context,
        Functor&& functor,
        Qt::ConnectionType type = Qt::AutoConnection
    )
    {
        verify(context != nullptr, "context is set to NULL");
        verify(sender != nullptr, "sender is set to NULL");

        auto connection = ::QObject::connect(
            sender, signal, context, std::forward<Functor>(functor), type);
        verify(
            static_cast<bool>(connection),
            "::QObject::connect(const {0}*, {1}, const {2}*, {3}, {4}): "
            "failed to connect functor to signal",
            get_class_name(sender),
            boost::core::demangle(typeid(Signal).name()),
            get_class_name(context),
            boost::core::demangle(typeid(Functor).name()),
            std::to_underlying(type));

        return connection;
    }

    export
    template<class T, class Signal, class Functor>
    requires (qobject_connectable<T, Signal, Functor>)
    ::QMetaObject::Connection
    connect(const T* sender, Signal signal, Functor&& functor)
    {
        return connect(sender, signal, sender, std::forward<Functor>(functor));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::disconnect()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    disconnect(const ::QMetaObject::Connection& connection)
    {
        verify(
            ::QObject::disconnect(connection),
            "the given connection is not valid or has already been "
            "disconnected");
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::qobject_connection_guard
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<class T, class Signal, class Functor>
    requires (qobject_connectable<T, Signal, Functor>)
    class qobject_connection_guard final: private nonmovable {

    public:

        inline explicit
        qobject_connection_guard(
            const T* sender,
            Signal signal,
            Functor functor
        ):
            connection_(connect(sender, signal, functor))
        {
            // empty
        }

        inline explicit
        qobject_connection_guard(
            const T* sender,
            Signal signal,
            const ::QObject* context,
            Functor functor,
            Qt::ConnectionType type = Qt::AutoConnection
        ):
            connection_(connect(sender, signal, context, functor, type))
        {
            // empty
        }

        inline
        ~qobject_connection_guard()
        {
            disconnect(connection_);
        }

    private:

        ::QMetaObject::Connection connection_;

    };

    export
    template<class T, class Signal, class Functor>
    qobject_connection_guard(const T*, Signal, Functor) ->
        qobject_connection_guard<T, Signal, Functor>;

    export
    template<class T, class Signal, class Functor>
    qobject_connection_guard(
        const T*,
        Signal,
        const ::QObject*,
        Functor
    ) -> qobject_connection_guard<T, Signal, Functor>;

    export
    template<class T, class Signal, class Functor>
    qobject_connection_guard(
        const T*,
        Signal,
        const ::QObject*,
        Functor,
        Qt::ConnectionType
    ) -> qobject_connection_guard<T, Signal, Functor>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::qobject_parent_guard
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    ::QObject*
    reparent(::QObject* child, ::QObject* parent)
    {
        verify(child != nullptr, "child is set to NULL");
        ::QObject* old_parent = child->parent();
        child->setParent(parent);
        return old_parent;
    }

    export
    class qobject_parent_guard final: private nonmovable {

    public:

        inline explicit
        qobject_parent_guard(::QObject* child, ::QObject* parent):
            child_(child),
            old_parent_(reparent(child, parent))
        {
            // empty
        }

        inline
        ~qobject_parent_guard()
        {
            child_->setParent(old_parent_);
        }

    private:

        ::QObject* child_;
        ::QObject* old_parent_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::find_child()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<std::derived_from<::QObject> T>
    T*
    find_child(
        const ::QObject* parent,
        std::string_view name,
        Qt::FindChildOptions options = Qt::FindChildrenRecursively
    )
    {
        verify(parent != nullptr, "parent is set to NULL");
        return parent->findChild<T*>(
            ::QAnyStringView(name.data(), name.size()), options);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::extract_child()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<std::derived_from<::QObject> T>
    T*
    extract_child(
        const ::QObject* parent,
        std::string_view name,
        Qt::FindChildOptions options = Qt::FindChildrenRecursively
    )
    {
        auto* child = find_child<T>(parent, name, options);
        verify(
            child != nullptr,
            "find_child(const {0}*, {1:?}, {2}): child not found",
            get_class_name(parent), name, options.toInt());
        return child;
    }

}
