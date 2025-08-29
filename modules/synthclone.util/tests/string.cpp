#include <boost/test/unit_test.hpp>

import std;

import synthclone.test;
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

    void
    verify_eq_types(const string_proxy& lhs, const string_proxy& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            (std::format("verify_eq_types({0:?}, {1:?})", lhs, rhs)));

        synthclone::verify_eq(lhs, rhs);

        synthclone::verify_eq(lhs, rhs.base());
        synthclone::verify_eq(lhs, static_cast<std::string>(rhs));
        synthclone::verify_eq(lhs, static_cast<std::string_view>(rhs));
        synthclone::verify_eq(lhs, rhs.base().c_str());

        synthclone::verify_eq(lhs.base(), rhs);
        synthclone::verify_eq(static_cast<std::string>(lhs), rhs);
        synthclone::verify_eq(static_cast<std::string_view>(lhs), rhs);
        synthclone::verify_eq(lhs.base().c_str(), rhs);
    }

    void
    verify_formatting(const std::string& s)
    {
        BOOST_TEST_INFO_SCOPE((std::format("verify_formatting({0:?})", s)));

        auto value_string = std::format("abc{0}", s);

        string_proxy proxy(s);
        auto proxy_string = std::format("abc{0}", proxy);

        synthclone::verify_eq(value_string, proxy_string);
    }

    void
    verify_gt_types(const string_proxy& lhs, const string_proxy& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            (std::format("verify_gt_types({0:?}, {1:?})", lhs, rhs)));

        synthclone::verify_gt(lhs, rhs);

        synthclone::verify_gt(lhs, rhs.base());
        synthclone::verify_gt(lhs, static_cast<std::string>(rhs));
        synthclone::verify_gt(lhs, static_cast<std::string_view>(rhs));
        synthclone::verify_gt(lhs, rhs.base().c_str());

        synthclone::verify_gt(lhs.base(), rhs);
        synthclone::verify_gt(static_cast<std::string>(lhs), rhs);
        synthclone::verify_gt(static_cast<std::string_view>(lhs), rhs);
        synthclone::verify_gt(lhs.base().c_str(), rhs);
    }

    void
    verify_hashing(const std::string& s)
    {
        BOOST_TEST_INFO_SCOPE((std::format("verify_hashing({0:?})", s)));

        auto value_hash = std::hash<std::string>{}(s);

        string_proxy proxy(s);
        auto proxy_hash = std::hash<string_proxy>{}(proxy);

        synthclone::verify_eq(value_hash, proxy_hash);
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
        synthclone::verify_eq(*iter, value);
        ++iter;
    }

    void
    verify_lt_types(const string_proxy& lhs, const string_proxy& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            (std::format("verify_lt_types({0:?}, {1:?})", lhs, rhs)));

        synthclone::verify_lt(lhs, rhs);

        synthclone::verify_lt(lhs, rhs.base());
        synthclone::verify_lt(lhs, static_cast<std::string>(rhs));
        synthclone::verify_lt(lhs, static_cast<std::string_view>(rhs));
        synthclone::verify_lt(lhs, rhs.base().c_str());

        synthclone::verify_lt(lhs.base(), rhs);
        synthclone::verify_lt(static_cast<std::string>(lhs), rhs);
        synthclone::verify_lt(static_cast<std::string_view>(lhs), rhs);
        synthclone::verify_lt(lhs.base().c_str(), rhs);
    }

}

BOOST_AUTO_TEST_SUITE(string)

BOOST_AUTO_TEST_CASE(concat_strings)
{
    synthclone::verify_eq(synthclone::concat_strings("", "", "", ""), "");
    synthclone::verify_eq(
        synthclone::concat_strings("foo", "bar", "baz"), "foobarbaz");

    synthclone::verify_eq(
        synthclone::concat_strings(
            std::string("foo"), std::string_view("bar"), "baz"),
        "foobarbaz");
}

BOOST_AUTO_TEST_CASE(string_proxy_assignment_and_construction)
{
    {
        string_proxy s1("stuff");
        string_proxy s2(s1);

        synthclone::verify_eq(s1, s2);
    }

    {
        string_proxy s1("stuff");
        string_proxy s2(std::move(s1));

        synthclone::verify_eq(s2, "stuff");
    }

    {
        string_proxy s1("stuff");
        string_proxy s2("foo");
        s2 = s1;

        synthclone::verify_eq(s1, s2);
    }

    {
        string_proxy s1("stuff");
        string_proxy s2("foo");
        s2 = std::move(s1);

        synthclone::verify_eq(s2, "stuff");
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

    synthclone::verify_eq(s, stream.str());
}

BOOST_AUTO_TEST_CASE(string_proxy_typical_lifetime)
{
    string_proxy s("stuff");

    synthclone::verify_eq(s[0], 's');
    synthclone::verify_eq(s[1], 't');
    synthclone::verify_eq(s[2], 'u');
    synthclone::verify_eq(s[3], 'f');
    synthclone::verify_eq(s[4], 'f');

    synthclone::verify_eq(s.at(0), 's');
    synthclone::verify_eq(s.at(1), 't');
    synthclone::verify_eq(s.at(2), 'u');
    synthclone::verify_eq(s.at(3), 'f');
    synthclone::verify_eq(s.at(4), 'f');
    BOOST_CHECK_THROW(s.at(5), std::out_of_range);

    {
        auto end = s.end();
        auto iter = s.begin();

        verify_iterator_value(iter, end, 's');
        verify_iterator_value(iter, end, 't');
        verify_iterator_value(iter, end, 'u');
        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'f');
        synthclone::verify_eq(iter, end);
    }

    {
        auto end = s.cend();
        auto iter = s.cbegin();

        verify_iterator_value(iter, end, 's');
        verify_iterator_value(iter, end, 't');
        verify_iterator_value(iter, end, 'u');
        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'f');
        synthclone::verify_eq(iter, end);
    }

    {
        auto end = s.rend();
        auto iter = s.rbegin();

        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'u');
        verify_iterator_value(iter, end, 't');
        verify_iterator_value(iter, end, 's');
        synthclone::verify_eq(iter, end);
    }

    {
        auto end = s.crend();
        auto iter = s.crbegin();

        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'f');
        verify_iterator_value(iter, end, 'u');
        verify_iterator_value(iter, end, 't');
        verify_iterator_value(iter, end, 's');
        synthclone::verify_eq(iter, end);
    }

    synthclone::verify_eq(s.back(), 'f');

    synthclone::verify_eq(s.base(), "stuff");

    synthclone::verify_eq(s.data(), s.base().data());

    BOOST_CHECK(! s.empty());

    synthclone::verify_eq(s.front(), 's');

    synthclone::verify_eq(s.max_size(), s.base().max_size());

    synthclone::verify_eq(s.size(), 5);

    string_proxy s2("");

    BOOST_CHECK_THROW(s2.at(0), std::out_of_range);

    synthclone::verify_eq(s2.begin(), s2.end());
    synthclone::verify_eq(s2.cbegin(), s2.cend());
    synthclone::verify_eq(s2.rbegin(), s2.rend());
    synthclone::verify_eq(s2.crbegin(), s2.crend());

    synthclone::verify_eq(s2.base(), "");

    synthclone::verify_eq(s2.data(), s2.base().data());

    BOOST_CHECK(s2.empty());

    synthclone::verify_eq(s2.max_size(), s2.base().max_size());

    synthclone::verify_eq(s2.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
