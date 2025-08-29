/**
 * @file
 *
 * Contains fast lookup table facilities.
 */

module;

#include <climits>

#include <synthclone/config.h>

export module synthclone.util:lookup;

import std;

import :debug;

///////////////////////////////////////////////////////////////////////////////
// synthclone::lookup_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class T, std::size_t N>
    constexpr inline
    bool
    needs_lookup_check =
        (sizeof(T) * CHAR_BIT) >
        (std::bit_width(N) + (std::has_single_bit(N) ? 0 : 1));

    /**
     * Fast lookup table facility that allows for table generation at compile
     * time.
     *
     * @tparam T
     *   The value type.
     * @tparam N
     *   The size of the table.
     */

    export
    template<class T, std::size_t N>
    class lookup_table {

        static_assert(N != 0, "lookup table size cannot be 0");

    public:

        /**
         * Constructs a `lookup_table` instance using the given elements.
         *
         * @param elements
         *   The lookup table elements.
         */

        template<std::convertible_to<T>... U>
        requires (sizeof...(U) == N)
        constexpr explicit
        lookup_table(U&&... elements):
            table_ { std::forward<U>(elements)... }
        {
            // empty
        }

        /**
         * Constructs a `lookup_table` instance using the given elements.
         *
         * @param elements
         *   The lookup table elements.
         */

        template<std::convertible_to<T> U>
        constexpr explicit
        lookup_table(std::array<U, N>&& elements):
            table_(elements)
        {
            // empty
        }

        /**
         * Constructs a `lookup_table` instance using the given elements.
         *
         * @param elements
         *   The lookup table elements.
         */

        template<std::convertible_to<T> U>
        constexpr explicit
        lookup_table(const std::array<U, N>& elements):
            table_(elements)
        {
            // empty
        }

        /**
         * Does a lookup of the given index in the lookup table *without*
         * bounds checking.
         *
         * @param n
         *   The index to lookup.
         *
         * @return
         *   The cached value associated with the index.
         */

        template<std::unsigned_integral Index>
        constexpr
        const T&
        operator[](Index index) const noexcept
        {
            assume(
                index < N,
                "index ({0}) is not less than lookup table size ({1})", index,
                N);

            return table_[index];
        }

        /**
         * Does a lookup of the given index in the lookup table.
         *
         * This lookup is used when it's determined that bounds checking is
         * not required for the given index type.
         *
         * @param n
         *   The index to lookup.
         *
         * @return
         *   The cached value associated with the index.
         */

        template<std::unsigned_integral Index>
        requires (! needs_lookup_check<Index, N>)
        constexpr
        const T&
        at(Index index) const noexcept
        {
            return (*this)[index];
        }

        /**
         * Does a lookup of the given index in the lookup table.
         *
         * This lookup is used when it's determined that bounds checking is
         * required for the given type.
         *
         * @param n
         *   The index to lookup.
         *
         * @return
         *   The cached value associated with the index.
         */

        template<std::unsigned_integral Index>
        requires (needs_lookup_check<Index, N>)
        constexpr
        const T&
        at(Index index) const
        {
            verify(
                index < N,
                "index ({0}) is not less than lookup table size ({1})", index,
                N);

            return table_[index];
        }

    private:

        std::array<T, N> table_;

    };

    /**
     * Deduces the `lookup_table` type by the type and number of elements.
     */

    template<class T, std::same_as<T>... U>
    lookup_table(T, U...) -> lookup_table<T, sizeof...(U) + 1>;

    /**
     * Deduces the `lookup_table` type by the type and number of elements in
     * the given array.
     */

    template<class T, std::size_t N>
    lookup_table(const std::array<T, N>&) -> lookup_table<T, N>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::make_lookup_table()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class F, class T>
    concept lookup_table_func =
        std::regular_invocable<F, std::size_t> &&
        std::convertible_to<std::invoke_result_t<F, std::size_t>, T>;

    // We have to pass `N` here because `clang` incorrectly calculates
    // sizeof...(Indices) when the number of indices is greater than 65535:
    //
    //     https://github.com/llvm/llvm-project/issues/119600

    template<
        class T,
        std::size_t N,
        lookup_table_func<T> F,
        std::size_t... Indices
    >
    constexpr
    auto
    make_lookup_table(F f, std::index_sequence<Indices...>)
    {
        return lookup_table(
            std::array<T, N> { static_cast<T>(f(Indices))... });
    }

    /**
     * Creates a `lookup_table` instance using the given function object to
     * generate the lookup table values.
     *
     * @tparam T
     *   The lookup table element type.
     * @tparam N
     *   The size of the lookup table.
     *
     * @param f
     *   The function object to use to generate lookup table values.
     *
     * @return
     *   The lookup table.
     */

    export
    template<class T, std::size_t N, lookup_table_func<T> F>
    requires (N != 0)
    constexpr
    auto
    make_lookup_table(F f)
    {
        return make_lookup_table<T, N>(f, std::make_index_sequence<N>());
    }

    /**
     * Creates a `lookup_table` instance using the given function object to
     * generate the lookup table values.
     *
     * @tparam N
     *   The size of the lookup table.
     *
     * @param f
     *   The function object to use to generate lookup table values.
     *
     * @return
     *   The lookup table.
     */

    export
    template<std::size_t N, std::regular_invocable<std::size_t> F>
    requires (N != 0)
    constexpr
    auto
    make_lookup_table(F f)
    {
        return make_lookup_table<
            std::remove_cvref_t<std::invoke_result_t<F, std::size_t>>,
            N
        >(f, std::make_index_sequence<N>());
    }

}
