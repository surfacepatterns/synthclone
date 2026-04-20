#include <boost/test/unit_test.hpp>

import std;

import synthclone.test;
import synthclone.util;

BOOST_AUTO_TEST_SUITE(utility)

BOOST_AUTO_TEST_CASE(basic_out_params)
{
    {
        synthclone::out_param<int> param;
        BOOST_REQUIRE(! param);

        param = 4;
        BOOST_REQUIRE((static_cast<bool>(param)));
        synthclone::verify_eq(4, param.value());
        synthclone::verify_eq(
            4, const_cast<const synthclone::out_param<int>&>(param).value());
        synthclone::verify_eq(4, std::move(param).value());
    }

    {
        synthclone::out_param<int> param;
        BOOST_REQUIRE(! param);

        const int n = 8;
        param = n;
        BOOST_REQUIRE((static_cast<bool>(param)));
        synthclone::verify_eq(8, param.value());
    }

    {
        synthclone::out_param<std::vector<int>> param;
        BOOST_REQUIRE(! param);

        std::array<int, 4> nums {0, 1, 2, 3};
        param.emplace(nums.begin(), nums.end());
        synthclone::verify_eq(std::vector<int> {0, 1, 2, 3}, param.value());
    }

    {
        synthclone::out_param<std::vector<int>> param;
        BOOST_REQUIRE(! param);

        param.emplace(std::initializer_list<int> {0, 1, 2, 3});
        synthclone::verify_eq(std::vector<int> {0, 1, 2, 3}, param.value());
    }
}

BOOST_AUTO_TEST_CASE(out_param_errors)
{
    {
        synthclone::out_param<int> param;
        BOOST_REQUIRE_THROW(param.value(), synthclone::verification_error);
        BOOST_REQUIRE_THROW(
            const_cast<const synthclone::out_param<int>&>(param).value(),
            synthclone::verification_error);
        BOOST_REQUIRE_THROW(
            std::move(param).value(), synthclone::verification_error);
    }

    {
        synthclone::out_param<int> param;

        param = 4;
        BOOST_REQUIRE_THROW(param = 8, synthclone::verification_error);

        const int n = 12;
        BOOST_REQUIRE_THROW(param = n, synthclone::verification_error);
    }

    {
        synthclone::out_param<std::vector<int>> param;
        param = std::vector<int>();

        std::array<int, 4> nums {0, 1, 2, 3};
        BOOST_REQUIRE_THROW(
            param.emplace(nums.begin(), nums.end()),
            synthclone::verification_error);

        std::initializer_list<int> ilist {0, 1, 2, 3};
        BOOST_REQUIRE_THROW(
            param.emplace(ilist), synthclone::verification_error);
    }
}

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
