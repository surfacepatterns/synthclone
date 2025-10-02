/**
 * @file
 *
 * Contains functionality for managing state snapshots for `synthclone`
 * components.
 *
 * @note
 *   This API is heavily influenced by `boost::json`; in fact, the
 *   (de)serialization functionality we currently use in `synthclone` is from
 *   `boost::json`.  The reason we're not using the primitives from
 *   `boost::json` is that we don't want to couple `synthclone` components with
 *   `boost::json` directly (or, more generally, with the concept of JSON) in
 *   case we decide to change the underlying implementation.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:state;

import std;

import synthclone.external.boost;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::state_error
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Identifies an error specific to state functionality.
     */

    export
    class state_error: public std::runtime_error {

    public:

        using std::runtime_error::runtime_error;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::state_string
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * A `state_value` type that holds UTF-8 encoded data.
     */

    export
    using state_string = utf8_string;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::state_array (declaration)
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    class state_value;

    using state_array_impl = std::vector<state_value>;

    /**
     * A `state_value` type that holds a contiguous array of `state_value`
     * instances.
     *
     * @note
     *   `state_array` elements are thin wrappers around `std::vector`
     *   instances, and thus have all the performance characteristics
     *   associated with `std::vector`.
     */

    export
    class state_array final {

    public:

        /**
         * Constant iterator type that iterates over `state_value` instances.
         */

        using const_iterator = typename state_array_impl::const_iterator;

        /**
         * Constant reference type to a `state_value` instance.
         */

        using const_reference = typename state_array_impl::const_reference;

        /**
         * Constant reverse iterator type that iterates over `state_value`
         * instances.
         */

        using const_reverse_iterator =
            typename state_array_impl::const_reverse_iterator;

        /**
         * Difference type for two iterators.
         */

        using difference_type = typename state_array_impl::difference_type;

        /**
         * @copydoc
         *   const_iterator
         */

        using iterator = const_iterator;

        /**
         * @copydoc
         *   const_reference
         */

        using reference = const_reference;

        /**
         * @copydoc
         *   const_reverse_iterator
         */

        using reverse_iterator = const_reverse_iterator;

        /**
         * Size type used by `state_array` instances.
         */

        using size_type = typename state_array_impl::size_type;

        /**
         * The element type.
         */

        using value_type = typename state_array_impl::value_type;

        /**
         * Constructs a `state_array` instance with no elements.
         */

        constexpr
        state_array();

        /**
         * Move constructor.
         */

        constexpr
        state_array(state_array&&);

        /**
         * Copy constructor.
         */

        constexpr
        state_array(const state_array&);

        /**
         * Constructs a `state_array` instance using the range specified by the
         * given iterator and sentinel.
         *
         * @param first
         *   The iterator.
         * @param last
         *   The sentinel.
         */

        template<std::input_iterator I, std::sentinel_for<I> S>
        requires (std::convertible_to<std::iter_value_t<I>, state_value>)
        constexpr
        state_array(I first, S last);

        /**
         * Constructs a `state_array` instance using elements in the given
         * range.
         *
         * @param range
         *   The range.
         */

        template<container_compatible_range<value_type> R>
        constexpr
        state_array(std::from_range_t, R&& range);

        /**
         * Constructs a `state_array` instance using elements in the given
         * `std::initializer_list`.
         *
         * @param list
         *   The initializer list.
         */

        constexpr
        state_array(std::initializer_list<value_type> list);

        /**
         * Destructor
         */

        constexpr
        ~state_array();

        /**
         * Move assignment operator.
         */

        constexpr
        state_array&
        operator=(state_array&&);

        /**
         * Copy assignment operator.
         */

        constexpr
        state_array&
        operator=(const state_array&);

        /**
         * Accesses the value at the given index without bounds checking.
         *
         * @param n
         *   The index.
         *
         * @return
         *   A value reference.
         */

        constexpr
        const_reference
        operator[](size_type n)
        const;

        /**
         * Gets a boolean indicating if this instance compares equal to the
         * given instance.
         *
         * @param array
         *   The array to compare.
         *
         * @return
         *   The boolean indicator.
         */

        constexpr
        bool
        operator==(const state_array& array)
        const noexcept;

        /**
         * Accesses the value at the given index with bounds checking.
         *
         * @param n
         *   The index.
         *
         * @return
         *   A value reference.
         */

        constexpr
        const_reference
        at(size_type n)
        const;

        /**
         * Gets an iterator that points at the beginning of the array.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        begin()
        const;

        /**
         * @copydoc
         *   const_iterator begin() const
         */

        constexpr
        const_iterator
        cbegin()
        const;

        /**
         * Gets an iterator that points at the end of the array.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_iterator
        cend()
        const;

        /**
         * Gets a reverse iterator that points at the end of the array.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_reverse_iterator
        crbegin()
        const;

        /**
         * Gets a reverse iterator that points at the beginning of the array.
         *
         * @return
         *   The iterator.
         */

        constexpr
        const_reverse_iterator
        crend()
        const;

        /**
         * Gets a boolean indicating if the size is set to 0.
         *
         * @return
         *   The boolean indicator.
         */

        constexpr
        bool
        empty()
        const;

        /**
         * @copydoc
         *   const_iterator cend() const
         */

        constexpr
        const_iterator
        end()
        const;

        /**
         * Gets the maximum amount of values that can be held by this instance.
         *
         * @return
         *   The maximum.
         */

        constexpr
        size_type
        max_size()
        const;

        /**
         * @copydoc
         *   const_reverse_iterator crbegin() const
         */

        constexpr
        const_reverse_iterator
        rbegin()
        const;

        /**
         * @copydoc
         *   const_reverse_iterator crend() const
         */

        constexpr
        const_reverse_iterator
        rend()
        const;

        /**
         * Gets the number of values held by this instance.
         *
         * @return
         *   The size.
         */

        constexpr
        size_type
        size()
        const;

        /**
         * Swaps the contents of this instance and the given instance.
         *
         * @param rhs
         *   The instance to swap with.
         */

        constexpr
        void
        swap(state_array& rhs)
        noexcept;

    private:

        state_array_impl impl_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::state_map (declaration)
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    // We use `boost::container::flat_map` instead of `std::flat_map` because
    // `boost::container::flat_map` is guaranteed to handle incomplete types.
    using state_map_impl = boost::container::flat_map<
        state_string,
        state_value
    >;

    /**
     * A `state_value` type that maps `std::string` keys to `state_value`
     * instances.
     *
     * @note
     *   `state_map` elements are thin wrappers around
     *   `boost::container::flat_map` instances, and thus have all the
     *   performance characteristics associated with
     *   `boost::container::flat_map`.
     */

    export
    class state_map final {

    public:

        /**
         * Constant iterator type that iterates over map pairs.
         */

        using const_iterator = state_map_impl::const_iterator;

        /**
         * Constant reference type to a map pair.
         */

        using const_reference = state_map_impl::const_reference;

        /**
         * Constant reverse iterator type that iterates over map pairs.
         */

        using const_reverse_iterator =
            typename state_map_impl::const_reverse_iterator;

        /**
         * Difference type for two iterators.
         */

        using difference_type = state_map_impl::difference_type;

        /**
         * @copydoc
         *   const_iterator
         */

        using iterator = const_iterator;

        /**
         * Key type contains by `state_map` instances.
         */

        using key_type = state_map_impl::key_type;

        /**
         * Mapped type contains by `state_map` instances.
         */

        using mapped_type = state_map_impl::mapped_type;

        /**
         * @copydoc
         *   const_reference
         */

        using reference = const_reference;

        /**
         * @copydoc
         *   const_reverse_iterator
         */

        using reverse_iterator = const_reverse_iterator;

        /**
         * Size type used by `state_map` instances.
         */

        using size_type = state_map_impl::size_type;

        /**
         * Key/value pair type contains by `state_map` instances.
         */

        using value_type = state_map_impl::value_type;

        /**
         * Constructs a `state_map` instance with no elements.
         */

        state_map();

        /**
         * Move constructor.
         */

        state_map(state_map&&);

        /**
         * Copy constructor.
         */

        state_map(const state_map&);

        /**
         * Constructs a `state_map` instance using key/value pairs in the range
         * specified by the given iterator and sentinel.
         *
         * @param first
         *   The iterator.
         * @param last
         *   The sentinel.
         */

        template<std::input_iterator I, std::sentinel_for<I> S>
        requires (std::convertible_to<std::iter_value_t<I>, value_type>)
        state_map(I first, S last);

        /**
         * Constructs a `state_map` instance using key/value pairs in the given
         * range.
         *
         * @param range
         *   The range.
         */

        template<container_compatible_range<value_type> R>
        state_map(std::from_range_t, R&& range);

        /**
         * Constructs a `state_map` instance using key/value pairs in the given
         * `std::initializer_list`.
         *
         * @param list
         *   The initializer list.
         */

        inline
        state_map(std::initializer_list<value_type> list);

        /**
         * Destructor
         */

        ~state_map();

        /**
         * Move assignment operator.
         */

        state_map&
        operator=(state_map&&);

        /**
         * Copy assignment operator.
         */

        state_map&
        operator=(const state_map&);

        /**
         * Gets a boolean indicating if this instance compares equal to the
         * given instance.
         *
         * @param map
         *   The map to compare.
         *
         * @return
         *   The boolean indicator.
         */

        inline
        bool
        operator==(const state_map& map)
        const noexcept;

        /**
         * @copydoc
         *   template<class K> const mapped_type at(const K&) const
         */

        inline
        const mapped_type&
        at(const key_type& key)
        const;

        /**
         * Gets an iterator that points at the beginning of the map.
         *
         * @return
         *   The iterator.
         */

        inline
        const_iterator
        begin()
        const noexcept;

        /**
         * @copydoc
         *   const_iterator begin() const
         */

        inline
        const_iterator
        cbegin()
        const noexcept;

        /**
         * Gets an iterator that points at the end of the map.
         *
         * @return
         *   The iterator.
         */

        inline
        const_iterator
        cend()
        const noexcept;

        /**
         * @copydoc
         *   template<class K> bool contains(const K&) const
         */

        inline
        bool
        contains(const key_type& key)
        const;

        /**
         * Gets a reverse iterator that points at the end of the map.
         *
         * @return
         *   The iterator.
         */

        inline
        const_reverse_iterator
        crbegin()
        const;

        /**
         * Gets a reverse iterator that points at the beginning of the map.
         *
         * @return
         *   The iterator.
         */

        inline
        const_reverse_iterator
        crend()
        const;

        /**
         * Gets a boolean indicating if the size is set to 0.
         *
         * @return
         *   The boolean indicator.
         */

        inline
        bool
        empty()
        const noexcept;

        /**
         * @copydoc
         *   const_iterator cend() const
         */

        inline
        const_iterator
        end()
        const noexcept;

        /**
         * @copydoc
         *   template<class K> const_iterator find(const K&) const
         */

        inline
        const_iterator
        find(const key_type& key)
        const;

        /**
         * Gets the maximum amount of values that can be held by this instance.
         *
         * @return
         *   The maximum.
         */

        inline
        size_type
        max_size()
        const noexcept;

        /**
         * @copydoc
         *   const_reverse_iterator crbegin() const
         */

        inline
        const_reverse_iterator
        rbegin()
        const;

        /**
         * @copydoc
         *   const_reverse_iterator crend() const
         */

        inline
        const_reverse_iterator
        rend()
        const;

        /**
         * Gets the number of pairs held by this instance.
         *
         * @return
         *   The size.
         */

        inline
        size_type
        size()
        const noexcept;

        /**
         * Swaps the contents of this instance and the given instance.
         *
         * @param rhs
         *   The instance to swap with.
         */

        inline
        void
        swap(state_map& rhs)
        noexcept;

    private:

        state_map_impl impl_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::state_kind
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    using state_variant = std::variant<
        std::nullptr_t,
        bool,
        std::int_least64_t,
        std::uint_least64_t,
        double,
        state_string,
        state_array,
        state_map
    >;

    /**
     * Enumerates the potential types contained in a `state_value` instance.
     */

    export
    enum class state_kind: std::uint_least8_t {

        /**
         * Array type.
         */

        array = boost::mp11::mp_find<state_variant, state_array>::value,

        /**
         * Boolean type.
         */

        boolean = boost::mp11::mp_find<state_variant, bool>::value,

        /**
         * Floating point type.
         */

        floating_point = boost::mp11::mp_find<state_variant, double>::value,

        /**
         * Signed integer type.
         */

        signed_int =
            boost::mp11::mp_find<state_variant, std::int_least64_t>::value,

        /**
         * Map type.
         */

        map = boost::mp11::mp_find<state_variant, state_map>::value,

        /**
         * Null type.
         */

        null = boost::mp11::mp_find<state_variant, std::nullptr_t>::value,

        /**
         * String type.
         */

        string = boost::mp11::mp_find<state_variant, state_string>::value,

        /**
         * Unsigned integer type.
         */

        unsigned_int =
            boost::mp11::mp_find<state_variant, std::uint_least64_t>::value

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::state_value
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    std::uint_least64_t max_unsigned_int_state_value = 0xffffffffffffffff;

    constexpr
    std::int_least64_t max_signed_int_state_value = 0xfffffffffffffff;

    constexpr
    std::int_least64_t min_signed_int_state_value = -9223372036854775807 - 1;

    template<class T>
    concept state_integral =
        std::same_as<std::remove_cvref_t<T>, std::int_least64_t> ||
        std::same_as<std::remove_cvref_t<T>, std::uint_least64_t>;

    struct state_variant_equality_visitor final {

        template<class T, class U>
        bool
        operator()(const T& lhs, const U& rhs)
        {
            if constexpr(std::same_as<T, U>) {
                return lhs == rhs;
            } else if constexpr(state_integral<T> && state_integral<U>) {
                return std::cmp_equal(lhs, rhs);
            } else {
                return false;
            }
        }

    };

    template<std::signed_integral T>
    requires (sizeof(T) <= 8)
    state_variant
    make_state_variant(const T n)
    {
        return state_variant(static_cast<std::int_least64_t>(n));
    }

    template<std::signed_integral T>
    requires (sizeof(T) > 8)
    state_variant
    make_state_variant(const T n)
    {
        if (
            (n < min_signed_int_state_value) ||
            (n > max_signed_int_state_value)
        ) [[unlikely]] {
            throw state_error(
                std::format(
                    "{0}: value is out of range [{1}, {2}] for signed integer "
                    "state values",
                    n, min_signed_int_state_value,
                    max_signed_int_state_value));
        }
        return state_variant(static_cast<std::int_least64_t>(n));
    }

    template<std::unsigned_integral T>
    requires (sizeof(T) <= 8)
    state_variant
    make_state_variant(const T n)
    {
        return state_variant(static_cast<std::uint_least64_t>(n));
    }

    template<std::unsigned_integral T>
    requires (sizeof(T) > 8)
    state_variant
    make_state_variant(const T n)
    {
        if (n > max_unsigned_int_state_value) [[unlikely]] {
            throw state_error(
                std::format(
                    "{0}: value is out of range [0, {1}] for unsigned "
                    "integer state values",
                    n, max_unsigned_int_state_value));
        }
        return state_variant(static_cast<std::uint_least64_t>(n));
    }

    struct state_value_array_extraction_visitor final {

        template<class T>
        [[noreturn]]
        const state_array&
        operator()(const T& array)
        const
        {
            throw state_error(
                "the given state value cannot be converted to an array");
        }

        const state_array&
        operator()(const state_array& array)
        const noexcept
        {
            return array;
        }

    };

    struct state_value_boolean_extraction_visitor final {

        template<class T>
        [[noreturn]]
        bool
        operator()(const T& value)
        const
        {
            throw state_error(
                "the given state value cannot be converted to a boolean");
        }

        bool
        operator()(const bool value)
        const noexcept
        {
            return value;
        }

    };

    template<class FormatContext, class Char>
    class state_value_format_visitor final: private nonmovable {

    public:

        state_value_format_visitor(FormatContext& context):
            context_(context)
        {
            // empty
        }

        template<class T>
        typename FormatContext::iterator
        operator()(const T& value)
        {
            return std::formatter<T, Char>{}.format(value, context_);
        }

        typename FormatContext::iterator
        operator()(const state_string& value)
        {
            std::formatter<state_string, Char> formatter;
            formatter.set_debug_format();
            return formatter.format(value, context_);
        }

    private:

        FormatContext& context_;

    };

    struct state_value_map_extraction_visitor final {

        template<class T>
        [[noreturn]]
        const state_map&
        operator()(const T& map)
        const
        {
            throw state_error(
                "the given state value cannot be converted to a map");
        }

        const state_map&
        operator()(const state_map& map)
        const noexcept
        {
            return map;
        }

    };

    template<class T>
    struct state_value_number_extraction_visitor final {};

    template<std::floating_point T>
    struct state_value_number_extraction_visitor<T> final {

        template<class U>
        [[noreturn]]
        T
        operator()(const U& n)
        const
        {
            throw state_error(
                "the given state value cannot be converted to the given "
                "floating point type");
        }

        T
        operator()(const double n)
        const noexcept
        {
            return static_cast<T>(n);
        }

        T
        operator()(const std::int_least64_t n)
        const noexcept
        {
            return static_cast<T>(n);
        }

        T
        operator()(const std::uint_least64_t n)
        const noexcept
        {
            return static_cast<T>(n);
        }

    };

    template<std::integral T>
    requires (! std::same_as<T, bool>)
    struct state_value_number_extraction_visitor<T> final {

        template<class U>
        [[noreturn]]
        T
        operator()(const U& n)
        const
        {
            throw state_error(
                "the given state value cannot be converted to the given "
                "integral type");
        }

        T
        operator()(const double n)
        const
        {

            using limits = std::numeric_limits<T>;

            if (
                ! (
                    (n >= static_cast<double>(limits::min())) &&
                    (n <= static_cast<double>(limits::max())) &&
                    (static_cast<T>(n) == n)
                )
            ) {
                throw state_error(
                    std::format(
                        "the given state value {0} cannot be converted to the "
                        "given integral type",
                        n));
            }
            return static_cast<T>(n);
        }

        T
        operator()(const std::int_least64_t n)
        const
        {
            if (! std::in_range<T>(n)) [[unlikely]] {
                throw state_error(
                    std::format(
                        "the given state value {0} cannot be converted to the "
                        "given integral type",
                        n));
            }
            return static_cast<T>(n);
        }

        T
        operator()(const std::uint_least64_t n)
        const
        {
            if (! std::in_range<T>(n)) [[unlikely]] {
                throw state_error(
                    std::format(
                        "the given state value {0} cannot be converted to the "
                        "given integral type",
                        n));
            }
            return static_cast<T>(n);
        }

    };

    struct state_value_string_extraction_visitor final {

        template<class T>
        [[noreturn]]
        const state_string&
        operator()(const T& s)
        const
        {
            throw state_error(
                "the given state value cannot be converted to a string");
        }

        const state_string&
        operator()(const state_string& s)
        const noexcept
        {
            return s;
        }

    };

    /**
     * Variant type used in `synthclone` for basic serialization of session
     * state.
     */

    export
    class state_value final {

    public:

        /**
         * Constructs a `state_value` instance that holds a `std::nullptr_t`.
         */

        constexpr
        state_value():
            state_value(nullptr)
        {
            // empty
        }

        /**
         * Constructs a `state_value` instance that holds an integral value.
         *
         * @param value
         *   The value.
         */

        template<std::integral T>
        constexpr
        state_value(const T value):
            impl_(make_state_variant(value))
        {
            // empty
        }

        /**
         * Constructs a `state_value` instance that holds a floating point
         * value.
         *
         * @param value
         *   The value.
         */

        template<std::floating_point T>
        requires (sizeof(T) <= sizeof(double))
        constexpr
        state_value(const T value):
            impl_(static_cast<double>(value))
        {
            // empty
        }

        /**
         * Constructs a `state_value` instance that holds a string value.
         *
         * @param value
         *   The value.
         */

        template<string_view_convertible T>
        constexpr
        state_value(T&& value):
            impl_(
                state_string(
                    static_cast<std::string_view>(std::forward<T>(value))))
        {
            // empty
        }

        /**
         * @copydoc
         *   state_value::state_value()
         */

        constexpr
        state_value(const std::nullptr_t):
            impl_(nullptr)
        {
            // empty
        }

        /**
         * Constructs a `state_value` instance that holds a boolean value.
         *
         * @param value
         *   The value.
         */

        constexpr
        state_value(const bool value):
            impl_(value)
        {
            // empty
        }

        /**
         * @copydoc
         *   template<string_view_convertible T> state_value::state_value(T&)
         */

        constexpr
        state_value(const char* value):
            state_value(state_string(value))
        {
            // empty
        }

        /**
         * @copydoc
         *   template<string_view_convertible T> state_value::state_value(T&)
         */

        constexpr
        state_value(std::string value):
            state_value(state_string(std::move(value)))
        {
            // empty
        }

        /**
         * Constructs a `state_value` instance that holds an array value.
         *
         * @param value
         *   The value.
         */

        constexpr
        state_value(state_array array):
            impl_(std::move(array))
        {
            // empty
        }

        /**
         * Constructs a `state_value` instance that holds a map value.
         *
         * @param value
         *   The value.
         */

        inline
        state_value(state_map map):
            impl_(std::move(map))
        {
            // empty
        }

        /**
         * @copydoc
         *   template<string_view_convertible T> state_value::state_value(T&)
         */

        constexpr
        state_value(state_string s):
            impl_(std::move(s))
        {
            // empty
        }

        /**
         * Gets the underlying value type.
         *
         * @return
         *   The underlying value type.
         */

        constexpr
        state_kind
        kind()
        const noexcept
        {
            return static_cast<state_kind>(impl_.index());
        }

        /**
         * Swaps the contents of this instance and the given instance.
         *
         * @param rhs
         *   The instance to swap with.
         */

        void
        swap(state_value& rhs)
        noexcept
        {
            impl_.swap(rhs.impl_);
        }

        /**
         * Applies the given visitor to the raw value contained in this
         * instance.
         *
         * @param visitor
         *   The visitor.
         *
         * @return
         *   The visitor result.
         */

        template<class Visitor>
        constexpr
        decltype(auto)
        visit(Visitor visitor)
        const
        {
            return std::visit(visitor, impl_);
        }

        /**
         * Applies the given visitor to the raw values contained in the given
         * `state_value` instances.
         *
         * @param visitor
         *   The visitor.
         * @param values
         *   The values.
         *
         * @return
         *   The visitor result.
         */

        template<
            class Visitor,
            std::common_reference_with<state_value&>... Values
        >
        friend
        constexpr
        decltype(auto)
        visit(Visitor visitor, Values&&... values)
        {
            return std::visit(visitor, values.impl_...);
        }

    private:

        state_variant impl_;

    };

    /**
     * Gets a boolean indicating whether or not the two `state_value` instances
     * are equal.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     *
     * @return
     *   The boolean indicator.
     */

    export
    inline
    bool
    operator==(const state_value& lhs, const state_value& rhs)
    noexcept
    {
        return visit(state_variant_equality_visitor{}, lhs, rhs);
    }

    /**
     * Gets a boolean indicating whether or not the two `state_value` instances
     * are inequal.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     *
     * @return
     *   The boolean indicator.
     */

    export
    inline
    bool
    operator!=(const state_value& lhs, const state_value& rhs)
    noexcept
    {
        return ! (lhs == rhs);
    }

    /**
     * Gets a reference to an array in a `state_value` instance.
     *
     * @param value
     *   The `state_value` instance.
     *
     * @return
     *   The array reference.
     */

    export
    inline
    const state_array&
    extract_array(const state_value& value)
    {
        return value.visit(state_value_array_extraction_visitor{});
    }

    /**
     * Gets a boolean value in a `state_value` instance.
     *
     * @param value
     *   The `state_value` instance.
     *
     * @return
     *   The boolean value.
     */

    export
    inline
    bool
    extract_boolean(const state_value& value)
    {
        return value.visit(state_value_boolean_extraction_visitor{});
    }

    /**
     * Gets a reference to a map in a `state_value` instance.
     *
     * @param value
     *   The `state_value` instance.
     *
     * @return
     *   The map reference.
     */

    export
    inline
    const state_map&
    extract_map(const state_value& value)
    {
        return value.visit(state_value_map_extraction_visitor{});
    }

    /**
     * Gets a number value in a `state_value` instance.
     *
     * If the given type is a floating point type, then the function will
     * extract a `double` and cast it to the given type.
     *
     * If the extraction type is a integral type, then this function will
     * extract a `std::int_least64_t` or `std::uint_least64_t`, and cast the
     * value to the given type if the value is within the range of the given
     * type.
     *
     * @param value
     *   The `state_value` instance.
     *
     * @return
     *   The number value.
     */

    export
    template<arithmetic T>
    requires (! std::same_as<T, bool>)
    T
    extract_number(const state_value& value)
    {
        return value.visit(state_value_number_extraction_visitor<T>{});
    }

    /**
     * Gets a reference to a string in a `state_value` instance.
     *
     * @param value
     *   The `state_value` instance.
     *
     * @return
     *   The string reference.
     */

    export
    inline
    const state_string&
    extract_string(const state_value& value)
    {
        return value.visit(state_value_string_extraction_visitor{});
    }

    /**
     * Swaps the contents of the two `state_value` instances.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     */

    export
    inline
    void
    swap(state_value& lhs, state_value& rhs)
    noexcept
    {
        lhs.swap(rhs);
    }

}

namespace std {

    /**
     * Formatter implementation for `state_value` instances.
     */

    template<class Char>
    struct formatter<synthclone::state_value, Char> {

    public:

        /**
         * Formats a `synthclone::state_value` instance for output.
         *
         * @param value
         *   The `synthclone::state_value` instance to format.
         * @param context
         *   The format context to write the output to.
         *
         * @return
         *   An iterator pointing at the end of the output range.
         */

        template<class FormatContext>
        typename FormatContext::iterator
        format(
            const synthclone::state_value& value,
            FormatContext& context
        )
        const
        {
            return value.visit(
                synthclone::state_value_format_visitor<FormatContext, Char>(
                    context));
        }

        /**
         * Parses the format specification for a `synthclone::state_value`
         * instance.
         *
         * @param context
         *   The parse context.
         *
         * @return
         *   An iterator pointing at the end of the parsed specification.
         */

        template<class ParseContext>
        constexpr
        typename ParseContext::iterator
        parse(ParseContext& context)
        {
            return context.begin();
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::state_array (definition)
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    state_array::state_array() = default;

    constexpr
    state_array::state_array(state_array&&) = default;

    constexpr
    state_array::state_array(const state_array&) = default;

    template<std::input_iterator I, std::sentinel_for<I> S>
    requires (std::convertible_to<std::iter_value_t<I>, state_value>)
    constexpr
    state_array::state_array(I first, S last):
        state_array(std::from_range, std::ranges::subrange(first, last))
    {
        // empty
    }

    template<container_compatible_range<state_array::value_type> R>
    constexpr
    state_array::state_array(std::from_range_t, R&& range):
        impl_(std::from_range, std::forward<R>(range))
    {
        // empty
    }

    constexpr
    state_array::state_array(std::initializer_list<value_type> list):
        impl_(list.begin(), list.end())
    {
        // empty
    }

    constexpr
    state_array::~state_array() = default;

    constexpr
    state_array&
    state_array::operator=(state_array&&) = default;

    constexpr
    state_array&
    state_array::operator=(const state_array&) = default;

    constexpr
    state_array::const_reference
    state_array::operator[](size_type n)
    const
    {
        assume(
            n < size(), "index {0} is greater than or equal to size {1}",
            n, size());
        return impl_[n];
    }

    constexpr
    bool
    state_array::operator==(const state_array& array)
    const noexcept
    {
        return impl_ == array.impl_;
    }

    constexpr
    state_array::const_reference
    state_array::at(size_type n)
    const
    {
        verify(
            n < size(), "index {0} is greater than or equal to size {1}",
            n, size());
        return impl_[n];
    }

    constexpr
    state_array::const_iterator
    state_array::begin()
    const
    {
        return impl_.cbegin();
    }

    constexpr
    state_array::const_iterator
    state_array::cbegin()
    const
    {
        return impl_.cbegin();
    }

    constexpr
    state_array::const_iterator
    state_array::cend()
    const
    {
        return impl_.cend();
    }

    constexpr
    state_array::const_reverse_iterator
    state_array::crbegin()
    const
    {
        return impl_.crbegin();
    }

    constexpr
    state_array::const_reverse_iterator
    state_array::crend()
    const
    {
        return impl_.crend();
    }

    constexpr
    bool
    state_array::empty()
    const
    {
        return impl_.empty();
    }

    constexpr
    state_array::const_iterator
    state_array::end()
    const
    {
        return impl_.cend();
    }

    constexpr
    state_array::size_type
    state_array::max_size()
    const
    {
        return impl_.max_size();
    }

    constexpr
    state_array::const_reverse_iterator
    state_array::rbegin()
    const
    {
        return impl_.crbegin();
    }

    constexpr
    state_array::const_reverse_iterator
    state_array::rend()
    const
    {
        return impl_.crend();
    }

    constexpr
    state_array::size_type
    state_array::size()
    const
    {
        return impl_.size();
    }

    constexpr
    void
    state_array::swap(state_array& rhs)
    noexcept
    {
        impl_.swap(rhs.impl_);
    }

    /**
     * Gets a boolean indicating if the two instances compare inequal.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     *
     * @return
     *   The boolean indicator.
     */

    export
    constexpr
    bool
    operator!=(const state_array& lhs, const state_array& rhs)
    noexcept
    {
        return ! (lhs == rhs);
    }

    /**
     * Swaps the contents of the two instances.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     */

    export
    constexpr
    void
    swap(state_array& lhs, state_array& rhs)
    noexcept
    {
        lhs.swap(rhs);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::state_map (definition)
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    state_map::state_map() = default;

    state_map::state_map(state_map&&) = default;

    state_map::state_map(const state_map&) = default;

    template<std::input_iterator I, std::sentinel_for<I> S>
    requires (std::convertible_to<std::iter_value_t<I>, state_map::value_type>)
    state_map::state_map(I first, S last):
         impl_(first, last)
    {
        // empty
    }

    template<container_compatible_range<state_map::value_type> R>
    state_map::state_map(std::from_range_t, R&& range):
        impl_(std::ranges::begin(range), std::ranges::end(range))
    {
        // empty
    }

    inline
    state_map::state_map(std::initializer_list<value_type> list):
        impl_(list)
    {
        // empty
    }

    state_map::~state_map() = default;

    state_map&
    state_map::operator=(state_map&&) = default;

    state_map&
    state_map::operator=(const state_map&) = default;

    inline
    bool
    state_map::operator==(const state_map& map)
    const noexcept
    {
        return impl_ == map.impl_;
    }

    inline
    const state_map::mapped_type&
    state_map::at(const key_type& key)
    const
    {
        const auto iter = find(key);
        verify(
            iter != end(),
            "key {0:?} is not associated with a value in this state map", key);
        return iter->second;
    }

    inline
    state_map::const_iterator
    state_map::begin()
    const noexcept
    {
        return impl_.cbegin();
    }

    inline
    state_map::const_iterator
    state_map::cbegin()
    const noexcept
    {
        return impl_.cbegin();
    }

    inline
    state_map::const_iterator
    state_map::cend()
    const noexcept
    {
        return impl_.cend();
    }

    inline
    bool
    state_map::contains(const key_type& key)
    const
    {
        return impl_.contains(key);
    }

    inline
    state_map::const_reverse_iterator
    state_map::crbegin()
    const
    {
        return impl_.crbegin();
    }

    inline
    state_map::const_reverse_iterator
    state_map::crend()
    const
    {
        return impl_.crend();
    }

    inline
    bool
    state_map::empty()
    const noexcept
    {
        return impl_.empty();
    }

    inline
    state_map::const_iterator
    state_map::end()
    const noexcept
    {
        return impl_.cend();
    }

    inline
    state_map::const_iterator
    state_map::find(const key_type& key)
    const
    {
        return impl_.find(key);
    }

    inline
    state_map::size_type
    state_map::max_size()
    const noexcept
    {
        return impl_.max_size();
    }

    inline
    state_map::const_reverse_iterator
    state_map::rbegin()
    const
    {
        return impl_.crbegin();
    }

    inline
    state_map::const_reverse_iterator
    state_map::rend()
    const
    {
        return impl_.crend();
    }

    inline
    state_map::size_type
    state_map::size()
    const noexcept
    {
        return impl_.size();
    }

    inline
    void
    state_map::swap(state_map& rhs)
    noexcept
    {
        impl_.swap(rhs.impl_);
    }

    /**
     * Gets a boolean indicating if the two instances compare inequal.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     *
     * @return
     *   The boolean indicator.
     */

    export
    inline
    bool
    operator!=(const state_map& lhs, const state_map& rhs)
    noexcept
    {
        return ! (lhs == rhs);
    }

    /**
     * Swaps the contents of the two instances.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     */

    export
    inline
    void
    swap(state_map& lhs, state_map& rhs)
    noexcept
    {
        lhs.swap(rhs);
    }

}
