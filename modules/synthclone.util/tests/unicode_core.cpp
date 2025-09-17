#include <boost/test/unit_test.hpp>

import std;

import synthclone.test;
import synthclone.util;

namespace {

    void
    verify_codepoint_category(
        const synthclone::unicode_codepoint codepoint,
        const synthclone::unicode_category expected_category
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_codepoint_category",
                static_cast<std::uint_least32_t>(codepoint),
                synthclone::get_identifier(expected_category)));

        synthclone::verify_eq(
            expected_category, synthclone::get_category(codepoint));
    }

    void
    verify_invalid_codepoint(char32_t n)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_invalid_codepoint",
                static_cast<std::uint_least32_t>(n)));

        BOOST_CHECK_THROW(
            synthclone::unicode_codepoint{n}, synthclone::verification_error);
    }

    void
    verify_invalid_utf8(const std::string& s)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info("verify_invalid_utf8", s));

        BOOST_CHECK_THROW(
            synthclone::utf8_string{s}, synthclone::unicode_error);
    }

    void
    verify_invalid_utf8_line(const std::string& s)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info("verify_invalid_utf8_line", s));

        BOOST_CHECK_THROW(synthclone::utf8_line{s}, synthclone::unicode_error);
    }

    void
    verify_line_terminator(char32_t n)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_line_terminator",
                static_cast<std::uint_least32_t>(n)));

        BOOST_CHECK(
            synthclone::is_line_terminator(synthclone::unicode_codepoint(n)));
    }

    void
    verify_non_line_terminator(char32_t n)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_non_line_terminator",
                static_cast<std::uint_least32_t>(n)));

        BOOST_CHECK(
            ! synthclone::is_line_terminator(
                synthclone::unicode_codepoint(n)));
    }

    void
    verify_valid_codepoint(char32_t n)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_valid_codepoint",
                static_cast<std::uint_least32_t>(n)));

        BOOST_CHECK_NO_THROW(synthclone::unicode_codepoint{n});
    }

    template<std::convertible_to<synthclone::unicode_codepoint>... Args>
    void
    verify_valid_utf8(const std::string& s, Args&&... args)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_valid_utf8", s, std::forward<Args>(args)...));

        BOOST_CHECK_NO_THROW(synthclone::utf8_string{s});

        const auto view = synthclone::decode_utf8(std::as_bytes(std::span(s)));
        synthclone::verify_range_elements_eq(
            view,
            static_cast<synthclone::unicode_codepoint>(
                std::forward<Args>(args))...);
    }

}

BOOST_AUTO_TEST_SUITE(unicode_core)

// Many of these tests are based on tests in `utf8core`.

BOOST_AUTO_TEST_CASE(codepoint_categories)
{
    for (char c = 'a'; c <= 'z'; ++c) {
        verify_codepoint_category(
            c, synthclone::unicode_category::lowercase_letter);
    }
    for (char c = 'A'; c <= 'Z'; ++c) {
        verify_codepoint_category(
            c, synthclone::unicode_category::uppercase_letter);
    }
    for (char c = '0'; c <= '9'; ++c) {
        verify_codepoint_category(
            c, synthclone::unicode_category::decimal_number);
    }
}

BOOST_AUTO_TEST_CASE(codepoint_line_terminators)
{
    verify_line_terminator('\n');
    verify_line_terminator('\v');
    verify_line_terminator('\r');
    verify_line_terminator('\f');
    verify_line_terminator(U'\U00000085');
    verify_line_terminator(U'\U00002028');
    verify_line_terminator(U'\U00002029');

    for (char32_t i = 0x20; i < 0x80; ++i) {
        verify_non_line_terminator(i);
    }
}

BOOST_AUTO_TEST_CASE(invalid_utf8_decode_iterator)
{
    synthclone::utf8_decode_iterator invalid_iter;

    const std::string s("foo");
    synthclone::verify_ne(
        invalid_iter,
        synthclone::decode_utf8(std::as_bytes(std::span(s))).begin());
}

BOOST_AUTO_TEST_CASE(invalid_unicode_codepoints)
{
    for (char32_t c = 0xd800; c < 0xe000; ++c) {
        verify_invalid_codepoint(c);
    }
    for (char32_t c = 0x110000; c < 0x110010; ++c) {
        verify_invalid_codepoint(c);
    }
}

BOOST_AUTO_TEST_CASE(invalid_utf8_lines)
{
    verify_invalid_utf8_line("foo\n");
    verify_invalid_utf8_line("foo\v");
    verify_invalid_utf8_line("foo\f");
    verify_invalid_utf8_line("foo\r");
    verify_invalid_utf8_line("foo\U00000085");
    verify_invalid_utf8_line("foo\U00002028");
    verify_invalid_utf8_line("foo\U00002029");
}

BOOST_AUTO_TEST_CASE(invalid_utf8_strings)
{
    // continuation byte not after lead
    for (auto i = 0x80; i < 0xc0; ++i) {
        verify_invalid_utf8(std::string {static_cast<char>(i)});
    }

    for (auto i = 0xc0; i <= 0xff; ++i) {
        auto c = static_cast<char>(i);

        // single lead byte at end of string
        verify_invalid_utf8(std::string {c});

        // lead followed by non-continuation character < 0x80
        verify_invalid_utf8(std::string {c, 65});

        // lead followed by non-continuation character > 0xbf
        verify_invalid_utf8(std::string {c, '\xc0'});
    }

    // overlong 2-byte
    for (auto i = 0x81; i <= 0xbf; ++i) {
        verify_invalid_utf8(std::string {'\xc0', static_cast<char>(i)});
    }
    for (auto i = 0x80; i <= 0xbf; ++i) {
        verify_invalid_utf8(std::string {'\xc1', static_cast<char>(i)});
    }

    // overlong 3-byte
    for (auto i = 0x81; i <= 0x9f; ++i) {
        verify_invalid_utf8(
            std::string {'\xe0', static_cast<char>(i), '\x80'});
    }

    // overlong 4-byte
    for (auto i = 0x81; i <= 0x8f; ++i) {
        verify_invalid_utf8(
            std::string {'\xf0', static_cast<char>(i), '\x80', '\x80'});
    }

    // 4-byte > 0x10ffff
    for (auto i = 0x90; i <= 0xbf; ++i) {
        verify_invalid_utf8(
            std::string {'\xf4', static_cast<char>(i), '\x80', '\x80'});
    }
    for (auto i = 0xf5; i <= 0xf7; ++i) {
        verify_invalid_utf8(
            std::string {static_cast<char>(i), '\x80', '\x80', '\x80'});
    }

    // 5-byte
    for (auto i = 0xf8; i <= 0xfb; ++i) {
        verify_invalid_utf8(
            std::string {
                static_cast<char>(i),
                '\x80',
                '\x80',
                '\x80',
                '\x80'
            });
    }

    // 6-byte
    for (auto i = 0xfc; i <= 0xfd; ++i) {
        verify_invalid_utf8(
            std::string {
                static_cast<char>(i),
                '\x80',
                '\x80',
                '\x80',
                '\x80',
                '\x80'
            });
    }

    // 7-byte
    verify_invalid_utf8(
        std::string {'\xfe', '\x80', '\x80', '\x80', '\x80', '\x80', '\x80'});

    for (auto i = 0xe0; i < 0xf0; ++i) {

        // lead followed by only 1 continuation byte
        verify_invalid_utf8(std::string {static_cast<char>(i), '\x80'});

        // lead ended by non-continuous character < 0x80
        verify_invalid_utf8(std::string {static_cast<char>(i), '\x80', 65});

        // lead ended by non-continuous character > 0xbf
        verify_invalid_utf8(
            std::string {static_cast<char>(i), '\x80', '\xc0'});
    }

    // single surrogate
    verify_invalid_utf8(std::string {'\xed', '\xa0', '\x80'});

    // trailing surrogate first
    verify_invalid_utf8(std::string {'\xed', '\xb0', '\x80'});

    for (auto i = 0xf0; i < 0xf5; ++i) {

        // lead followed by only 1 continuation byte
        verify_invalid_utf8(std::string {static_cast<char>(i), '\x80'});

        // lead followed by only 2 continuation bytes
        verify_invalid_utf8(
            std::string {static_cast<char>(i), '\x80', '\x80'});

        // lead followed by non-continuous character < 0x80
        verify_invalid_utf8(
            std::string {static_cast<char>(i), '\x80', '\x80', 65});

        // lead followed by non-continuous character > 0xbf
        verify_invalid_utf8(
            std::string {static_cast<char>(i), '\x80', '\x80', '\xc0'});
    }
}

BOOST_AUTO_TEST_CASE(valid_unicode_codepoints)
{
    for (char32_t c = 0; c < 0xd800; ++c) {
        verify_valid_codepoint(c);
    }
    for (char32_t c = 0xe000; c < 0x110000; ++c) {
        verify_valid_codepoint(c);
    }
}

BOOST_AUTO_TEST_CASE(valid_utf8_lines)
{
    synthclone::utf8_line line_1("foo");
    synthclone::verify_eq(line_1, "foo");

    synthclone::utf8_line line_2("bar");
    synthclone::verify_eq(line_2, "bar");

    synthclone::verify_ne(line_1, line_2);

    synthclone::utf8_line line_3(line_1);
    synthclone::verify_eq(line_1, line_3);

    synthclone::utf8_line line_4(std::move(line_1));
    synthclone::verify_eq(line_3, line_4);

    line_1 = line_2;
    synthclone::verify_eq(line_1, line_2);

    line_4 = std::move(line_1);
    synthclone::verify_eq(line_2, line_4);
}

BOOST_AUTO_TEST_CASE(valid_utf8_strings)
{
    for (auto i = 0; i < 0x10; ++i) {
        verify_valid_utf8(
            std::string {'\xef', '\xb7', static_cast<char>(0x90 + i)},
            0xfdd0 + i);
    }

    verify_valid_utf8(std::string {'\xef', '\xbf', '\xbe'}, 0xfffe);
    verify_valid_utf8(std::string {'\xef', '\xbf', '\xbf'}, 0xffff);

    for (char32_t i = 0x1fffe; i < 0x110000; i += 0x10000) {
        const char c1 = static_cast<char>(0xf0 | (i >> 18));
        const char c2 = static_cast<char>(0x80 | ((i >> 12) & 0x3f));

        verify_valid_utf8(std::string {c1, c2, '\xbf', '\xbe'}, i);
        verify_valid_utf8(std::string {c1, c2, '\xbf', '\xbf'}, i + 1);
    }
}

BOOST_AUTO_TEST_SUITE_END()
