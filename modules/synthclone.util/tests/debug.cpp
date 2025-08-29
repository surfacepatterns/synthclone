#include <boost/test/unit_test.hpp>

import std;

import synthclone.util;

BOOST_AUTO_TEST_SUITE(debug)

// XXX: Boost.Test doesn't have death tests.  We'd like to do death tests with
// `assume()` and `assume_unreachable()`.

BOOST_AUTO_TEST_CASE(assume)
{
    BOOST_CHECK_NO_THROW(synthclone::assume(true, "wut"));
}

BOOST_AUTO_TEST_CASE(verify)
{
    BOOST_CHECK_NO_THROW(synthclone::verify(true, "wut"));
    BOOST_CHECK_THROW(
        synthclone::verify(false, "yup"), synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(verify_unreachable)
{
    BOOST_CHECK_THROW(
        synthclone::verify_unreachable("wow"), synthclone::verification_error);
}

BOOST_AUTO_TEST_SUITE_END()
