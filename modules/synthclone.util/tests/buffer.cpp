#include <boost/test/unit_test.hpp>

import std;

import synthclone.test;
import synthclone.util;

namespace {

    template<class T, std::size_t... Indices>
    void
    verify_basic_ops(T& buffer, std::index_sequence<Indices...> indices)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info("verify_basic_ops", buffer, indices));

        const auto& const_buffer = buffer;

        ((buffer[Indices] = Indices), ...);

        synthclone::verify_range_elements_eq(buffer, Indices...);
        synthclone::verify_range_elements_eq(const_buffer, Indices...);

        ((buffer.at(Indices) *= 2), ...);

        (synthclone::verify_eq(Indices * 2, buffer.at(Indices)), ...);
        (synthclone::verify_eq(Indices * 2, buffer[Indices]), ...);
        (synthclone::verify_eq(Indices * 2, const_buffer.at(Indices)), ...);
        (synthclone::verify_eq(Indices * 2, const_buffer[Indices]), ...);

        BOOST_CHECK_THROW(
            buffer.at(sizeof...(Indices)), synthclone::verification_error);
        BOOST_CHECK_THROW(
            const_buffer.at(
                sizeof...(Indices)), synthclone::verification_error);

        auto iter = buffer.begin();
        ((*iter++ = Indices * 4), ...);

        BOOST_CHECK(iter == buffer.end());

        synthclone::verify_range_elements_eq(buffer, (Indices * 4)...);
        synthclone::verify_range_elements_eq(const_buffer, (Indices * 4)...);
        synthclone::verify_range_elements_eq(
            const_buffer.cbegin(), const_buffer.cend(), (Indices * 4)...);

        synthclone::verify_range_elements_eq(
            buffer.rbegin(), buffer.rend(),
            ((sizeof...(Indices) - Indices - 1) * 4)...);
        synthclone::verify_range_elements_eq(
            const_buffer.crbegin(), const_buffer.crend(),
            ((sizeof...(Indices) - Indices - 1) * 4)...);

        BOOST_CHECK(! buffer.empty());
        BOOST_CHECK_EQUAL(sizeof...(Indices), buffer.size());
    }

    template<std::size_t N, class T>
    void
    verify_basic_ops(T& buffer)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info("verify_basic_ops", buffer));

        verify_basic_ops<T>(buffer, std::make_index_sequence<N>());
    }

    template<class T>
    void
    verify_move_ops(T& buffer_1, T& buffer_2)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info("verify_move_ops", buffer_1, buffer_2));

        const auto* b1_data = buffer_1.data();
        const auto* b2_data = buffer_2.data();

        {
            buffer_1.swap(buffer_2);

            BOOST_CHECK(buffer_1.data() == b2_data);
            BOOST_CHECK(buffer_2.data() == b1_data);

            synthclone::swap(buffer_1, buffer_2);

            BOOST_CHECK(buffer_1.data() == b1_data);
            BOOST_CHECK(buffer_2.data() == b2_data);
        }

        {
            buffer_1 = std::move(buffer_2);

            BOOST_CHECK(buffer_1.data() == b2_data);
        }

        {
            T buffer_3(std::move(buffer_1));

            BOOST_CHECK(buffer_3.data() == b2_data);
        }
    }

}

BOOST_AUTO_TEST_SUITE(buffer)

BOOST_AUTO_TEST_CASE(dynamic_buffer_move_ops)
{
    synthclone::dynamic_buffer<std::uint_least32_t> buffer_1(40);
    synthclone::dynamic_buffer<std::uint_least32_t> buffer_2(40);

    verify_move_ops(buffer_1, buffer_2);
}

BOOST_AUTO_TEST_CASE(dynamic_buffer_typical_lifetime)
{
    synthclone::dynamic_buffer<std::uint_least32_t> buffer(20);
    verify_basic_ops<20>(buffer);

    auto capacity = buffer.capacity();
    BOOST_CHECK_GE(capacity, 20);

    buffer.reserve(capacity * 4);
    BOOST_CHECK_GE(buffer.capacity(), capacity * 4);
    BOOST_CHECK_EQUAL(20, buffer.size());

    buffer.shrink_to_fit();
    BOOST_CHECK_GE(buffer.capacity(), 20);

    BOOST_CHECK_EQUAL(
        (std::vector<std::uint_least32_t>().max_size()), buffer.max_size());
}

BOOST_AUTO_TEST_CASE(fixed_buffer_move_ops)
{
    synthclone::fixed_buffer<std::uint_least32_t, 40> buffer_1;
    synthclone::fixed_buffer<std::uint_least32_t, 40> buffer_2;

    verify_move_ops(buffer_1, buffer_2);
}

BOOST_AUTO_TEST_CASE(fixed_buffer_typical_lifetime)
{
    synthclone::fixed_buffer<std::uint_least32_t, 20> buffer;
    verify_basic_ops<20>(buffer);

    BOOST_CHECK_EQUAL(20, buffer.max_size());
}

BOOST_AUTO_TEST_SUITE_END()
