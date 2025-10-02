/**
 * @file
 *
 * Bootstrap `sndfile` types.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:sndfile_core;

import std;

import synthclone.external.sndfile;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::sndfile_codec_shift
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr inline
    std::size_t sndfile_codec_shift = std::countr_zero(
        to_unsigned(std::to_underlying(::SF_FORMAT_SUBMASK)));

    static_assert(sndfile_codec_shift == 0);

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sndfile_codec_count
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr inline
    std::size_t sndfile_codec_count = std::bit_ceil(
        (
            to_unsigned(std::to_underlying(::SF_FORMAT_SUBMASK)) >>
            sndfile_codec_shift
        ));

    static_assert(sndfile_codec_count == 65536);

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sndfile_endianness_shift
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr inline
    std::size_t sndfile_endianness_shift = std::countr_zero(
        to_unsigned(std::to_underlying(::SF_FORMAT_ENDMASK)));

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sndfile_endianness_count
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr inline
    std::size_t sndfile_endianness_count = std::bit_ceil(
        (
            to_unsigned(std::to_underlying(::SF_FORMAT_ENDMASK)) >>
            sndfile_endianness_shift
        ));

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sndfile_format_shift
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr inline
    std::size_t sndfile_format_shift = std::countr_zero(
        to_unsigned(std::to_underlying(::SF_FORMAT_TYPEMASK)));

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sndfile_format_count
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr inline
    std::size_t sndfile_format_count = std::bit_ceil(
        (
            to_unsigned(std::to_underlying(::SF_FORMAT_TYPEMASK)) >>
            sndfile_format_shift
        ));

}
