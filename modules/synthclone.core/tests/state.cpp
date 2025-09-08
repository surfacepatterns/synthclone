#include <boost/container/flat_map.hpp>
#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;
import synthclone.util;

namespace {

    template<synthclone::state_kind K>
    void
    verify_basic_value(const synthclone::state_value& value)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_basic_value<{0}>("
                "const synthclone::state_value&)",
                std::to_underlying(K)));

        synthclone::verify_eq(K, value.kind());

        synthclone::state_value cc_value(value);
        synthclone::verify_eq(K, cc_value.kind());
        synthclone::verify_eq(value, cc_value);

        synthclone::state_value mc_value(std::move(cc_value));
        synthclone::verify_eq(K, mc_value.kind());
        synthclone::verify_eq(value, mc_value);

        synthclone::state_value ca_value;
        synthclone::verify_eq(synthclone::state_kind::null, ca_value.kind());
        ca_value = mc_value;
        synthclone::verify_eq(K, ca_value.kind());
        synthclone::verify_eq(value, ca_value);

        synthclone::state_value ma_value;
        synthclone::verify_eq(synthclone::state_kind::null, ma_value.kind());
        ma_value = std::move(ca_value);
        synthclone::verify_eq(K, ma_value.kind());
        synthclone::verify_eq(value, ma_value);
    }

    template<
        std::same_as<synthclone::state_value>... Args,
        std::size_t... Indices
    >
    void
    verify_array_ops(
        const synthclone::state_array& array,
        std::index_sequence<Indices...>,
        const Args... args
    )
    {
        BOOST_TEST_INFO_SCOPE(
            "synthclone::verify_array_ops(const synthclone::state_array&, "
            "std::index_sequence<...>, ...)");

        synthclone::verify_range_elements_eq(array, args...);
        synthclone::verify_range_elements_eq(
            array.begin(), array.end(), args...);
        synthclone::verify_range_elements_eq(
            array.cbegin(), array.cend(), args...);

        auto tuple = std::make_tuple(args...);

        synthclone::verify_range_elements_eq(
            array.rbegin(), array.rend(),
            std::get<sizeof...(Indices) - Indices - 1>(tuple)...);
        synthclone::verify_range_elements_eq(
            array.crbegin(), array.crend(),
            std::get<sizeof...(Indices) - Indices - 1>(tuple)...);

        (synthclone::verify_eq(args, array[Indices]), ...);

        (synthclone::verify_eq(args, array.at(Indices)), ...);

        BOOST_CHECK_THROW(
            array.at(sizeof...(Indices)), synthclone::verification_error);

        constexpr auto expected_empty_result = sizeof...(Args) == 0;
        BOOST_CHECK_EQUAL(expected_empty_result, std::ranges::empty(array));
        BOOST_CHECK_EQUAL(expected_empty_result, array.empty());

        BOOST_CHECK_EQUAL(
            std::vector<synthclone::state_value>{}.max_size(),
            array.max_size());
    }

    template<class... Args>
    requires (... && std::constructible_from<synthclone::state_value, Args>)
    void
    verify_array_ops(const synthclone::state_array& array, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            "synthclone::verify_array_ops(const synthclone::state_array&, "
            "...)");

        verify_array_ops(
            array, std::make_index_sequence<sizeof...(Args)>(),
            synthclone::state_value(std::forward<Args>(args))...);
    }

    void
    verify_array_value(
        const synthclone::state_value& value,
        const synthclone::state_array& a
    )
    {
        BOOST_TEST_INFO_SCOPE(
            "synthclone::verify_array_value(const synthclone::state_value&, "
            "const synthclone::state_array&)");

        verify_basic_value<synthclone::state_kind::array>(value);

        synthclone::verify_eq(value, synthclone::state_value(a));
        synthclone::verify_ne(
            value,
            synthclone::state_value(
                synthclone::state_array(
                    std::from_range,
                    std::ranges::views::join(
                        std::vector<synthclone::state_array> {
                            a,
                            synthclone::state_array {0}
                        }
                    )
                )
            )
        );

        synthclone::verify_eq(a, synthclone::extract_array(value));

        synthclone::verify_ne(value, synthclone::state_value(true));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_map()));
        synthclone::verify_ne(value, synthclone::state_value(nullptr));
        synthclone::verify_ne(value, synthclone::state_value(0U));
        synthclone::verify_ne(value, synthclone::state_value(0));
        synthclone::verify_ne(value, synthclone::state_value(0.0));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_string()));

        BOOST_CHECK_THROW(
            synthclone::extract_boolean(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_map(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::uint_least64_t>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::int_least64_t>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<double>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_string(value), synthclone::state_error);
    }

    void
    verify_boolean_value(const synthclone::state_value& value, const bool b)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_boolean_value("
                "const synthclone::state_value&, {0})",
                b));

        verify_basic_value<synthclone::state_kind::boolean>(value);

        synthclone::verify_eq(value, synthclone::state_value(b));
        synthclone::verify_ne(value, synthclone::state_value(! b));

        synthclone::verify_eq(b, synthclone::extract_boolean(value));

        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_array()));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_map()));
        synthclone::verify_ne(value, synthclone::state_value(nullptr));
        synthclone::verify_ne(value, synthclone::state_value(0U));
        synthclone::verify_ne(value, synthclone::state_value(0));
        synthclone::verify_ne(value, synthclone::state_value(0.0));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_string()));

        BOOST_CHECK_THROW(
            synthclone::extract_array(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_map(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::uint_least64_t>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::int_least64_t>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<double>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_string(value), synthclone::state_error);
    }

    template<std::floating_point T>
    void
    verify_floating_point_value(
        const synthclone::state_value& value,
        const T n
    )
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_floating_point_value<>("
                "const synthclone::state_value&, {0})",
                n));

        verify_basic_value<synthclone::state_kind::floating_point>(value);

        synthclone::verify_eq(value, synthclone::state_value(n));

        synthclone::verify_eq(n, synthclone::extract_number<T>(value));
        synthclone::verify_eq(
            static_cast<double>(n), synthclone::extract_number<double>(value));

        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_array()));
        synthclone::verify_ne(value, synthclone::state_value(true));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_map()));
        synthclone::verify_ne(value, synthclone::state_value(nullptr));
        synthclone::verify_ne(value, synthclone::state_value(0U));
        synthclone::verify_ne(value, synthclone::state_value(0));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_string()));

        BOOST_CHECK_THROW(
            synthclone::extract_array(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_boolean(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_map(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_string(value), synthclone::state_error);
    }

    template<std::floating_point T>
    void
    verify_floating_point_values()
    {
        BOOST_TEST_INFO_SCOPE("synthclone::verify_floating_point_values<>()");

        constexpr T max_n = std::numeric_limits<T>::max();
        constexpr T min_n = std::numeric_limits<T>::lowest();
        constexpr T zero_n = static_cast<T>(0);

        synthclone::state_value max_value(max_n);
        verify_floating_point_value(max_value, max_n);

        synthclone::state_value min_value(min_n);
        verify_floating_point_value(min_value, min_n);

        synthclone::state_value zero_value(zero_n);
        verify_floating_point_value(zero_value, zero_n);

        synthclone::verify_ne(min_value, max_value);
        synthclone::verify_ne(min_value, zero_value);
        synthclone::verify_ne(zero_value, max_value);

        max_value.swap(zero_value);
        verify_floating_point_value(zero_value, max_n);
        verify_floating_point_value(max_value, zero_n);

        synthclone::swap(max_value, zero_value);
        verify_floating_point_value(zero_value, zero_n);
        verify_floating_point_value(max_value, max_n);

        synthclone::verify_eq(
            0, synthclone::extract_number<std::int_least8_t>(zero_value));
        synthclone::verify_eq(
            0, synthclone::extract_number<std::uint_least8_t>(zero_value));

        BOOST_CHECK_THROW(
            synthclone::extract_number<std::int_least8_t>(min_value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::int_least8_t>(max_value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::uint_least8_t>(min_value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::uint_least8_t>(max_value),
            synthclone::state_error);
    }

    template<
        std::same_as<synthclone::state_map::value_type>... Args,
        std::size_t... Indices
    >
    void
    verify_map_ops(
        const synthclone::state_map& map,
        std::index_sequence<Indices...>,
        const Args... args
    )
    {
        BOOST_TEST_INFO_SCOPE(
            "synthclone::verify_map_ops(const synthclone::state_map&, "
            "std::index_sequence<...>, ...)");

        synthclone::verify_range_elements_eq(map, args...);
        synthclone::verify_range_elements_eq(
            map.begin(), map.end(), args...);
        synthclone::verify_range_elements_eq(
            map.cbegin(), map.cend(), args...);

        auto tuple = std::make_tuple(args...);

        synthclone::verify_range_elements_eq(
            map.rbegin(), map.rend(),
            std::get<sizeof...(Indices) - Indices - 1>(tuple)...);
        synthclone::verify_range_elements_eq(
            map.crbegin(), map.crend(),
            std::get<sizeof...(Indices) - Indices - 1>(tuple)...);

        (synthclone::verify_eq(args.second, map.at(args.first)), ...);
        (synthclone::verify_eq(true, map.contains(args.first)), ...);

        BOOST_CHECK_THROW(map.at("new-key"), synthclone::verification_error);
        BOOST_CHECK(! map.contains("new-key"));

        constexpr auto expected_empty_result = sizeof...(Args) == 0;
        BOOST_CHECK_EQUAL(expected_empty_result, std::ranges::empty(map));
        BOOST_CHECK_EQUAL(expected_empty_result, map.empty());

        BOOST_CHECK_EQUAL(
            (
                boost::container::flat_map<
                    synthclone::state_string,
                    synthclone::state_value
                >{}.max_size()
            ),
            map.max_size());
    }

    template<class... Args>
    requires (
        ... && std::constructible_from<synthclone::state_map::value_type, Args>
    )
    void
    verify_map_ops(const synthclone::state_map& map, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            "synthclone::verify_map_ops(const synthclone::state_map&, ...)");

        verify_map_ops(
            map, std::make_index_sequence<sizeof...(Args)>(),
            synthclone::state_map::value_type(std::forward<Args>(args))...);
    }

    void
    verify_map_value(
        const synthclone::state_value& value,
        const synthclone::state_map& m
    )
    {
        BOOST_TEST_INFO_SCOPE(
            "synthclone::verify_map_value(const synthclone::state_value&, "
            "const synthclone::state_map&)");

        verify_basic_value<synthclone::state_kind::map>(value);

        synthclone::verify_eq(value, synthclone::state_value(m));
        synthclone::verify_ne(
            value,
            synthclone::state_value(
                synthclone::state_map(
                    std::from_range,
                    std::ranges::views::join(
                        std::vector<synthclone::state_map> {
                            m,
                            synthclone::state_map {{"new-key", 43}}
                        }
                    )
                )
            )
        );

        synthclone::verify_eq(m, synthclone::extract_map(value));

        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_array()));
        synthclone::verify_ne(value, synthclone::state_value(true));
        synthclone::verify_ne(value, synthclone::state_value(nullptr));
        synthclone::verify_ne(value, synthclone::state_value(0U));
        synthclone::verify_ne(value, synthclone::state_value(0));
        synthclone::verify_ne(value, synthclone::state_value(0.0));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_string()));

        BOOST_CHECK_THROW(
            synthclone::extract_array(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_boolean(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::uint_least64_t>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::int_least64_t>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<double>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_string(value), synthclone::state_error);
    }

    void
    verify_null_value(const synthclone::state_value& value)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_null_value("
                "const synthclone::state_value&)"));

        verify_basic_value<synthclone::state_kind::null>(value);

        synthclone::verify_eq(value, synthclone::state_value(nullptr));
        synthclone::verify_eq(value, synthclone::state_value());

        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_array()));
        synthclone::verify_ne(value, synthclone::state_value(true));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_map()));
        synthclone::verify_ne(value, synthclone::state_value(0U));
        synthclone::verify_ne(value, synthclone::state_value(0));
        synthclone::verify_ne(value, synthclone::state_value(0.0));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_string()));

        BOOST_CHECK_THROW(
            synthclone::extract_array(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_boolean(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_map(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::uint_least64_t>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::int_least64_t>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<double>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_string(value), synthclone::state_error);
    }

    template<std::signed_integral T>
    void
    verify_signed_int_value(
        const synthclone::state_value& value,
        const T n
    )
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_signed_int_value<>("
                "const synthclone::state_value&, {0})",
                n));

        verify_basic_value<synthclone::state_kind::signed_int>(value);

        synthclone::verify_eq(value, synthclone::state_value(n));
        synthclone::verify_ne(value, synthclone::state_value(n - 1));
        synthclone::verify_ne(value, synthclone::state_value(n + 1));

        synthclone::verify_eq(n, synthclone::extract_number<T>(value));
        synthclone::verify_eq(
            n, synthclone::extract_number<std::int64_t>(value));
        synthclone::verify_eq(
            static_cast<double>(n), synthclone::extract_number<double>(value));

        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_array()));
        synthclone::verify_ne(value, synthclone::state_value(true));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_map()));
        synthclone::verify_ne(value, synthclone::state_value(nullptr));
        synthclone::verify_ne(value, synthclone::state_value(0.0));
        synthclone::verify_ne(value, synthclone::state_value(
            synthclone::state_string()));

        BOOST_CHECK_THROW(
            synthclone::extract_array(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_boolean(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_map(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_string(value), synthclone::state_error);
    }

    template<std::signed_integral T>
    void
    verify_signed_int_values()
    {
        BOOST_TEST_INFO_SCOPE("synthclone::verify_signed_int_values<>()");

        constexpr T max_n = std::numeric_limits<T>::max();
        constexpr T min_n = std::numeric_limits<T>::min();
        constexpr T zero_n = static_cast<T>(0);

        synthclone::state_value max_value(max_n);
        verify_signed_int_value(max_value, max_n);

        synthclone::state_value min_value(min_n);
        verify_signed_int_value(min_value, min_n);

        synthclone::state_value zero_value(zero_n);
        verify_signed_int_value(zero_value, zero_n);

        synthclone::verify_ne(min_value, max_value);
        synthclone::verify_ne(min_value, zero_value);
        synthclone::verify_ne(zero_value, max_value);

        synthclone::verify_eq(zero_value, synthclone::state_value(0U));
        synthclone::verify_ne(zero_value, synthclone::state_value(1U));

        max_value.swap(zero_value);
        verify_signed_int_value(zero_value, max_n);
        verify_signed_int_value(max_value, zero_n);

        synthclone::swap(max_value, zero_value);
        verify_signed_int_value(zero_value, zero_n);
        verify_signed_int_value(max_value, max_n);

        synthclone::verify_eq(
            0.0, synthclone::extract_number<double>(zero_value));
        synthclone::verify_eq(
            0U, synthclone::extract_number<std::uint_least8_t>(zero_value));

        BOOST_CHECK_THROW(
            synthclone::extract_number<std::uint_least8_t>(min_value),
            synthclone::state_error);
    }

    void
    verify_string_value(
        const synthclone::state_value& value,
        const synthclone::state_string& s
    )
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_string_value("
                "const synthclone::state_value&, {0:?})",
                s));

        verify_basic_value<synthclone::state_kind::string>(value);

        synthclone::verify_eq(value, synthclone::state_value(s));
        synthclone::verify_eq(
            value, synthclone::state_value(static_cast<std::string_view>(s)));

        synthclone::verify_ne(value, synthclone::state_value(s.base() + "a"));

        synthclone::verify_eq(s, synthclone::extract_string(value));

        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_array()));
        synthclone::verify_ne(value, synthclone::state_value(true));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_map()));
        synthclone::verify_ne(value, synthclone::state_value(nullptr));
        synthclone::verify_ne(value, synthclone::state_value(0U));
        synthclone::verify_ne(value, synthclone::state_value(0));
        synthclone::verify_ne(value, synthclone::state_value(0.0));

        BOOST_CHECK_THROW(
            synthclone::extract_array(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_boolean(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_map(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::uint_least64_t>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<std::int_least64_t>(value),
            synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_number<double>(value),
            synthclone::state_error);
    }

    template<std::unsigned_integral T>
    void
    verify_unsigned_int_value(
        const synthclone::state_value& value,
        const T n
    )
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_unsigned_int_value<>("
                "const synthclone::state_value&, {0})",
                n));

        verify_basic_value<synthclone::state_kind::unsigned_int>(value);

        synthclone::verify_eq(value, synthclone::state_value(n));
        synthclone::verify_ne(value, synthclone::state_value(n - 1));
        synthclone::verify_ne(value, synthclone::state_value(n + 1));

        synthclone::verify_eq(n, synthclone::extract_number<T>(value));
        synthclone::verify_eq(
            n, synthclone::extract_number<std::uint64_t>(value));
        synthclone::verify_eq(
            static_cast<double>(n), synthclone::extract_number<double>(value));

        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_array()));
        synthclone::verify_ne(value, synthclone::state_value(true));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_map()));
        synthclone::verify_ne(value, synthclone::state_value(nullptr));
        synthclone::verify_ne(value, synthclone::state_value(0.0));
        synthclone::verify_ne(
            value, synthclone::state_value(synthclone::state_string()));

        BOOST_CHECK_THROW(
            synthclone::extract_array(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_boolean(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_map(value), synthclone::state_error);
        BOOST_CHECK_THROW(
            synthclone::extract_string(value), synthclone::state_error);
    }

    template<std::unsigned_integral T>
    void
    verify_unsigned_int_values()
    {
        BOOST_TEST_INFO_SCOPE("synthclone::verify_unsigned_int_values<>()");

        constexpr T max_n = std::numeric_limits<T>::max();
        constexpr T min_n = std::numeric_limits<T>::min();

        synthclone::state_value max_value(max_n);
        verify_unsigned_int_value(max_value, max_n);

        synthclone::state_value min_value(min_n);
        verify_unsigned_int_value(min_value, min_n);

        synthclone::verify_ne(min_value, max_value);

        synthclone::verify_eq(min_value, synthclone::state_value(0));
        synthclone::verify_ne(min_value, synthclone::state_value(1));
        synthclone::verify_ne(min_value, synthclone::state_value(-1));

        max_value.swap(min_value);
        verify_unsigned_int_value(min_value, max_n);
        verify_unsigned_int_value(max_value, min_n);

        synthclone::swap(max_value, min_value);
        verify_unsigned_int_value(min_value, min_n);
        verify_unsigned_int_value(max_value, max_n);

        synthclone::verify_eq(
            0.0, synthclone::extract_number<double>(min_value));
        synthclone::verify_eq(
            0, synthclone::extract_number<std::int_least8_t>(min_value));

        BOOST_CHECK_THROW(
            synthclone::extract_number<std::int_least8_t>(max_value),
            synthclone::state_error);
    }

}

BOOST_AUTO_TEST_SUITE(state)

BOOST_AUTO_TEST_CASE(array_ops)
{
    synthclone::state_array state_array_1 {"foo", "bar", "baz"};
    verify_array_ops(state_array_1, "foo", "bar", "baz");

    std::vector<synthclone::state_value> values_2 {1, 2, "red", "blue"};
    synthclone::state_array state_array_2(values_2.begin(), values_2.end());
    verify_array_ops(state_array_2, 1, 2, "red", "blue");

    synthclone::verify_ne(state_array_1, state_array_2);

    synthclone::state_array state_array_3(state_array_1);
    verify_array_ops(state_array_3, "foo", "bar", "baz");

    synthclone::verify_eq(state_array_1, state_array_3);
    synthclone::verify_ne(state_array_2, state_array_3);

    state_array_3 = state_array_2;
    verify_array_ops(state_array_3, 1, 2, "red", "blue");

    synthclone::verify_eq(state_array_2, state_array_3);

    state_array_3 = std::move(state_array_1);
    verify_array_ops(state_array_3, "foo", "bar", "baz");

    synthclone::verify_ne(state_array_2, state_array_3);

    state_array_2.swap(state_array_3);
    verify_array_ops(state_array_2, "foo", "bar", "baz");
    verify_array_ops(state_array_3, 1, 2, "red", "blue");

    synthclone::swap(state_array_2, state_array_3);
    verify_array_ops(state_array_2, 1, 2, "red", "blue");
    verify_array_ops(state_array_3, "foo", "bar", "baz");

    synthclone::state_array state_array_4;
    verify_array_ops(state_array_4);

    synthclone::verify_ne(state_array_2, state_array_4);
    synthclone::verify_ne(state_array_3, state_array_4);
}

BOOST_AUTO_TEST_CASE(array_values)
{
    synthclone::state_array array {
        1,
        true,
        3,
        4.0,
        "I",
        "declare",
        "a",
        nullptr,
        "war"
    };
    synthclone::state_value value(array);
    verify_array_value(value, array);

    synthclone::state_array array_2;
    synthclone::state_value value_2(array_2);
    verify_array_value(value_2, array_2);
}

BOOST_AUTO_TEST_CASE(boolean_values)
{
    synthclone::state_value value(true);
    verify_boolean_value(value, true);

    synthclone::state_value value_2(false);
    verify_boolean_value(value_2, false);
}

BOOST_AUTO_TEST_CASE(floating_point_values)
{
    verify_floating_point_values<float>();
    verify_floating_point_values<double>();
}

BOOST_AUTO_TEST_CASE(map_ops)
{
    synthclone::state_map state_map_1 {
        {"foo", "bar"},
        {"baz", 2}
    };
    verify_map_ops(
        state_map_1, std::make_pair("baz", 2), std::make_pair("foo", "bar"));

    std::vector<synthclone::state_map::value_type> values_2 {
        {"red", 1},
        {"blue", 2}
    };
    synthclone::state_map state_map_2(values_2.begin(), values_2.end());
    verify_map_ops(
        state_map_2, std::make_pair("blue", 2), std::make_pair("red", 1));

    synthclone::verify_ne(state_map_1, state_map_2);

    synthclone::state_map state_map_3(state_map_1);
    verify_map_ops(
        state_map_3, std::make_pair("baz", 2), std::make_pair("foo", "bar"));

    synthclone::verify_eq(state_map_1, state_map_3);
    synthclone::verify_ne(state_map_2, state_map_3);

    state_map_3 = state_map_2;
    verify_map_ops(
        state_map_3, std::make_pair("blue", 2), std::make_pair("red", 1));

    synthclone::verify_eq(state_map_2, state_map_3);

    state_map_3 = std::move(state_map_1);
    verify_map_ops(
        state_map_3, std::make_pair("baz", 2), std::make_pair("foo", "bar"));

    synthclone::verify_ne(state_map_2, state_map_3);

    state_map_2.swap(state_map_3);
    verify_map_ops(
        state_map_2, std::make_pair("baz", 2), std::make_pair("foo", "bar"));
    verify_map_ops(
        state_map_3, std::make_pair("blue", 2), std::make_pair("red", 1));

    synthclone::swap(state_map_2, state_map_3);
    verify_map_ops(
        state_map_2, std::make_pair("blue", 2), std::make_pair("red", 1));
    verify_map_ops(
        state_map_3, std::make_pair("baz", 2), std::make_pair("foo", "bar"));

    synthclone::state_map state_map_4;
    verify_map_ops(state_map_4);

    synthclone::verify_ne(state_map_2, state_map_4);
    synthclone::verify_ne(state_map_3, state_map_4);
}

BOOST_AUTO_TEST_CASE(map_values)
{
    synthclone::state_map map {
        {"one", true},
        {"three", 4.0},
        {"I", "declare"},
        {"a", nullptr},
        {"thumb", "war"}
    };
    synthclone::state_value value(map);
    verify_map_value(value, map);

    synthclone::state_map map_2;
    synthclone::state_value value_2(map_2);
    verify_map_value(value_2, map_2);
}

BOOST_AUTO_TEST_CASE(null_values)
{
    synthclone::state_value value;
    verify_null_value(value);

    synthclone::state_value value_2(nullptr);
    verify_null_value(value_2);
}

BOOST_AUTO_TEST_CASE(signed_int_values)
{
    verify_signed_int_values<std::int_least8_t>();
    verify_signed_int_values<std::int_least16_t>();
    verify_signed_int_values<std::int_least32_t>();
    verify_signed_int_values<std::int_least64_t>();
}

BOOST_AUTO_TEST_CASE(string_values)
{
    synthclone::state_value value("foo");
    verify_string_value(value, "foo");

    synthclone::state_value value_2("bar");
    verify_string_value(value_2, "bar");
}

BOOST_AUTO_TEST_CASE(unsigned_int_values)
{
    verify_unsigned_int_values<std::uint_least8_t>();
    verify_unsigned_int_values<std::uint_least16_t>();
    verify_unsigned_int_values<std::uint_least32_t>();
    verify_unsigned_int_values<std::uint_least64_t>();
}

BOOST_AUTO_TEST_SUITE_END()
