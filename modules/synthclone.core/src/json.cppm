/**
 * @file
 *
 * Contains base exported JSON functionality.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:json;

import :json_core;

namespace SYNTHCLONE_LIB_NAMESPACE {

    export using synthclone::json_traversal_error;

}
