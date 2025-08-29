#include <boost/test/unit_test.hpp>

import std;

import synthclone.test;
import synthclone.util;

namespace {

    template<synthclone::scalar S>
    class scalar_proxy final:
        public synthclone::basic_scalar_proxy<scalar_proxy<S>, S> {

    public:

        explicit
        scalar_proxy(S value):
            synthclone::basic_scalar_proxy<scalar_proxy<S>, S>(value)
        {
            // empty
        }

    };

    enum class test_enum {
        value = 0,
        other_value = 1
    };

    template<class S>
    void
    test_arithmetic_comparisons()
    {
        BOOST_TEST_INFO_SCOPE("test_arithmetic_comparisons()");

        S value = std::numeric_limits<S>::max() - 1;
        S value_dec = value - 1;
        S value_inc = value + 1;

        scalar_proxy<S> proxy(value);
        scalar_proxy<S> proxy_copy(proxy);
        scalar_proxy<S> proxy_dec(value_dec);
        scalar_proxy<S> proxy_inc(value_inc);

        scalar_proxy<S> proxy_copy_2(0);
        synthclone::verify_eq(proxy_copy_2, static_cast<S>(0));
        proxy_copy_2 = proxy;

        synthclone::verify_gt(proxy, value_dec);
        synthclone::verify_gt(proxy, proxy_dec);

        synthclone::verify_eq(proxy, value);
        synthclone::verify_eq(proxy, proxy_copy);
        synthclone::verify_eq(proxy, proxy_copy_2);

        synthclone::verify_lt(proxy, value_inc);
        synthclone::verify_lt(proxy, proxy_inc);
    }

    template<class S, class T, S Value = static_cast<S>(0)>
    void
    test_explicit_conversion()
    {
        BOOST_TEST_INFO_SCOPE("test_explicit_conversion()");

        S value = Value;
        scalar_proxy<S> proxy(value);
        auto n = static_cast<T>(proxy);

        synthclone::verify_eq(n , (static_cast<T>(value)));
    }

    template<class S, S Value = static_cast<S>(0)>
    void
    test_formatting()
    {
        BOOST_TEST_INFO_SCOPE("test_formatting()");

        S value = Value;
        auto value_string = std::format("abc{0}", value);

        scalar_proxy<S> proxy(value);
        auto proxy_string = std::format("abc{0}", proxy);

        synthclone::verify_eq(value_string, proxy_string);
    }

    template<class S, S Value = static_cast<S>(0)>
    void
    test_hashing()
    {
        BOOST_TEST_INFO_SCOPE("test_hashing()");

        S value = Value;
        auto value_hash = std::hash<S>{}(value);

        scalar_proxy<S> proxy(value);
        auto proxy_hash = std::hash<scalar_proxy<S>>{}(proxy);

        synthclone::verify_eq(value_hash, proxy_hash);
    }

    template<class S, class T = S, S Value = static_cast<S>(0)>
    void
    test_implicit_conversion()
    {
        BOOST_TEST_INFO_SCOPE("test_implicit_conversion()");

        S value = Value;
        scalar_proxy<S> proxy(value);
        T n = proxy;

        synthclone::verify_eq(proxy, value);
        synthclone::verify_eq(n , (static_cast<T>(value)));
    }

    template<class S, S Value = static_cast<S>(0)>
    void
    test_ostream_output()
    {
        BOOST_TEST_INFO_SCOPE("test_ostream_output()");

        S value = Value;
        std::ostringstream value_stream;
        value_stream << value;

        scalar_proxy<S> proxy(value);
        std::ostringstream proxy_stream;
        proxy_stream << proxy;

        synthclone::verify_eq(value_stream.str(), proxy_stream.str());
    }

}

BOOST_AUTO_TEST_SUITE(scalar)

BOOST_AUTO_TEST_CASE(enum_comparisons)
{
    test_enum zero_value = test_enum::value;
    test_enum one_value = test_enum::other_value;

    scalar_proxy<test_enum> zero_proxy(zero_value);
    scalar_proxy<test_enum> one_proxy(one_value);

    synthclone::verify_gt(one_proxy, zero_proxy);
    synthclone::verify_lt(zero_proxy, one_proxy);

    synthclone::verify_gt(one_proxy, zero_value);
    synthclone::verify_lt(zero_proxy, one_value);

    scalar_proxy<test_enum> zero_proxy_copy(zero_proxy);

    synthclone::verify_eq(zero_proxy, zero_proxy_copy);
}

BOOST_AUTO_TEST_CASE(enum_explicit_conversion)
{
    test_explicit_conversion<test_enum, short>();
    test_explicit_conversion<test_enum, short, test_enum::other_value>();
}

BOOST_AUTO_TEST_CASE(enum_formatting)
{
    BOOST_CHECK((! std::formattable<test_enum, char>));
}

BOOST_AUTO_TEST_CASE(enum_hashing)
{
    test_hashing<test_enum>();
    test_hashing<test_enum, test_enum::other_value>();
}

BOOST_AUTO_TEST_CASE(enum_implicit_conversion)
{
    test_implicit_conversion<test_enum>();
}

BOOST_AUTO_TEST_CASE(long_comparisons)
{
    test_arithmetic_comparisons<long>();
}

BOOST_AUTO_TEST_CASE(long_explicit_conversion)
{
    test_explicit_conversion<long, test_enum>();
    test_explicit_conversion<long, test_enum, 1>();
}

BOOST_AUTO_TEST_CASE(long_formatting)
{
    test_formatting<long>();
    test_formatting<long, 0x7ffffffe>();
}

BOOST_AUTO_TEST_CASE(long_hashing)
{
    test_hashing<long>();
    test_hashing<long, 0x7ffffffe>();
}

BOOST_AUTO_TEST_CASE(long_implicit_conversion)
{
    test_implicit_conversion<long>();
    test_implicit_conversion<long, long long>();
    test_implicit_conversion<long, unsigned short>();
}

BOOST_AUTO_TEST_CASE(long_ostream_output)
{
    test_ostream_output<long>();
    test_ostream_output<long, 0x7fffffff>();
}

BOOST_AUTO_TEST_CASE(short_comparisons)
{
    test_arithmetic_comparisons<short>();
}

BOOST_AUTO_TEST_CASE(short_explicit_conversion)
{
    test_explicit_conversion<short, test_enum>();
    test_explicit_conversion<short, test_enum, 1>();
}

BOOST_AUTO_TEST_CASE(short_formatting)
{
    test_formatting<short>();
    test_formatting<short, 0x7ffe>();
}

BOOST_AUTO_TEST_CASE(short_hashing)
{
    test_hashing<short>();
    test_hashing<short, 0x7ffe>();
}

BOOST_AUTO_TEST_CASE(short_implicit_conversion)
{
    test_implicit_conversion<short>();
    test_implicit_conversion<short, long>();
    test_implicit_conversion<short, unsigned short>();
}

BOOST_AUTO_TEST_CASE(short_ostream_output)
{
    test_ostream_output<short>();
    test_ostream_output<short, 0x7fff>();
}

BOOST_AUTO_TEST_SUITE_END()
