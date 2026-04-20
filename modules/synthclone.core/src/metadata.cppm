/**
 * @file
 *
 * Contains base exported metadata functionality.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:metadata;

import :metadata_core;

namespace SYNTHCLONE_LIB_NAMESPACE {

    export using synthclone::metadata_element;
    export using synthclone::metadata_element_sequence;
    export using synthclone::metadata_error;
    export using synthclone::metadata_text;
    export using synthclone::metadata_url;

}
