/**
 * @file
 *
 * Contains exported base component functionality.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:component;

import :component_core;

namespace SYNTHCLONE_LIB_NAMESPACE {

    export using synthclone::component_metadata;
    export using synthclone::component_metadata_init_args;

}
