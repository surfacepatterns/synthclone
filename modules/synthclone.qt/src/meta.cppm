export module synthclone.qt:meta;

import synthclone.external.qt.core;
import synthclone.util;

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
// synthclone::get_class_name()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    const char*
    get_class_name(const ::QMetaObject& object)
    {
        const auto* class_name = object.className();
        assume(class_name != nullptr, "`class_name` is set to NULL");
        return class_name;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_method()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    ::QMetaMethod
    get_method(const ::QMetaObject& object, const char* method)
    {
        verify(method != nullptr, "`method` is set to NULL");

        auto index = object.indexOfMethod(
            ::QMetaObject::normalizedSignature(method).data());
        verify(
            index != -1, "{0}: {1:?}: method does not exist",
            get_class_name(object), method);

        auto meta_method = object.method(index);
        assume(
            meta_method.isValid(), "{0}: {1:?}: method is not valid",
            get_class_name(object), method);
        return meta_method;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_property()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    ::QMetaProperty
    get_property(const ::QMetaObject& object, const char* name)
    {
        verify(name != nullptr, "`name` is set to NULL");

        auto index = object.indexOfProperty(name);
        verify(
            index != -1, "{0}: {1:?}: property does not exist",
            get_class_name(object), name);

        auto meta_property = object.property(index);
        assume(
            meta_property.isValid(), "{0}: {1:?}: property is not valid",
            get_class_name(object), name);
        return meta_property;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_signal()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    ::QMetaMethod
    get_signal(const ::QMetaObject& object, const char* method)
    {
        verify(method != nullptr, "`method` is set to NULL");

        auto index = object.indexOfSignal(
            ::QMetaObject::normalizedSignature(method).data());
        verify(
            index != -1, "{0}: {1:?}: signal does not exist",
            get_class_name(object), method);

        auto meta_method = object.method(index);
        assume(
            meta_method.isValid(), "{0}: {1:?}: method is not valid",
            get_class_name(object), method);
        return meta_method;
    }

}
