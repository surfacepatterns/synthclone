#include <format>
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include <boost/test/unit_test.hpp>

import synthclone.util;

namespace {

    class string_proxy final: public synthclone::string_proxy<string_proxy> {

    public:

        template<class... Args>
        explicit
        string_proxy(Args&&... args):
            synthclone::string_proxy<string_proxy>(std::forward<Args>(args)...)
        {
            // empty
        }

    };

    template<class T, class U>
    void
    verify_eq(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            (std::format("verify_eq({0:?}, {1:?})", lhs, rhs)));

        BOOST_CHECK(lhs == rhs);
        BOOST_CHECK(! (lhs != rhs));
        BOOST_CHECK(lhs >= rhs);
        BOOST_CHECK(! (lhs > rhs));
        BOOST_CHECK(lhs <= rhs);
        BOOST_CHECK(! (lhs < rhs));

        BOOST_CHECK(rhs == lhs);
        BOOST_CHECK(! (rhs != lhs));
        BOOST_CHECK(rhs >= lhs);
        BOOST_CHECK(! (rhs > lhs));
        BOOST_CHECK(rhs <= lhs);
        BOOST_CHECK(! (rhs < lhs));
    }

    void
    verify_eq_types(const string_proxy& lhs, const string_proxy& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            (std::format("verify_eq_types({0:?}, {1:?})", lhs, rhs)));

        verify_eq(lhs, rhs);

        verify_eq(lhs, rhs.base());
        verify_eq(lhs, static_cast<std::string>(rhs));
        verify_eq(lhs, static_cast<std::string_view>(rhs));
        verify_eq(lhs, rhs.base().c_str());

        verify_eq(lhs.base(), rhs);
        verify_eq(static_cast<std::string>(lhs), rhs);
        verify_eq(static_cast<std::string_view>(lhs), rhs);
        verify_eq(lhs.base().c_str(), rhs);
    }

    void
    verify_formatting(const std::string& s)
    {
        BOOST_TEST_INFO_SCOPE((std::format("verify_formatting({0:?})", s)));

        auto value_string = std::format("abc{0}", s);

        string_proxy proxy(s);
        auto proxy_string = std::format("abc{0}", proxy);

        BOOST_TEST(value_string == proxy_string);
    }

    template<class T, class U>
    void
    verify_gt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            (std::format("verify_gt({0:?}, {1:?})", lhs, rhs)));

        BOOST_CHECK(! (lhs == rhs));
        BOOST_CHECK(lhs != rhs);
        BOOST_CHECK(lhs >= rhs);
        BOOST_CHECK(lhs > rhs);
        BOOST_CHECK(! (lhs <= rhs));
        BOOST_CHECK(! (lhs < rhs));

        BOOST_CHECK(! (rhs == lhs));
        BOOST_CHECK(rhs != lhs);
        BOOST_CHECK(! (rhs >= lhs));
        BOOST_CHECK(! (rhs > lhs));
        BOOST_CHECK(rhs <= lhs);
        BOOST_CHECK(rhs < lhs);
    }

    void
    verify_gt_types(const string_proxy& lhs, const string_proxy& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            (std::format("verify_gt_types({0:?}, {1:?})", lhs, rhs)));

        verify_gt(lhs, rhs);

        verify_gt(lhs, rhs.base());
        verify_gt(lhs, static_cast<std::string>(rhs));
        verify_gt(lhs, static_cast<std::string_view>(rhs));
        verify_gt(lhs, rhs.base().c_str());

        verify_gt(lhs.base(), rhs);
        verify_gt(static_cast<std::string>(lhs), rhs);
        verify_gt(static_cast<std::string_view>(lhs), rhs);
        verify_gt(lhs.base().c_str(), rhs);
    }

    void
    verify_hashing(const std::string& s)
    {
        BOOST_TEST_INFO_SCOPE((std::format("verify_hashing({0:?})", s)));

        auto value_hash = std::hash<std::string>{}(s);

        string_proxy proxy(s);
        auto proxy_hash = std::hash<string_proxy>{}(proxy);

        BOOST_TEST(value_hash == proxy_hash);
    }

    template<class I, class S, class C>
    void
    verify_iterator_value(I& iter, S sentinel, C value)
    {
        BOOST_TEST_INFO_SCOPE(
            (
                std::format(
                    "verify_iterator_value({0:x}, {1:x}, {2:?})",
                    reinterpret_cast<std::uintptr_t>(std::to_address(iter)),
                    reinterpret_cast<std::uintptr_t>(
                        std::to_address(sentinel)),
                    value)
            ));

        BOOST_REQUIRE(iter != sentinel);
        BOOST_CHECK(*iter == value);
        ++iter;
    }

    template<class T, class U>
    void
    verify_lt(const T& lhs, const U& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            (std::format("verify_lt({0:?}, {1:?})", lhs, rhs)));

        BOOST_CHECK(! (lhs == rhs));
        BOOST_CHECK(lhs != rhs);
        BOOST_CHECK(! (lhs >= rhs));
        BOOST_CHECK(! (lhs > rhs));
        BOOST_CHECK(lhs <= rhs);
        BOOST_CHECK(lhs < rhs);

        BOOST_CHECK(! (rhs == lhs));
        BOOST_CHECK(rhs != lhs);
        BOOST_CHECK(rhs >= lhs);
        BOOST_CHECK(rhs > lhs);
        BOOST_CHECK(! (rhs <= lhs));
        BOOST_CHECK(! (rhs < lhs));
    }

    void
    verify_lt_types(const string_proxy& lhs, const string_proxy& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            (std::format("verify_lt_types({0:?}, {1:?})", lhs, rhs)));

        verify_lt(lhs, rhs);

        verify_lt(lhs, rhs.base());
        verify_lt(lhs, static_cast<std::string>(rhs));
        verify_lt(lhs, static_cast<std::string_view>(rhs));
        verify_lt(lhs, rhs.base().c_str());

        verify_lt(lhs.base(), rhs);
        verify_lt(static_cast<std::string>(lhs), rhs);
        verify_lt(static_cast<std::string_view>(lhs), rhs);
        verify_lt(lhs.base().c_str(), rhs);
    }

}

BOOST_AUTO_TEST_SUITE(string)

BOOST_AUTO_TEST_CASE(concat_strings)
{
    BOOST_CHECK(synthclone::concat_strings("", "", "", "") == "");
    BOOST_CHECK(
        synthclone::concat_strings("foo", "bar", "baz") == "foobarbaz");

    BOOST_CHECK(
        synthclone::concat_strings(
            std::string("foo"), std::string_view("bar"), "baz") ==
        "foobarbaz");
}

BOOST_AUTO_TEST_CASE(string_proxy_assignment_and_construction)
{
    {
        string_proxy s1("stuff");
        string_proxy s2(s1);

        BOOST_CHECK(s1 == s2);
    }

    {
        string_proxy s1("stuff");
        string_proxy s2(std::move(s1));

        BOOST_CHECK(s2 == "stuff");
    }

    {
        string_proxy s1("stuff");
        string_proxy s2("foo");
        s2 = s1;

        BOOST_CHECK(s1 == s2);
    }

    {
        string_proxy s1("stuff");
        string_proxy s2("foo");
        s2 = std::move(s1);

        BOOST_CHECK(s2 == "stuff");
    }
}

BOOST_AUTO_TEST_CASE(string_proxy_comparisons)
{
    {
        string_proxy s1("stuff");
        string_proxy s2("stuff");

        verify_eq_types(s1, s2);
    }

    {
        string_proxy s1("");
        string_proxy s2("");

        verify_eq_types(s1, s2);
    }

    {
        string_proxy s1("apple");
        string_proxy s2("apples");

        verify_lt_types(s1, s2);
        verify_gt_types(s2, s1);
    }

    {
        string_proxy s1("oranges");
        string_proxy s2("apples");

        verify_gt_types(s1, s2);
        verify_lt_types(s2, s1);
    }
}

BOOST_AUTO_TEST_CASE(string_proxy_formatting)
{
    verify_formatting("stuff");
    verify_formatting("");
}

BOOST_AUTO_TEST_CASE(string_proxy_hashing)
{
    verify_hashing("stuff");
    verify_hashing("");
}

BOOST_AUTO_TEST_CASE(string_proxy_ostream_output)
{
    string_proxy s("stuff");
    std::ostringstream stream;
    stream << s;

    BOOST_CHECK(s == stream.str());
}

BOOST_AUTO_TEST_CASE(string_proxy_typical_lifetime)
{
    string_proxy s("stuff");

    BOOST_CHECK(s[0] == 's');
    BOOST_CHECK(s[1] == 't');
    BOOST_CHECK(s[2] == 'u');
    BOOST_CHECK(s[3] == 'f');
    BOOST_CHECK(s[4] == 'f');

    BOOST_CHECK(s.at(0) == 's');
    BOOST_CHECK(s.at(1) == 't');
    BOOST_CHECK(s.at(2) == 'u');
    BOOST_CHECK(s.at(3) == 'f');
    BOOST_CHECK(s.at(4) == 'f');
    BOOST_CHECK_THROW(s.at(5), std::out_of_range);

    {
        auto end = s.end();
        auto iter = s.begin();

        verify_iterator_value(iter, end, 's');
        verify_iterator_value(iter, end, 't');
        verify_iterator_value(iter, end, 'u');
        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'f');
        BOOST_CHECK(iter == end);
    }

    {
        auto end = s.cend();
        auto iter = s.cbegin();

        verify_iterator_value(iter, end, 's');
        verify_iterator_value(iter, end, 't');
        verify_iterator_value(iter, end, 'u');
        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'f');
        BOOST_CHECK(iter == end);
    }

    {
        auto end = s.rend();
        auto iter = s.rbegin();

        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'u');
        verify_iterator_value(iter, end, 't');
        verify_iterator_value(iter, end, 's');
        BOOST_CHECK(iter == end);
    }

    {
        auto end = s.crend();
        auto iter = s.crbegin();

        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'u');
        verify_iterator_value(iter, end, 't');
        verify_iterator_value(iter, end, 's');
        BOOST_CHECK(iter == end);
    }

    BOOST_CHECK(s.back() == 'f');

    BOOST_CHECK(s.base() == "stuff");

    BOOST_CHECK(s.data() == s.base().data());

    BOOST_CHECK(! s.empty());

    BOOST_CHECK(s.front() == 's');

    BOOST_CHECK(s.max_size() == s.base().max_size());

    BOOST_CHECK(s.size() == 5);

    string_proxy s2("");

    BOOST_CHECK_THROW(s2.at(0), std::out_of_range);

    BOOST_CHECK(s2.begin() == s2.end());
    BOOST_CHECK(s2.cbegin() == s2.cend());
    BOOST_CHECK(s2.rbegin() == s2.rend());
    BOOST_CHECK(s2.crbegin() == s2.crend());

    BOOST_CHECK(s2.base() == "");

    BOOST_CHECK(s2.data() == s2.base().data());

    BOOST_CHECK(s2.empty());

    BOOST_CHECK(s2.max_size() == s2.base().max_size());

    BOOST_CHECK(s2.size() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
