// This code was auto-generated at:
//
//     2025-09-05 05:59:47.715982+00:00
//
// Do *not* edit this code.  Any changes made to this code will be lost.

#include <boost/test/unit_test.hpp>

import std;

import synthclone.test;
import synthclone.util;

BOOST_AUTO_TEST_SUITE(unicode_gen)

BOOST_AUTO_TEST_CASE(unicode_category_identifiers)
{
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::uppercase_letter), std::string_view("uppercase_letter"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::lowercase_letter), std::string_view("lowercase_letter"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::titlecase_letter), std::string_view("titlecase_letter"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::modifier_letter), std::string_view("modifier_letter"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::other_letter), std::string_view("other_letter"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::nonspacing_mark), std::string_view("nonspacing_mark"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::spacing_mark), std::string_view("spacing_mark"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::enclosing_mark), std::string_view("enclosing_mark"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::decimal_number), std::string_view("decimal_number"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::letter_number), std::string_view("letter_number"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::other_number), std::string_view("other_number"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::connector_punctuation), std::string_view("connector_punctuation"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::dash_punctuation), std::string_view("dash_punctuation"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::open_punctuation), std::string_view("open_punctuation"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::close_punctuation), std::string_view("close_punctuation"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::initial_punctuation), std::string_view("initial_punctuation"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::final_punctuation), std::string_view("final_punctuation"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::other_punctuation), std::string_view("other_punctuation"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::math_symbol), std::string_view("math_symbol"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::currency_symbol), std::string_view("currency_symbol"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::modifier_symbol), std::string_view("modifier_symbol"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::other_symbol), std::string_view("other_symbol"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::space_separator), std::string_view("space_separator"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::line_separator), std::string_view("line_separator"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::paragraph_separator), std::string_view("paragraph_separator"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::control), std::string_view("control"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::format), std::string_view("format"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::surrogate), std::string_view("surrogate"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::private_use), std::string_view("private_use"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::unicode_category::unassigned), std::string_view("unassigned"));
}

BOOST_AUTO_TEST_SUITE_END()
