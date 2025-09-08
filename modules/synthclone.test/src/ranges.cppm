module;

#include <boost/test/unit_test.hpp>

export module synthclone.test:ranges;

import std;

import synthclone.util;

import :comparison;
import :concepts;
import :debug;

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_range_elements_eq()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<
        std::input_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    requires (! std::forward_iterator<I>)
    void
    verify_range_elements_eq(I iter, S sentinel, Args&&... args);

    export
    template<
        std::forward_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    requires (! std::bidirectional_iterator<I>)
    void
    verify_range_elements_eq(I iter, S sentinel, Args&&... args);

    export
    template<
        std::bidirectional_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    requires (! std::random_access_iterator<I>)
    void
    verify_range_elements_eq(I iter, S sentinel, Args&&... args);

    export
    template<
        std::random_access_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    requires (! std::contiguous_iterator<I>)
    void
    verify_range_elements_eq(I iter, S sentinel, Args&&... args);

    export
    template<
        std::contiguous_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    void
    verify_range_elements_eq(I iter, S sentinel, Args&&... args);

    template<
        std::input_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>> T
    >
    void
    verify_iter_range_element_eq(
        I& iter,
        S sentinel,
        std::size_t index,
        T&& expected_value
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_iter_range_element_eq", iter, sentinel,
                index, expected_value));

        verify_ne(iter, sentinel);
        verify_eq(*iter, std::forward<T>(expected_value));
    }

    template<
        std::input_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>> T
    >
    void
    verify_input_iter_range_element_eq(
        I& iter,
        S sentinel,
        std::size_t index,
        T&& expected_value
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_input_iter_range_element_eq", iter,
                sentinel, index, expected_value));

        verify_iter_range_element_eq(
            iter, sentinel, index, std::forward<T>(expected_value));

        ++iter;
    }

    template<
        std::input_iterator I,
        std::sentinel_for<I> S,
        std::size_t... Indices,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    void
    verify_input_iter_range_elements_eq(
        I iter,
        S sentinel,
        std::index_sequence<Indices...> indices,
        Args&&... args
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_input_iter_range_elements_eq", iter,
                sentinel, indices, std::forward<Args>(args)...));

        (
            verify_input_iter_range_element_eq(
                iter, sentinel, Indices, std::forward<Args>(args)),
            ...
        );

        verify_eq(iter, sentinel);
    }

    template<
        std::input_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    void
    verify_input_range_elements_eq(I iter, S sentinel, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_input_range_elements_eq", iter, sentinel,
                std::forward<Args>(args)...));

        verify_input_iter_range_elements_eq(
            iter, sentinel, std::make_index_sequence<sizeof...(Args)>(),
            std::forward<Args>(args)...);
    }

    template<
        std::forward_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>> T
    >
    void
    verify_forward_iter_range_element_eq(
        I& iter,
        S sentinel,
        std::size_t index,
        T&& expected_value
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_forward_iter_range_element_eq", iter,
                sentinel, index, expected_value));

        verify_iter_range_element_eq(
            iter, sentinel, index, std::forward<T>(expected_value));

        auto iter_copy = iter;

        verify_eq(iter, iter_copy);

        auto pre_iter = iter++;

        verify_eq(iter_copy, pre_iter);
        verify_ne(iter_copy, iter);
        verify_ne(pre_iter, iter);
    }

    template<
        std::forward_iterator I,
        std::sentinel_for<I> S,
        std::size_t... Indices,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    void
    verify_forward_iter_range_elements_eq(
        I iter,
        S sentinel,
        std::index_sequence<Indices...> indices,
        Args&&... args
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_forward_iter_range_elements_eq", iter,
                sentinel, indices, std::forward<Args>(args)...));

        (
            verify_forward_iter_range_element_eq(
                iter, sentinel, Indices, args),
            ...
        );

        verify_eq(iter, sentinel);
    }

    template<
        std::forward_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    void
    verify_forward_range_elements_eq(I iter, S sentinel, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_forward_range_elements_eq", iter, sentinel,
                std::forward<Args>(args)...));

        verify_input_range_elements_eq(
            iter, sentinel, std::forward<Args>(args)...);

        verify_forward_iter_range_elements_eq(
            iter, sentinel, std::make_index_sequence<sizeof...(Args)>(),
            std::forward<Args>(args)...);
    }

    template<
        std::bidirectional_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>> T
    >
    void
    verify_bidirectional_iter_range_element_eq(
        I& iter,
        I start,
        S sentinel,
        std::size_t index,
        T&& expected_value
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_bidirectional_iter_range_element_eq", iter,
                start, sentinel, index, expected_value));

        verify_ne(iter, start);

        auto tmp = iter;
        auto prev = tmp--;

        verify_eq(prev, iter);
        verify_ne(tmp, iter);

        --iter;

        verify_eq(tmp, iter);
        verify_ne(prev, iter);

        verify_iter_range_element_eq(
            iter, sentinel, index, std::forward<T>(expected_value));
    }

    template<
        std::bidirectional_iterator I,
        std::sentinel_for<I> S,
        std::size_t... Indices,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    void
    verify_bidirectional_iter_range_elements_eq(
        I start,
        S sentinel,
        std::index_sequence<Indices...> indices,
        std::tuple<Args...> args
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_bidirectional_iter_range_elements_eq",
                start, sentinel, indices, args));

        constexpr std::size_t index_count = sizeof...(Indices);

        auto iter = start;
        std::advance(iter, index_count);

        verify_eq(sentinel, iter);

        (
            verify_bidirectional_iter_range_element_eq(
                iter, start, sentinel, (index_count - Indices) - 1,
                std::get<(index_count - Indices) - 1>(args)),
            ...
        );

        verify_eq(start, iter);
    }

    template<
        std::bidirectional_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    void
    verify_bidirectional_range_elements_eq(I iter, S sentinel, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_bidirectional_range_elements_eq", iter,
                sentinel, std::forward<Args>(args)...));

        verify_forward_range_elements_eq(
            iter, sentinel, std::forward<Args>(args)...);

        verify_bidirectional_iter_range_elements_eq(
            iter, sentinel, std::make_index_sequence<sizeof...(Args)>(),
            std::forward_as_tuple(std::forward<Args>(args)...));
    }

    template<
        std::random_access_iterator I,
        weakly_equality_comparable_with<std::iter_value_t<I>> T,
        weakly_equality_comparable_with<std::iter_value_t<I>> U
    >
    void
    verify_random_access_iter_range_element_eq(
        I start,
        I end,
        std::size_t index,
        T&& expected_add_value,
        U&& expected_sub_value
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_random_access_iter_range_element_eq",
                start, end, index, expected_add_value, expected_sub_value));

        auto add_iter_1 = start + index;
        auto add_iter_2 = index + start;
        auto add_equal_iter = start;
        add_equal_iter += index;

        verify_eq(add_iter_1, add_iter_2);
        verify_eq(add_iter_1, add_equal_iter);

        verify_eq(*add_iter_1, expected_add_value);
        verify_eq(*add_iter_2, expected_add_value);
        verify_eq(*add_equal_iter, expected_add_value);
        verify_eq(start[index], expected_add_value);

        auto sub_delta = index + 1;
        auto sub_iter = end - sub_delta;
        auto sub_equal_iter = end;
        sub_equal_iter -= sub_delta;

        verify_eq(sub_iter, sub_equal_iter);

        verify_eq(*sub_iter, expected_sub_value);
        verify_eq(*sub_equal_iter, expected_sub_value);
    }

    template<
        std::random_access_iterator I,
        std::sentinel_for<I> S,
        std::size_t... Indices,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    void
    verify_random_access_iter_range_elements_eq(
        I start,
        S sentinel,
        std::index_sequence<Indices...> indices,
        std::tuple<Args...> args
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_random_access_iter_range_elements_eq",
                start, sentinel, indices, args));

        constexpr std::size_t index_count = sizeof...(Indices);

        auto end = start + index_count;

        verify_eq(sentinel, end);

        (
            verify_random_access_iter_range_element_eq(
                start, end, Indices, std::get<Indices>(args),
                std::get<(index_count - Indices) - 1>(args)),
            ...
        );
    }

    template<
        std::random_access_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    void
    verify_random_access_range_elements_eq(I iter, S sentinel, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_random_access_range_elements_eq", iter,
                sentinel, std::forward<Args>(args)...));

        verify_bidirectional_range_elements_eq(
            iter, sentinel, std::forward<Args>(args)...);

        verify_random_access_iter_range_elements_eq(
            iter, sentinel, std::make_index_sequence<sizeof...(Args)>(),
            std::forward_as_tuple(std::forward<Args>(args)...));
    }

    template<std::contiguous_iterator I>
    void
    verify_contiguous_iter_range_element_eq(I start, std::size_t index)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_contiguous_iter_range_element_eq", start,
                index));

        verify_eq(
            std::to_address(start + index), std::to_address(start) + index);
    }

    template<std::contiguous_iterator I, std::size_t... Indices>
    void
    verify_contiguous_iter_range_elements_eq(
        I start,
        std::index_sequence<Indices...> indices
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_contiguous_iter_range_elements_eq", start,
                indices));

        (verify_contiguous_iter_range_element_eq(start, Indices), ...);
    }

    template<
        std::contiguous_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    void
    verify_contiguous_range_elements_eq(I iter, S sentinel, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_contiguous_range_elements_eq", iter,
                sentinel, std::forward<Args>(args)...));

        verify_random_access_range_elements_eq(
            iter, sentinel, std::forward<Args>(args)...);

        verify_contiguous_iter_range_elements_eq(
            iter, std::make_index_sequence<sizeof...(Args)>());
    }

    template<
        std::ranges::input_range R,
        weakly_equality_comparable_with<std::ranges::range_value_t<R>>... Args
    >
    void
    verify_input_range_elements_eq(R&& range, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_input_range_elements_eq", range,
                std::forward<Args>(args)...));

        if constexpr(std::ranges::sized_range<R>) {
            verify_eq(sizeof...(Args), std::ranges::size(range));
        }

        verify_range_elements_eq(
            std::ranges::begin(range), std::ranges::end(range),
            std::forward<Args>(args)...);
    }

    template<
        std::ranges::contiguous_range R,
        weakly_equality_comparable_with<std::ranges::range_value_t<R>>... Args
    >
    void
    verify_contiguous_range_elements_eq(R&& range, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_contiguous_range_elements_eq", range,
                std::forward<Args>(args)...));

        verify_input_range_elements_eq(range, std::forward<Args>(args)...);

        const auto* data = std::ranges::data(range);
        verify_range_elements_eq(
            data, data + sizeof...(Args), std::forward<Args>(args)...);
    }

    export
    template<
        std::input_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    requires (! std::forward_iterator<I>)
    void
    verify_range_elements_eq(I iter, S sentinel, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_range_elements_eq", iter, sentinel,
                std::forward<Args>(args)...));

        verify_input_range_elements_eq(
            iter, sentinel, std::forward<Args>(args)...);
    }

    export
    template<
        std::forward_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    requires (! std::bidirectional_iterator<I>)
    void
    verify_range_elements_eq(I iter, S sentinel, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_range_elements_eq", iter, sentinel,
                std::forward<Args>(args)...));

        verify_forward_range_elements_eq(
            iter, sentinel, std::forward<Args>(args)...);
    }

    export
    template<
        std::bidirectional_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    requires (! std::random_access_iterator<I>)
    void
    verify_range_elements_eq(I iter, S sentinel, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_range_elements_eq", iter, sentinel,
                std::forward<Args>(args)...));

        verify_bidirectional_range_elements_eq(
            iter, sentinel, std::forward<Args>(args)...);
    }

    export
    template<
        std::random_access_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    requires (! std::contiguous_iterator<I>)
    void
    verify_range_elements_eq(I iter, S sentinel, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_range_elements_eq", iter, sentinel,
                std::forward<Args>(args)...));

        verify_random_access_range_elements_eq(
            iter, sentinel, std::forward<Args>(args)...);
    }

    export
    template<
        std::contiguous_iterator I,
        std::sentinel_for<I> S,
        weakly_equality_comparable_with<std::iter_value_t<I>>... Args
    >
    void
    verify_range_elements_eq(I iter, S sentinel, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_range_elements_eq", iter, sentinel,
                std::forward<Args>(args)...));

        verify_contiguous_range_elements_eq(
            iter, sentinel, std::forward<Args>(args)...);
    }

    export
    template<
        std::ranges::input_range R,
        weakly_equality_comparable_with<std::ranges::range_value_t<R>>... Args
    >
    requires (! std::ranges::contiguous_range<R>)
    void
    verify_range_elements_eq(R&& range, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_range_elements_eq", range,
                std::forward<Args>(args)...));

        verify_input_range_elements_eq(range, std::forward<Args>(args)...);
    }

    export
    template<
        std::ranges::contiguous_range R,
        weakly_equality_comparable_with<std::ranges::range_value_t<R>>... Args
    >
    void
    verify_range_elements_eq(R&& range, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_range_elements_eq", range,
                std::forward<Args>(args)...));

        verify_contiguous_range_elements_eq(
            range, std::forward<Args>(args)...);
    }

}
