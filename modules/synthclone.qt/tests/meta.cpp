#include <boost/test/unit_test.hpp>

import synthclone.external.qt.core;
import synthclone.external.qt.quick;
import synthclone.qt;
import synthclone.test;
import synthclone.util;

BOOST_AUTO_TEST_SUITE(meta)

BOOST_AUTO_TEST_CASE(methods)
{
    const auto& meta_obj = ::QQuickItem::staticMetaObject;

    auto method = synthclone::get_method(meta_obj, "dumpItemTree()");
    BOOST_CHECK(method.isValid());

    BOOST_CHECK_THROW(
        synthclone::get_method(meta_obj, nullptr),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::get_method(meta_obj, ""), synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(signals)
{
    const auto& meta_obj = ::QObject::staticMetaObject;

    auto method = synthclone::get_signal(meta_obj, "destroyed(QObject*)");
    BOOST_CHECK(method.isValid());

    BOOST_CHECK_THROW(
        synthclone::get_signal(meta_obj, nullptr),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::get_signal(meta_obj, ""), synthclone::verification_error);
}

BOOST_AUTO_TEST_SUITE_END()
