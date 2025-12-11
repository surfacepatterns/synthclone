#include <boost/test/unit_test.hpp>

import std;

import synthclone.external.qt.core;
import synthclone.external.qt.quick;
import synthclone.qt;
import synthclone.test;
import synthclone.util;

namespace {

    template<class T>
    void
    verify_qobject_ptr_equality(
        synthclone::qobject_ptr<T> object_1,
        synthclone::qobject_ptr<T> object_2
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_qobject_ptr_equality", object_1, object_2));

        const auto* address = object_1.get();

        synthclone::verify_eq(object_1, object_2);
        synthclone::verify_eq(address, object_2.get());

        synthclone::swap(object_1, object_2);

        synthclone::verify_eq(object_1, object_2);
        synthclone::verify_eq(address, object_1.get());
        synthclone::verify_eq(address, object_2.get());

        object_1.swap(object_2);

        synthclone::verify_eq(object_1, object_2);
        synthclone::verify_eq(address, object_1.get());
        synthclone::verify_eq(address, object_2.get());

        std::hash<synthclone::qobject_ptr<T>> hash;
        synthclone::verify_eq(hash(object_1), hash(object_2));
    }

    template<class T>
    void
    verify_qobject_ptr_inequality(
        synthclone::qobject_ptr<T> object_1,
        synthclone::qobject_ptr<T> object_2
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_qobject_ptr_inequality", object_1, object_2));

        const auto* address_1 = object_1.get();
        const auto* address_2 = object_2.get();

        synthclone::verify_ne(object_1, object_2);
        synthclone::verify_ne(address_1, address_2);

        synthclone::swap(object_1, object_2);

        synthclone::verify_ne(object_1, object_2);
        synthclone::verify_eq(address_2, object_1.get());
        synthclone::verify_eq(address_1, object_2.get());

        object_1.swap(object_2);

        synthclone::verify_ne(object_1, object_2);
        synthclone::verify_eq(address_1, object_1.get());
        synthclone::verify_eq(address_2, object_2.get());
    }

    template<class T>
    void
    verify_qobject_ptr_ops(T&& object, const ::QObject* expected_address)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_qobject_ptr_ops", object, expected_address));

        auto ptr = synthclone::make_qobject_ptr(object);

        using ptr_t = decltype(ptr);
        using address_t = synthclone::pointer_address_t<ptr_t>;

        address_t addr = ptr;
        synthclone::verify_eq(expected_address, addr);
        synthclone::verify_eq(expected_address, ptr.operator->());
        synthclone::verify_eq(expected_address, &(*ptr));
        synthclone::verify_eq(expected_address, ptr.get());

        auto ptr_copy = ptr;
        synthclone::verify_eq(ptr, ptr_copy);
    }

}

BOOST_AUTO_TEST_SUITE(object)

BOOST_AUTO_TEST_CASE(connect_disconnect)
{
    auto ptr = std::make_unique<::QObject>();

    {
        ::QString updated_name;
        auto connection_1 = synthclone::connect(
            ptr, &QObject::objectNameChanged,
            [&updated_name](const ::QString& name) {
                updated_name = name;
            });
        BOOST_CHECK(static_cast<bool>(connection_1));
        synthclone::verify_eq("", updated_name);

        bool updated = false;
        auto connection_2 = synthclone::connect(
            synthclone::qobject_ptr(ptr.get()), &QObject::objectNameChanged,
            [&updated]() { updated = true; });
        BOOST_CHECK(static_cast<bool>(connection_2));
        BOOST_CHECK(! updated);

        ptr->setObjectName("foo");
        synthclone::verify_eq("foo", updated_name);
        BOOST_CHECK(updated);

        updated = false;
        ptr->setObjectName("bar");
        synthclone::verify_eq("bar", updated_name);
        BOOST_CHECK(updated);

        BOOST_CHECK(static_cast<bool>(connection_1));
        BOOST_CHECK(static_cast<bool>(connection_2));

        synthclone::disconnect(connection_1);

        BOOST_CHECK(! static_cast<bool>(connection_1));
        BOOST_CHECK(static_cast<bool>(connection_2));

        updated = false;
        ptr->setObjectName("baz");
        synthclone::verify_eq("bar", updated_name);
        BOOST_CHECK(updated);

        BOOST_CHECK_THROW(
            synthclone::disconnect(connection_1),
            synthclone::verification_error);

        BOOST_CHECK(static_cast<bool>(connection_2));
        synthclone::disconnect(connection_2);
        BOOST_CHECK(! static_cast<bool>(connection_2));
    }

    BOOST_CHECK_THROW(
        synthclone::connect(
            static_cast<const ::QObject*>(nullptr),
            &QObject::objectNameChanged, [](const ::QString& name) {}),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(connection_guard)
{
    auto ptr = std::make_unique<::QObject>();

    ::QString updated_name;
    {
        synthclone::qobject_connection_guard guard_1(
            ptr.get(), &QObject::objectNameChanged,
            [&updated_name](const ::QString& name) {
                updated_name = name;
            });
        synthclone::verify_eq("", updated_name);

        bool updated = false;
        {
            synthclone::qobject_connection_guard guard_2(
                ptr.get(), "objectNameChanged(const QString&)",
                [&updated]() { updated = true; });
            BOOST_CHECK(! updated);

            ptr->setObjectName("foo");
            synthclone::verify_eq("foo", updated_name);
            BOOST_CHECK(updated);

            updated = false;
            ptr->setObjectName("bar");
            synthclone::verify_eq("bar", updated_name);
            BOOST_CHECK(updated);
        }

        updated = false;
        ptr->setObjectName("baz");
        synthclone::verify_eq("baz", updated_name);
        BOOST_CHECK(! updated);
    }

    ptr->setObjectName("boo");
    synthclone::verify_eq("baz", updated_name);

    BOOST_CHECK_THROW(
        synthclone::qobject_connection_guard(
            static_cast<const ::QObject*>(nullptr),
            &QObject::objectNameChanged, [](const ::QString& name) {}),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(find_child)
{
    ::QObject parent;
    ::QObject child(&parent);
    child.setObjectName("foo");

    auto ptr = synthclone::find_child<::QObject>(parent, "foo");
    synthclone::verify_eq(&child, ptr.get());

    BOOST_CHECK_THROW(
        (synthclone::find_child<::QQuickItem>(parent, "foo")),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        (synthclone::find_child<::QObject>(parent, "bar")),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(get_method)
{
    ::QQuickItem item;

    auto method = synthclone::get_method(item, "dumpItemTree()");
    BOOST_CHECK(method.isValid());

    BOOST_CHECK_THROW(
        synthclone::get_method(item, nullptr), synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::get_method(item, ""), synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(get_set_property_value)
{
    ::QObject object;
    synthclone::verify_eq(
        "", synthclone::get_property_value(object, "objectName"));

    synthclone::set_property_value(object, "objectName", "foo");
    synthclone::verify_eq(
        "foo", synthclone::get_property_value(object, "objectName"));

    auto property = synthclone::get_property(object, "objectName");
    synthclone::set_property_value(object, property, "bar");
    synthclone::verify_eq(
        "bar", synthclone::get_property_value(object, property));

    BOOST_CHECK_THROW(
        synthclone::get_property_value(object, "not-a-property"),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::get_property_value(object, ::QMetaProperty()),
        synthclone::verification_error);

    BOOST_CHECK_THROW(
        synthclone::set_property_value(
            object, "objectName", ::QList<::QString>()),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::set_property_value(object, "not-a-property", "bar"),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::set_property_value(object, ::QMetaProperty(), "bar"),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(get_signal)
{
    ::QObject object;

    auto method = synthclone::get_signal(object, "destroyed(QObject*)");
    BOOST_CHECK(method.isValid());

    BOOST_CHECK_THROW(
        synthclone::get_signal(object, nullptr),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::get_signal(object, ""), synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(invoke_method)
{
    ::QQuickItem item;

    synthclone::invoke_method(item, "ensurePolished()");

    auto method = synthclone::get_method(item, "ensurePolished()");
    synthclone::invoke_method(item, method);

    ::QQuickItem* child = reinterpret_cast<::QQuickItem*>(0xdeadbeef);
    synthclone::invoke_method(child, item, "childAt(qreal,qreal)", 0.0, 0.0);
    synthclone::verify_eq(nullptr, child);

    child = reinterpret_cast<::QQuickItem*>(0xdeadbeef);
    method = synthclone::get_method(item, "childAt(qreal,qreal)");
    synthclone::invoke_method(child, item, method, 0.0, 0.0);
    synthclone::verify_eq(nullptr, child);

    BOOST_CHECK_THROW(
        synthclone::invoke_method(item, "ensureFoo()"),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::invoke_method(item, "ensurePolished()", 32),
        synthclone::verification_error);

    BOOST_CHECK_THROW(
        synthclone::invoke_method(child, item, "childAt(qreal, qreal)"),
        synthclone::verification_error);

    ::QString s;
    BOOST_CHECK_THROW(
        synthclone::invoke_method(s, item, "childAt(qreal, qreal)", 0.0, 0.0),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(parent_guard)
{
    auto ptr = std::make_unique<::QObject>();
    synthclone::verify_eq(nullptr, ptr->parent());

    {
        auto parent_ptr = std::make_unique<::QObject>();
        synthclone::verify_eq(nullptr, ptr->parent());
        synthclone::qobject_parent_guard guard(ptr.get(), parent_ptr.get());
        synthclone::verify_eq(parent_ptr.get(), ptr->parent());
    }

    synthclone::verify_eq(nullptr, ptr->parent());
}

BOOST_AUTO_TEST_CASE(property_value_guard)
{
    auto ptr = std::make_unique<::QObject>();
    synthclone::verify_eq("", ptr->objectName());

    {
        synthclone::qobject_property_value_guard guard(
            ptr.get(), "objectName", "foo");
        synthclone::verify_eq("foo", ptr->objectName());
    }

    synthclone::verify_eq("", ptr->objectName());

    {
        synthclone::qobject_property_value_guard guard(
            ptr.get(), synthclone::get_property(ptr, "objectName"), "bar");
        synthclone::verify_eq("bar", ptr->objectName());
    }

    synthclone::verify_eq("", ptr->objectName());

    BOOST_CHECK_THROW(
        synthclone::qobject_property_value_guard(ptr.get(), nullptr, "foo"),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::qobject_property_value_guard(ptr.get(), "foo", "foo"),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::qobject_property_value_guard(
            ptr.get(), "objectName", ::QList<::QString>()),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(qobject_ptr)
{
    ::QObject object;
    verify_qobject_ptr_ops(object, &object);
    verify_qobject_ptr_ops(&object, &object);
    verify_qobject_ptr_equality(
        synthclone::make_qobject_ptr(object),
        synthclone::make_qobject_ptr(object));

    ::QObject object_2;
    verify_qobject_ptr_inequality(
        synthclone::make_qobject_ptr(object),
        synthclone::make_qobject_ptr(object_2));
}

BOOST_AUTO_TEST_SUITE_END()
