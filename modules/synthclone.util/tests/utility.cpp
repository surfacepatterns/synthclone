#include <boost/test/unit_test.hpp>

import std;

import synthclone.util;

BOOST_AUTO_TEST_SUITE(utility)

BOOST_AUTO_TEST_CASE(to_signed)
{
    {
        std::uint_least32_t n = 24;
        auto result = synthclone::to_signed(n);

        BOOST_CHECK_EQUAL(24, result);
        BOOST_CHECK((std::same_as<decltype(result), std::int_least32_t>));
    }

    {
        std::int_least16_t n = std::numeric_limits<std::uint_least16_t>::max();
        auto result = synthclone::to_signed(n);

        BOOST_CHECK_EQUAL(-1, result);
        BOOST_CHECK((std::same_as<decltype(result), std::int_least16_t>));
    }

    {
        std::int_least64_t n = 24;
        auto result = synthclone::to_signed(n);

        BOOST_CHECK_EQUAL(24, result);
        BOOST_CHECK((std::same_as<decltype(result), std::int_least64_t>));
    }
}

BOOST_AUTO_TEST_CASE(to_unsigned)
{
    {
        std::int_least32_t n = 24;
        auto result = synthclone::to_unsigned(n);

        BOOST_CHECK_EQUAL(24, result);
        BOOST_CHECK((std::same_as<decltype(result), std::uint_least32_t>));
    }

    {
        std::int_least16_t n = -1;
        auto result = synthclone::to_unsigned(n);

        BOOST_CHECK_EQUAL(
            std::numeric_limits<std::uint_least16_t>::max(), result);
        BOOST_CHECK((std::same_as<decltype(result), std::uint_least16_t>));
    }

    {
        std::uint_least64_t n = 24;
        auto result = synthclone::to_unsigned(n);

        BOOST_CHECK_EQUAL(24, result);
        BOOST_CHECK((std::same_as<decltype(result), std::uint_least64_t>));
    }
}

BOOST_AUTO_TEST_SUITE_END()
