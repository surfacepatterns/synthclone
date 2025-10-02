#include <boost/test/unit_test.hpp>

import std;

import synthclone.external.qt;
import synthclone.qt;
import synthclone.test;
import synthclone.util;

BOOST_AUTO_TEST_SUITE(qobject)

BOOST_AUTO_TEST_CASE(connect_disconnect)
{
    auto ptr = std::make_unique<::QObject>();

    {
        ::QString updated_name;
        auto connection_1 = synthclone::connect(
            ptr.get(), &QObject::objectNameChanged,
            [&updated_name](const ::QString& name) {
                updated_name = name;
            });
        BOOST_CHECK(static_cast<bool>(connection_1));
        synthclone::verify_eq("", updated_name);

        bool updated = false;
        auto connection_2 = synthclone::connect(
            ptr.get(), &QObject::objectNameChanged,
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

    BOOST_CHECK_THROW(
        synthclone::connect(
            ptr.get(), &QObject::objectNameChanged, nullptr,
            [](const ::QString& name) {}),
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
                ptr.get(), &QObject::objectNameChanged,
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

    BOOST_CHECK_THROW(
        synthclone::qobject_connection_guard(
            ptr.get(), &QObject::objectNameChanged, nullptr,
            [](const ::QString& name) {}),
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

    BOOST_CHECK_THROW(
        synthclone::qobject_parent_guard(nullptr, ptr.get()),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_SUITE_END()
