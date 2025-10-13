#include <boost/test/unit_test.hpp>

import std;

import synthclone.external.qt.core;
import synthclone.qt;
import synthclone.test;
import synthclone.util;

BOOST_AUTO_TEST_SUITE(qresource)

BOOST_AUTO_TEST_CASE(load_bytes)
{
    synthclone::verify_eq(
        ::QByteArray("bar\n"),
        synthclone::load_qresource_bytes(
            ":/synthclone.qt.test/qresource/foo"));

    BOOST_CHECK_THROW(
        synthclone::load_qresource_bytes(":/synthclone.qt.test/qresource"),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::load_qresource_bytes(":/synthclone.qt.test/qresource/baz"),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_SUITE_END()
