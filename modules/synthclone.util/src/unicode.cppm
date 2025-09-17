/**
 * @file
 *
 * Base module for all things unicode.
 */

module;

#include <synthclone/config.h>

export module synthclone.util:unicode;

import :unicode_core;
import :unicode_gen;

namespace SYNTHCLONE_LIB_NAMESPACE {

    export using synthclone::unicode_category;
    export using synthclone::unicode_category_count;
    export using synthclone::unicode_codepoint;
    export using synthclone::unicode_error;
    export using synthclone::utf8_decode_iterator;
    export using synthclone::utf8_decode_view;
    export using synthclone::utf8_line;
    export using synthclone::utf8_string;

    export using synthclone::decode_utf8;
    export using synthclone::get_category;
    export using synthclone::get_identifier;
    export using synthclone::is_line_terminator;

}
