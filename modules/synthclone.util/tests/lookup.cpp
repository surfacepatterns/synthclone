#include <boost/test/unit_test.hpp>

import std;

import synthclone.util;

BOOST_AUTO_TEST_SUITE(lookup)

// XXX: We'd like to add death tests for assume() failures when using the
// subscript operator to access out-of-bounds.

BOOST_AUTO_TEST_CASE(lookup_table_element_constructor)
{
    synthclone::lookup_table odds {
        false,
        true,
        false,
        true,
        false,
        true,
        false,
        true,
        false,
        true
    };

    for (std::size_t i = 0; i < 10; i += 2) {
        BOOST_CHECK(! odds[i]);
        BOOST_CHECK(odds[i + 1]);
        BOOST_CHECK(! odds.at(i));
        BOOST_CHECK(odds.at(i + 1));
    }
}

BOOST_AUTO_TEST_CASE(lookup_table_out_of_bounds_error)
{
    synthclone::lookup_table one { 1 };
    BOOST_CHECK_THROW(one.at(1U), synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(make_lookup_table)
{
    auto odds = synthclone::make_lookup_table<256>(
        [](std::size_t i) -> bool { return i % 2; });

    for (std::size_t i = 0; i < 256; i += 2) {
        BOOST_CHECK(! odds[i]);
        BOOST_CHECK(odds[i + 1]);
        BOOST_CHECK(! odds.at(i));
        BOOST_CHECK(! odds.at(static_cast<std::uint_least8_t>(i)));
        BOOST_CHECK(odds.at(i + 1));
        BOOST_CHECK(odds.at(static_cast<std::uint_least8_t>(i + 1)));
    }
}

BOOST_AUTO_TEST_SUITE_END()
