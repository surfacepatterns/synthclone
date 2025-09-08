/**
 * @file
 *
 * Contains generated unicode enums.
 *
 * @note
 *   This code was auto-generated at:
 *
 *       2025-09-05 05:59:47.715747+00:00
 *
 *   Do *not* edit this code.  Any changes made to this code will be lost.
 *
 *   Category enum data is a slightly modified version of the data here:
 *
 *       https://www.unicode.org/reports/tr44/#General_Category_Values
 */

module;

#include <utf8proc.h>

#include <synthclone/config.h>

export module synthclone.util:unicode_gen;

import std;

import :lookup;

///////////////////////////////////////////////////////////////////////////////
// synthclone::unicode_category
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains the unicode categories applied to codepoints.
     */

    export
    enum class unicode_category: std::uint_least8_t {

        /**
         * an uppercase letter
         */

        uppercase_letter = ::UTF8PROC_CATEGORY_LU,

        /**
         * a lowercase letter
         */

        lowercase_letter = ::UTF8PROC_CATEGORY_LL,

        /**
         * a digraph encoded as a single character, with first part uppercase
         */

        titlecase_letter = ::UTF8PROC_CATEGORY_LT,

        /**
         * a modifier letter
         */

        modifier_letter = ::UTF8PROC_CATEGORY_LM,

        /**
         * other letters, including syllables and ideographs
         */

        other_letter = ::UTF8PROC_CATEGORY_LO,

        /**
         * a nonspacing combining mark (zero advance width)
         */

        nonspacing_mark = ::UTF8PROC_CATEGORY_MN,

        /**
         * a spacing combining mark (positive advance width)
         */

        spacing_mark = ::UTF8PROC_CATEGORY_MC,

        /**
         * an enclosing combining mark
         */

        enclosing_mark = ::UTF8PROC_CATEGORY_ME,

        /**
         * a decimal digit
         */

        decimal_number = ::UTF8PROC_CATEGORY_ND,

        /**
         * a letterlike numeric character
         */

        letter_number = ::UTF8PROC_CATEGORY_NL,

        /**
         * a numeric character of other type
         */

        other_number = ::UTF8PROC_CATEGORY_NO,

        /**
         * a connecting punctuation mark, like a tie
         */

        connector_punctuation = ::UTF8PROC_CATEGORY_PC,

        /**
         * a dash or hyphen punctuation mark
         */

        dash_punctuation = ::UTF8PROC_CATEGORY_PD,

        /**
         * an opening punctuation mark (of a pair)
         */

        open_punctuation = ::UTF8PROC_CATEGORY_PS,

        /**
         * a closing punctuation mark (of a pair)
         */

        close_punctuation = ::UTF8PROC_CATEGORY_PE,

        /**
         * an initial quotation mark
         */

        initial_punctuation = ::UTF8PROC_CATEGORY_PI,

        /**
         * a final quotation mark
         */

        final_punctuation = ::UTF8PROC_CATEGORY_PF,

        /**
         * a punctuation mark of other type
         */

        other_punctuation = ::UTF8PROC_CATEGORY_PO,

        /**
         * a symbol of mathematical use
         */

        math_symbol = ::UTF8PROC_CATEGORY_SM,

        /**
         * a currency sign
         */

        currency_symbol = ::UTF8PROC_CATEGORY_SC,

        /**
         * a non-letterlike modifier symbol
         */

        modifier_symbol = ::UTF8PROC_CATEGORY_SK,

        /**
         * a symbol of other type
         */

        other_symbol = ::UTF8PROC_CATEGORY_SO,

        /**
         * a space character (of various non-zero widths)
         */

        space_separator = ::UTF8PROC_CATEGORY_ZS,

        /**
         * U+2028 LINE SEPARATOR only
         */

        line_separator = ::UTF8PROC_CATEGORY_ZL,

        /**
         * U+2029 PARAGRAPH SEPARATOR only
         */

        paragraph_separator = ::UTF8PROC_CATEGORY_ZP,

        /**
         * a C0 or C1 control code
         */

        control = ::UTF8PROC_CATEGORY_CC,

        /**
         * a format control character
         */

        format = ::UTF8PROC_CATEGORY_CF,

        /**
         * a surrogate code point
         */

        surrogate = ::UTF8PROC_CATEGORY_CS,

        /**
         * a private-use character
         */

        private_use = ::UTF8PROC_CATEGORY_CO,

        /**
         * a reserved unassigned code point or a noncharacter
         */

        unassigned = ::UTF8PROC_CATEGORY_CN

    };

    /**
     * The number of `unicode_category` elements
     */

    export
    constexpr
    std::size_t unicode_category_count = 30;

    constexpr
    lookup_table<std::string_view, 30>
    unicode_category_identifier_table = make_lookup_table<std::string_view, 30>(
        [](std::size_t n) consteval -> std::string_view {
            switch (static_cast<unicode_category>(n)) {
            case unicode_category::uppercase_letter:
                return std::string_view("uppercase_letter");
            case unicode_category::lowercase_letter:
                return std::string_view("lowercase_letter");
            case unicode_category::titlecase_letter:
                return std::string_view("titlecase_letter");
            case unicode_category::modifier_letter:
                return std::string_view("modifier_letter");
            case unicode_category::other_letter:
                return std::string_view("other_letter");
            case unicode_category::nonspacing_mark:
                return std::string_view("nonspacing_mark");
            case unicode_category::spacing_mark:
                return std::string_view("spacing_mark");
            case unicode_category::enclosing_mark:
                return std::string_view("enclosing_mark");
            case unicode_category::decimal_number:
                return std::string_view("decimal_number");
            case unicode_category::letter_number:
                return std::string_view("letter_number");
            case unicode_category::other_number:
                return std::string_view("other_number");
            case unicode_category::connector_punctuation:
                return std::string_view("connector_punctuation");
            case unicode_category::dash_punctuation:
                return std::string_view("dash_punctuation");
            case unicode_category::open_punctuation:
                return std::string_view("open_punctuation");
            case unicode_category::close_punctuation:
                return std::string_view("close_punctuation");
            case unicode_category::initial_punctuation:
                return std::string_view("initial_punctuation");
            case unicode_category::final_punctuation:
                return std::string_view("final_punctuation");
            case unicode_category::other_punctuation:
                return std::string_view("other_punctuation");
            case unicode_category::math_symbol:
                return std::string_view("math_symbol");
            case unicode_category::currency_symbol:
                return std::string_view("currency_symbol");
            case unicode_category::modifier_symbol:
                return std::string_view("modifier_symbol");
            case unicode_category::other_symbol:
                return std::string_view("other_symbol");
            case unicode_category::space_separator:
                return std::string_view("space_separator");
            case unicode_category::line_separator:
                return std::string_view("line_separator");
            case unicode_category::paragraph_separator:
                return std::string_view("paragraph_separator");
            case unicode_category::control:
                return std::string_view("control");
            case unicode_category::format:
                return std::string_view("format");
            case unicode_category::surrogate:
                return std::string_view("surrogate");
            case unicode_category::private_use:
                return std::string_view("private_use");
            case unicode_category::unassigned:
                return std::string_view("unassigned");
            default:
                break;
            }
            assume_unreachable("{0}: unexpected index", n);
        }
    );

    /**
     * Gets the identifier for the given `unicode_category` element.
     * 
     * @param element
     *   The `unicode_category` element.
     * 
     * @return
     *   The identifier.
     */

    export
    constexpr
    std::string_view
    get_identifier(unicode_category element)
    {
        return unicode_category_identifier_table.at(
            static_cast<std::size_t>(element));
    }

}
