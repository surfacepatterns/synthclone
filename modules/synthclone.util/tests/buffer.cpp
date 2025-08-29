#include <boost/test/unit_test.hpp>

import std;

import synthclone.test;
import synthclone.util;

namespace {

    template<class I, class C>
    void
    verify_iterator_value(I& iter, I sentinel, C value)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format("verify_iterator_value(..., {0})", value));

        synthclone::verify_ne(iter, sentinel);
        BOOST_CHECK_EQUAL(*iter, value);
        ++iter;
    }

    template<class T, std::size_t... Indices>
    void
    verify_basic_ops(T& buffer, std::index_sequence<Indices...>)
    {
        BOOST_TEST_INFO_SCOPE(std::format("verify_basic_ops<T, ...>()"));

        const auto& const_buffer = buffer;

        ((buffer[Indices] = Indices), ...);

        (synthclone::verify_eq(Indices, buffer[Indices]), ...);
        (synthclone::verify_eq(Indices, const_buffer[Indices]), ...);

        ((buffer.at(Indices) *= 2), ...);

        (synthclone::verify_eq(Indices * 2, buffer.at(Indices)), ...);
        (synthclone::verify_eq(Indices * 2, const_buffer.at(Indices)), ...);

        BOOST_CHECK_THROW(
            buffer.at(sizeof...(Indices)), synthclone::verification_error);
        BOOST_CHECK_THROW(
            const_buffer.at(
                sizeof...(Indices)), synthclone::verification_error);

        auto iter = buffer.begin();
        ((*iter++ = Indices * 4), ...);

        BOOST_CHECK(iter == buffer.end());

        {
            auto iter = buffer.begin();
            auto end = buffer.end();

            (verify_iterator_value(iter, end, Indices * 4), ...);

            BOOST_CHECK(iter == end);
        }

        {
            auto iter = buffer.rbegin();
            auto end = buffer.rend();

            (
                verify_iterator_value(
                    iter, end, (sizeof...(Indices) - Indices - 1) * 4),
                ...
            );

            BOOST_CHECK(iter == end);
        }

        {
            auto iter = const_buffer.begin();
            auto end = const_buffer.end();

            (verify_iterator_value(iter, end, Indices * 4), ...);

            BOOST_CHECK(iter == end);
        }

        {
            auto iter = const_buffer.cbegin();
            auto end = const_buffer.cend();

            (verify_iterator_value(iter, end, Indices * 4), ...);

            BOOST_CHECK(iter == end);
        }

        {
            auto iter = const_buffer.crbegin();
            auto end = const_buffer.crend();

            (
                verify_iterator_value(
                    iter, end, (sizeof...(Indices) - Indices - 1) * 4),
                ...
            );

            BOOST_CHECK(iter == end);
        }

        BOOST_CHECK(buffer.data() == std::addressof(*(buffer.begin())));
        BOOST_CHECK(
            const_buffer.data() == std::addressof(*(const_buffer.cbegin())));
        BOOST_CHECK(! buffer.empty());
        BOOST_CHECK_EQUAL(sizeof...(Indices), buffer.size());
    }

    template<std::size_t N, class T>
    void
    verify_basic_ops(T& buffer)
    {
        BOOST_TEST_INFO_SCOPE(std::format("verify_basic_ops<{0}, T>()", N));

        verify_basic_ops<T>(buffer, std::make_index_sequence<N>());
    }

    template<class T>
    void
    verify_move_ops(T& buffer_1, T& buffer_2)
    {
        BOOST_TEST_INFO_SCOPE("verify_move_ops<T>(T&, T&)");

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
