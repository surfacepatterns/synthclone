module;

#include <boost/test/unit_test.hpp>

export module synthclone.test:comparison;

import std;

import synthclone.util;

import :concepts;
import :debug;

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_ge()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T, class U>
    requires (! (ostream_printable<T> && ostream_printable<U>))
    void
    verify_ordered_ge(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ordered_ge", lhs, rhs));

        BOOST_REQUIRE(lhs >= rhs);
        BOOST_REQUIRE(rhs <= lhs);
        BOOST_REQUIRE(! (lhs < rhs));
        BOOST_REQUIRE(! (rhs > lhs));
    }

    template<ostream_printable T, ostream_printable U>
    void
    verify_ordered_ge(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ordered_ge", lhs, rhs));

        BOOST_REQUIRE_GE(lhs, rhs);
        BOOST_REQUIRE_LE(rhs, lhs);
        BOOST_REQUIRE(! (lhs < rhs));
        BOOST_REQUIRE(! (rhs > lhs));
    }

    template<class T, class U>
    void
    verify_three_way_ge(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_three_way_ge", lhs, rhs));

        verify_ordered_ge(lhs, rhs);

        BOOST_REQUIRE(std::is_gteq(lhs <=> rhs));
        BOOST_REQUIRE(std::is_lteq(rhs <=> lhs));
    }

    export
    template<class T, class U>
    requires (
        partially_ordered_with<T, U> &&
        (! std::three_way_comparable_with<T, U>)
    )
    void
    verify_ge(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ge", lhs, rhs));

        verify_ordered_ge(lhs, rhs);
    }

    export
    template<class T, class U>
    requires (std::three_way_comparable_with<T, U>)
    void
    verify_ge(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ge", lhs, rhs));

        verify_three_way_ge(lhs, rhs);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_le()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T, class U>
    requires (! (ostream_printable<T> && ostream_printable<U>))
    void
    verify_ordered_le(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ordered_le", lhs, rhs));

        BOOST_REQUIRE(lhs <= rhs);
        BOOST_REQUIRE(rhs >= lhs);
        BOOST_REQUIRE(! (lhs > rhs));
        BOOST_REQUIRE(! (rhs < lhs));
    }

    template<ostream_printable T, ostream_printable U>
    void
    verify_ordered_le(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ordered_le", lhs, rhs));

        BOOST_REQUIRE_LE(lhs, rhs);
        BOOST_REQUIRE_GE(rhs, lhs);
        BOOST_REQUIRE(! (lhs > rhs));
        BOOST_REQUIRE(! (rhs < lhs));
    }

    template<class T, class U>
    void
    verify_three_way_le(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_three_way_le", lhs, rhs));

        verify_ordered_le(lhs, rhs);

        BOOST_REQUIRE(std::is_lteq(lhs <=> rhs));
        BOOST_REQUIRE(std::is_gteq(rhs <=> lhs));
    }

    export
    template<class T, class U>
    requires (
        partially_ordered_with<T, U> &&
        (! std::three_way_comparable_with<T, U>)
    )
    void
    verify_le(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_le", lhs, rhs));

        verify_ordered_le(lhs, rhs);
    }

    export
    template<class T, class U>
    requires (std::three_way_comparable_with<T, U>)
    void
    verify_le(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_le", lhs, rhs));

        verify_three_way_le(lhs, rhs);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_ne()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T, class U>
    requires (! (ostream_printable<T> && ostream_printable<U>))
    void
    verify_equality_ne(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_equality_ne", lhs, rhs));

        BOOST_REQUIRE(lhs != rhs);
        BOOST_REQUIRE(rhs != lhs);
        BOOST_REQUIRE(! (lhs == rhs));
        BOOST_REQUIRE(! (rhs == lhs));
    }

    template<ostream_printable T, ostream_printable U>
    void
    verify_equality_ne(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_equality_ne", lhs, rhs));

        BOOST_REQUIRE_NE(lhs, rhs);
        BOOST_REQUIRE_NE(rhs, lhs);
        BOOST_REQUIRE(! (lhs == rhs));
        BOOST_REQUIRE(! (rhs == lhs));
    }

    template<class T, class U>
    void
    verify_three_way_ne(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_three_way_ne", lhs, rhs));

        verify_equality_ne(lhs, rhs);

        BOOST_REQUIRE(std::is_neq(lhs <=> rhs));
        BOOST_REQUIRE(std::is_neq(rhs <=> lhs));
    }

    export
    template<class T, class U>
    requires (
        weakly_equality_comparable_with<T, U> &&
        (! std::three_way_comparable_with<T, U>)
    )
    void
    verify_ne(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ne", lhs, rhs));

        verify_equality_ne(lhs, rhs);
    }

    export
    template<class T, class U>
    requires (std::three_way_comparable_with<T, U>)
    void
    verify_ne(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ne", lhs, rhs));

        verify_three_way_ne(lhs, rhs);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_eq()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T, class U>
    requires (! (ostream_printable<T> && ostream_printable<U>))
    void
    verify_equality_eq(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_equality_eq", lhs, rhs));

        BOOST_REQUIRE(lhs == rhs);
        BOOST_REQUIRE(rhs == lhs);
        BOOST_REQUIRE(! (lhs != rhs));
        BOOST_REQUIRE(! (rhs != lhs));
    }

    template<ostream_printable T, ostream_printable U>
    void
    verify_equality_eq(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_equality_eq", lhs, rhs));

        BOOST_REQUIRE_EQUAL(lhs, rhs);
        BOOST_REQUIRE_EQUAL(rhs, lhs);
        BOOST_REQUIRE(! (lhs != rhs));
        BOOST_REQUIRE(! (rhs != lhs));
    }

    template<class T, class U>
    void
    verify_ordered_eq(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ordered_eq", lhs, rhs));

        verify_ge(lhs, rhs);
        verify_le(lhs, rhs);
        verify_equality_eq(lhs, rhs);
    }

    template<class T, class U>
    void
    verify_three_way_eq(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_three_way_eq", lhs, rhs));

        verify_ordered_eq(lhs, rhs);

        BOOST_REQUIRE(std::is_eq(lhs <=> rhs));
        BOOST_REQUIRE(std::is_eq(rhs <=> lhs));
    }

    export
    template<class T, class U>
    requires (
        weakly_equality_comparable_with<T, U> &&
        (! partially_ordered_with<T, U>)
    )
    void
    verify_eq(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_eq", lhs, rhs));

        verify_equality_eq(lhs, rhs);
    }

    export
    template<class T, class U>
    requires (
        weakly_equality_comparable_with<T, U> &&
        partially_ordered_with<T, U> &&
        (! std::three_way_comparable_with<T, U>)
    )
    void
    verify_eq(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_eq", lhs, rhs));

        verify_ordered_eq(lhs, rhs);
    }

    export
    template<class T, class U>
    requires (std::three_way_comparable_with<T, U>)
    void
    verify_eq(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_eq", lhs, rhs));

        verify_three_way_eq(lhs, rhs);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_gt()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T, class U>
    requires (! (ostream_printable<T> && ostream_printable<U>))
    void
    verify_ordered_gt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ordered_gt", lhs, rhs));

        verify_ge(lhs, rhs);

        BOOST_REQUIRE(lhs > rhs);
        BOOST_REQUIRE(rhs < lhs);
        BOOST_REQUIRE(! (lhs <= rhs));
        BOOST_REQUIRE(! (rhs >= lhs));
    }

    template<ostream_printable T, ostream_printable U>
    void
    verify_ordered_gt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ordered_gt", lhs, rhs));

        verify_ge(lhs, rhs);

        BOOST_REQUIRE_GT(lhs, rhs);
        BOOST_REQUIRE_LT(rhs, lhs);
        BOOST_REQUIRE(! (lhs <= rhs));
        BOOST_REQUIRE(! (rhs >= lhs));
    }

    template<class T, class U>
    void
    verify_equality_gt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_equality_gt", lhs, rhs));

        verify_ne(lhs, rhs);
        verify_ordered_gt(lhs, rhs);
    }

    template<class T, class U>
    void
    verify_three_way_gt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_three_way_gt", lhs, rhs));

        verify_equality_gt(lhs, rhs);

        BOOST_REQUIRE(std::is_gt(lhs <=> rhs));
        BOOST_REQUIRE(std::is_lt(rhs <=> lhs));
    }

    export
    template<class T, class U>
    requires (
        partially_ordered_with<T, U> &&
        (! weakly_equality_comparable_with<T, U>)
    )
    void
    verify_gt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_gt", lhs, rhs));

        verify_ordered_gt(lhs, rhs);
    }

    export
    template<class T, class U>
    requires (
        partially_ordered_with<T, U> &&
        weakly_equality_comparable_with<T, U> &&
        (! std::three_way_comparable_with<T, U>)
    )
    void
    verify_gt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_gt", lhs, rhs));

        verify_equality_gt(lhs, rhs);
    }

    export
    template<class T, class U>
    requires (std::three_way_comparable_with<T, U>)
    void
    verify_gt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_gt", lhs, rhs));

        verify_three_way_gt(lhs, rhs);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_lt()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T, class U>
    requires (! (ostream_printable<T> && ostream_printable<U>))
    void
    verify_ordered_lt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ordered_lt", lhs, rhs));

        verify_le(lhs, rhs);

        BOOST_REQUIRE(lhs < rhs);
        BOOST_REQUIRE(rhs > lhs);
        BOOST_REQUIRE(! (lhs >= rhs));
        BOOST_REQUIRE(! (rhs <= lhs));
    }

    template<ostream_printable T, ostream_printable U>
    void
    verify_ordered_lt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_ordered_lt", lhs, rhs));

        verify_le(lhs, rhs);

        BOOST_REQUIRE_LT(lhs, rhs);
        BOOST_REQUIRE_GT(rhs, lhs);
        BOOST_REQUIRE(! (lhs >= rhs));
        BOOST_REQUIRE(! (rhs <= lhs));
    }

    template<class T, class U>
    void
    verify_equality_lt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_equality_lt", lhs, rhs));

        verify_ne(lhs, rhs);
        verify_ordered_lt(lhs, rhs);
    }

    template<class T, class U>
    void
    verify_three_way_lt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_three_way_lt", lhs, rhs));

        verify_equality_lt(lhs, rhs);

        BOOST_REQUIRE(std::is_lt(lhs <=> rhs));
        BOOST_REQUIRE(std::is_gt(rhs <=> lhs));
    }

    export
    template<class T, class U>
    requires (
        partially_ordered_with<T, U> &&
        (! weakly_equality_comparable_with<T, U>)
    )
    void
    verify_lt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_lt", lhs, rhs));

        verify_ordered_lt(lhs, rhs);
    }

    export
    template<class T, class U>
    requires (
        partially_ordered_with<T, U> &&
        weakly_equality_comparable_with<T, U> &&
        (! std::three_way_comparable_with<T, U>)
    )
    void
    verify_lt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_lt", lhs, rhs));

        verify_equality_lt(lhs, rhs);
    }

    export
    template<class T, class U>
    requires (std::three_way_comparable_with<T, U>)
    void
    verify_lt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_lt", lhs, rhs));

        verify_three_way_lt(lhs, rhs);
    }

}
