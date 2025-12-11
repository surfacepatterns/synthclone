export module synthclone.qt:object;

import std;

import synthclone.external.boost.callable_traits;
import synthclone.external.boost.core;
import synthclone.external.boost.mp11;
import synthclone.external.qt.core;
import synthclone.util;

import :meta;

///////////////////////////////////////////////////////////////////////////////
// synthclone::qobject_ptr
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<class T>
    requires (std::derived_from<std::remove_cv_t<T>, ::QObject>)
    class qobject_ptr;

    template<class T, class = void>
    struct is_qobject_ptr_type: std::false_type {};

    template<class T>
    struct is_qobject_ptr_type<qobject_ptr<T>>: std::true_type {};

    template<class T>
    concept qobject_ptr_type = is_qobject_ptr_type<T>::value;

    template<class T>
    constexpr
    T*
    verify_qobject_ptr(T* ptr)
    {
        verify(ptr != nullptr, "`ptr` is set to NULL");
        return ptr;
    }

    export
    template<class T>
    requires (std::derived_from<std::remove_cv_t<T>, ::QObject>)
    class qobject_ptr final {

    public:

        using element_type = T;

        using pointer = T*;

        constexpr
        qobject_ptr(T& obj)
        noexcept:
            ptr_(&obj)
        {
            // empty
        }

        constexpr
        qobject_ptr(T* ptr):
            ptr_(verify_qobject_ptr<T>(ptr))
        {
            // empty
        }

        template<class U>
        requires (
            explicitly_convertible_to<T*, U> ||
            implicitly_convertible_to<T*, U>
        )
        explicit (! implicitly_convertible_to<T*, U>)
        operator U()
        const noexcept(std::is_nothrow_convertible_v<pointer, U>)
        {
            return static_cast<U>(ptr_);
        }

        constexpr
        pointer
        operator->()
        const noexcept
        {
            return ptr_;
        }

        constexpr
        element_type&
        operator*()
        const noexcept
        {
            return *ptr_;
        }

        constexpr
        pointer
        get()
        const noexcept
        {
            return ptr_;
        }

        constexpr
        void
        swap(qobject_ptr& ptr)
        noexcept
        {
            std::swap(ptr_, ptr.ptr_);
        }

    private:

        pointer ptr_;

    };

    export
    template<class T, class U>
    requires (
        std::equality_comparable_with<
            typename qobject_ptr<T>::pointer,
            typename qobject_ptr<U>::pointer
        >
    )
    constexpr
    bool
    operator==(const qobject_ptr<T>& lhs, const qobject_ptr<U>& rhs)
    noexcept
    {
        return lhs.get() == rhs.get();
    }

    export
    template<class T, class U>
    requires (
        std::three_way_comparable_with<
            typename qobject_ptr<T>::pointer,
            typename qobject_ptr<U>::pointer
        >
    )
    constexpr
    std::compare_three_way_result_t<
        typename qobject_ptr<T>::pointer,
        typename qobject_ptr<U>::pointer
    >
    operator<=>(const qobject_ptr<T>& lhs, const qobject_ptr<U>& rhs)
    noexcept
    {
        return lhs.get() <=> rhs.get();
    }

    export
    template<class Char, class Traits, class T>
    std::basic_ostream<Char, Traits>&
    operator<<(
        std::basic_ostream<Char, Traits>& stream,
        const qobject_ptr<T>& ptr
    )
    {
        return stream << ptr.get();
    }

    export
    template<class T>
    requires (qobject_ptr_type<std::remove_cvref_t<T>>)
    constexpr
    T&&
    make_qobject_ptr(T&& object)
    {
        return std::forward<T>(object);
    }

    export
    template<class T>
    requires (std::derived_from<std::remove_cvref_t<T>, ::QObject>)
    constexpr
    qobject_ptr<std::remove_reference_t<T>>
    make_qobject_ptr(T&& object)
    {
        return std::forward<T>(object);
    }

    export
    template<class T>
    requires(
        (! qobject_ptr_type<std::remove_cvref_t<T>>) &&
        pointer<std::remove_cvref_t<T>> &&
        std::derived_from<pointer_element_t<std::remove_cvref_t<T>>, ::QObject>
    )
    constexpr
    qobject_ptr<pointer_element_t<std::remove_cvref_t<T>>>
    make_qobject_ptr(T&& ptr)
    {
        return std::to_address(std::forward<T>(ptr));
    }

    export
    template<class T>
    constexpr
    void
    swap(qobject_ptr<T>& lhs, qobject_ptr<T>& rhs)
    noexcept
    {
        lhs.swap(rhs);
    }

}

namespace std {

    export
    template<class T>
    class hash<synthclone::qobject_ptr<T>> {

    public:

        std::size_t
        operator()(const synthclone::qobject_ptr<T>& ptr)
        const noexcept
        {
            return impl_(ptr.get());
        }

    private:

        hash<typename synthclone::qobject_ptr<T>::pointer> impl_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_meta_object()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T>
    concept qobject_ptr_convert_result =
        qobject_ptr_type<std::remove_cvref_t<T>>;

    template<class T>
    concept qobject_ptr_convertible = requires (T& obj) {
        { make_qobject_ptr(obj) } -> qobject_ptr_convert_result;
    };

    export
    template<qobject_ptr_convertible T>
    const ::QMetaObject&
    get_meta_object(T&& object)
    {
        const auto* meta_object =
            make_qobject_ptr(std::forward<T>(object))->metaObject();
        assume(meta_object != nullptr, "`meta_object` is set to NULL");
        return *meta_object;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_class_name()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<qobject_ptr_convertible T>
    const char*
    get_class_name(T&& object)
    {
        return get_class_name(get_meta_object(std::forward<T>(object)));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_property()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<qobject_ptr_convertible T>
    ::QMetaProperty
    get_property(T&& object, const char* name)
    {
        return get_property(get_meta_object(std::forward<T>(object)), name);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_method()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<qobject_ptr_convertible T>
    ::QMetaMethod
    get_method(T&& object, const char* method)
    {
        return get_method(get_meta_object(std::forward<T>(object)), method);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_signal()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<qobject_ptr_convertible T>
    ::QMetaMethod
    get_signal(T&& object, const char* signal)
    {
        return get_signal(get_meta_object(std::forward<T>(object)), signal);
    }

}

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
        std::derived_from<
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

    template<class T, class Signal, class U, class Functor>
    requires (qobject_connectable<T, Signal, Functor>)
    ::QMetaObject::Connection
    connect_(
        qobject_ptr<T> sender,
        Signal signal,
        qobject_ptr<U> context,
        Functor&& functor,
        Qt::ConnectionType type = Qt::AutoConnection
    )
    {
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

    template<class T, class U, class Functor>
    ::QMetaObject::Connection
    connect_(
        qobject_ptr<T> sender,
        const char* signal,
        qobject_ptr<U> context,
        Functor &&functor,
        Qt::ConnectionType type = Qt::AutoConnection
    )
    {
        auto connection = ::QMetaObject::connect(
            sender, get_signal(sender, signal), context,
            std::forward<Functor>(functor), type);
        verify(
            static_cast<bool>(connection),
            "::QMetaObject::connect(const {0}*, {1:?}, const {2}*, {3}, {4}): "
            "failed to connect method to signal",
            get_class_name(sender), signal,
            get_class_name(context),
            boost::core::demangle(typeid(Functor).name()),
            std::to_underlying(type));
        return connection;
    }

    export
    template<
        qobject_ptr_convertible T,
        class Signal,
        qobject_ptr_convertible U,
        class Functor
    >
    ::QMetaObject::Connection
    connect(
        T&& sender,
        Signal signal,
        U&& context,
        Functor&& functor,
        Qt::ConnectionType type = Qt::AutoConnection
    )
    {
        return connect_(
            make_qobject_ptr(std::forward<T>(sender)), signal,
            make_qobject_ptr(std::forward<U>(context)), functor, type);
    }

    export
    template<qobject_ptr_convertible T, class Signal, class Functor>
    ::QMetaObject::Connection
    connect(T&& sender, Signal signal, Functor&& functor)
    {
        auto ptr = make_qobject_ptr(std::forward<T>(sender));
        return connect_(ptr, signal, ptr, std::forward<Functor>(functor));
    }

    export
    template<
        qobject_ptr_convertible T,
        qobject_ptr_convertible U,
        class Functor
    >
    ::QMetaObject::Connection
    connect(
        T&& sender,
        const char* signal,
        U&& context,
        Functor &&functor,
        Qt::ConnectionType type = Qt::AutoConnection
    )
    {
        return connect_(
            make_qobject_ptr(std::forward<T>(sender)), signal,
            make_qobject_ptr(std::forward<U>(context)), functor, type);
    }

    export
    template<qobject_ptr_convertible T, class Functor>
    ::QMetaObject::Connection
    connect(T&& sender, const char* signal, Functor &&functor)
    {
        auto ptr = make_qobject_ptr(std::forward<T>(sender));
        return connect(ptr, signal, ptr, std::forward<Functor>(functor));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::find_child()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<std::derived_from<::QObject> T, qobject_ptr_convertible U>
    qobject_ptr<T>
    find_child(
        U&& object,
        std::string_view name,
        Qt::FindChildOptions options = Qt::FindChildrenRecursively
    )
    {
        auto ptr = make_qobject_ptr(std::forward<U>(object));
        auto* child = ptr->template findChild<T*>(
            ::QAnyStringView(name.data(), name.size()), options);
        verify(
            child != nullptr, "{0}::findChild({1:?}, {2}): child not found",
            get_class_name(ptr), name, options.toInt());
        return *child;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_property_value()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T>
    ::QVariant
    get_property_value_(qobject_ptr<T> ptr, ::QMetaProperty property)
    {
        auto value = property.read(ptr);
        verify(
            value.isValid(), "{0}: {1:?}: failed to get property value",
            get_class_name(ptr), property.name());
        return value;
    }

    export
    template<qobject_ptr_convertible T>
    ::QVariant
    get_property_value(T&& object, ::QMetaProperty property)
    {
        verify(property.isValid(), "`property` is not valid");
        return get_property_value_(
            make_qobject_ptr(std::forward<T>(object)), property);
    }

    export
    template<qobject_ptr_convertible T>
    ::QVariant
    get_property_value(T&& object, const char* name)
    {
        auto ptr = make_qobject_ptr(std::forward<T>(object));
        return get_property_value_(ptr, get_property(ptr, name));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::invoke_method()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T, class... Args>
    void
    invoke_method_(qobject_ptr<T> ptr, ::QMetaMethod method, Args&&... args)
    {
        auto invoked = method.invoke(ptr, std::forward<Args>(args)...);
        verify(
            invoked, "{0}: {1}: failed to invoke method", get_class_name(ptr),
            method.name());
    }

    template<class R, class T, class... Args>
    void
    invoke_method_(
        R& result,
        qobject_ptr<T> ptr,
        ::QMetaMethod method,
        Args&&... args
    )
    {
        auto invoked = method.invoke(
            ptr, ::qReturnArg(result), std::forward<Args>(args)...);
        verify(
            invoked, "{0}: {1}: failed to invoke method", get_class_name(ptr),
            method.name());
    }

    export
    template<qobject_ptr_convertible T, class... Args>
    void
    invoke_method(T&& object, QMetaMethod method, Args&&... args)
    {
        verify(method.isValid(), "`method` is not valid");
        invoke_method_(
            make_qobject_ptr(std::forward<T>(object)), method,
            std::forward<Args>(args)...);
    }

    export
    template<qobject_ptr_convertible T, class... Args>
    void
    invoke_method(T&& object, const char* method, Args&&... args)
    {
        auto ptr = make_qobject_ptr(std::forward<T>(object));
        invoke_method_(
            ptr, get_method(ptr, method), std::forward<Args>(args)...);
    }

    export
    template<class R, qobject_ptr_convertible T, class... Args>
    void
    invoke_method(R& result, T&& object, QMetaMethod method, Args&&... args)
    {
        verify(method.isValid(), "`method` is not valid");
        invoke_method_(
            result, make_qobject_ptr(std::forward<T>(object)), method,
            std::forward<Args>(args)...);
    }

    export
    template<class R, qobject_ptr_convertible T, class... Args>
    void
    invoke_method(R& result, T&& object, const char* method, Args&&... args)
    {
        auto ptr = make_qobject_ptr(std::forward<T>(object));
        invoke_method_(
            result, ptr, get_method(ptr, method), std::forward<Args>(args)...);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::qobject_cast()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<class T, qobject_ptr_convertible U>
    requires (std::derived_from<std::remove_cv_t<T>, ::QObject>)
    qobject_ptr<T>
    qobject_cast(U&& object)
    {
        auto ptr = make_qobject_ptr(std::forward<U>(object));
        auto* result = ::qobject_cast<T*>(ptr.get());
        verify(
            result != nullptr, "qobject_cast<{0}*>({1}): cast failed",
            boost::core::demangle(typeid(T).name()), get_class_name(ptr));
        return *result;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_property_value()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T>
    void
    set_property_value_(
        qobject_ptr<T> ptr,
        ::QMetaProperty property,
        ::QVariant&& value
    )
    {
        verify(value.isValid(), "`value` is not valid");
        verify(
            property.write(ptr, std::move(value)),
            "{0}: {1}: failed to set property", get_class_name(ptr),
            property.name());
    }

    export
    template<qobject_ptr_convertible T>
    void
    set_property_value(
        T&& object,
        ::QMetaProperty property,
        ::QVariant value
    )
    {
        verify(property.isValid(), "`property` is not valid");
        set_property_value_(
            make_qobject_ptr(std::forward<T>(object)), property,
            std::move(value));
    }

    export
    template<qobject_ptr_convertible T>
    void
    set_property_value(T&& object, const char* name, ::QVariant value)
    {
        auto ptr = make_qobject_ptr(std::forward<T>(object));
        set_property_value_(
            ptr, get_property(ptr, name), std::move(value));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::qobject_connection_guard
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    class qobject_connection_guard final: private nonmovable {

    public:

        template<class... Args>
        requires (
            requires (Args&&... args) {
                { synthclone::connect(std::forward<Args>(args)...) } ->
                    std::same_as<::QMetaObject::Connection>;
            }
        )
        qobject_connection_guard(Args&&... args):
            connection_(connect(std::forward<Args>(args)...))
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

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::qobject_parent_guard
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T>
    ::QObject*
    reparent(qobject_ptr<T> child, ::QObject* parent)
    {
        ::QObject* old_parent = child->parent();
        child->setParent(parent);
        return old_parent;
    }

    export
    class qobject_parent_guard final: private nonmovable {

    public:

        template<qobject_ptr_convertible T, class U>
        requires (
            pointer<std::remove_cvref_t<U>> &&
            std::derived_from<
                pointer_element_t<std::remove_cvref_t<U>>,
                ::QObject
            >
        )
        qobject_parent_guard(T&& child, U&& parent):
            child_(make_qobject_ptr(std::forward<T>(child))),
            old_parent_(reparent(child_, std::to_address(parent)))
        {
            // empty
        }

        inline
        ~qobject_parent_guard()
        {
            child_->setParent(old_parent_);
        }

    private:

        qobject_ptr<::QObject> child_;
        ::QObject* old_parent_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::qobject_property_value_guard
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T>
    ::QVariant
    swap_property_value(
        qobject_ptr<T> ptr,
        ::QMetaProperty property,
        ::QVariant&& value
    )
    {
        auto old_value = get_property_value(ptr, property);
        set_property_value(ptr, property, std::move(value));
        return old_value;
    }

    export
    class qobject_property_value_guard final: private nonmovable {

    public:

        template<qobject_ptr_convertible T>
        qobject_property_value_guard(
            T&& object,
            ::QMetaProperty property,
            ::QVariant value
        ):
            qobject_property_value_guard(
                make_qobject_ptr(object), property, std::move(value))
        {
            // empty
        }

        template<qobject_ptr_convertible T>
        qobject_property_value_guard(
            T&& object,
            const char* name,
            ::QVariant value
        ):
            qobject_property_value_guard(
                make_qobject_ptr(object), name, std::move(value))
        {
            // empty
        }

        template<class T>
        qobject_property_value_guard(
            qobject_ptr<T> ptr,
            ::QMetaProperty property,
            ::QVariant&& value
        ):
            old_value_(
                swap_property_value(ptr, property, std::move(value))),
            property_(property),
            ptr_(qobject_cast<::QObject>(ptr))
        {
            // empty
        }

        template<class T>
        qobject_property_value_guard(
            qobject_ptr<T> ptr,
            const char* name,
            ::QVariant&& value
        ):
            qobject_property_value_guard(
                ptr, get_property(ptr, name), std::move(value))
        {
            // empty
        }

        inline
        ~qobject_property_value_guard()
        {
            set_property_value(ptr_, property_, std::move(old_value_));
        }

    private:

        ::QVariant old_value_;
        ::QMetaProperty property_;
        qobject_ptr<::QObject> ptr_;

    };

}
