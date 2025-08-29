/**
 * @file
 *
 * Maps `synthclone` audio type elements to `libsndfile` type elements, and
 * vice versa.
 *
 * @note
 *   This code was auto-generated at:
 *
 *       2025-08-29 22:11:45.836966+00:00
 *
 *   Do *not* edit this code.  Any changes made to this code will be lost.
 */

module;

#include <cstdio>

#include <sndfile.h>

#include <synthclone/config.h>

export module synthclone.core:sndfile_gen;

import std;

import synthclone.util;

import :audio_gen;
import :sndfile_core;

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_to_sndfile_codec_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr
    lookup_table<int, 14> audio_to_sndfile_codec_table {
        ::SF_FORMAT_PCM_S8,
        ::SF_FORMAT_PCM_U8,
        ::SF_FORMAT_PCM_16,
        ::SF_FORMAT_PCM_24,
        ::SF_FORMAT_PCM_32,
        ::SF_FORMAT_FLOAT,
        ::SF_FORMAT_DOUBLE,
        ::SF_FORMAT_ULAW,
        ::SF_FORMAT_ALAW,
        ::SF_FORMAT_IMA_ADPCM,
        ::SF_FORMAT_MS_ADPCM,
        ::SF_FORMAT_GSM610,
        ::SF_FORMAT_VOX_ADPCM,
        ::SF_FORMAT_VORBIS
    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sndfile_to_audio_codec_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr
    lookup_table<int, sndfile_codec_count>
    sndfile_to_audio_codec_table = make_lookup_table<int, sndfile_codec_count>(
        [](std::size_t n) consteval -> int {
            switch (n << sndfile_codec_shift) {
            case ::SF_FORMAT_PCM_S8:
                return static_cast<int>(audio_codec::pcm_s8);
            case ::SF_FORMAT_PCM_U8:
                return static_cast<int>(audio_codec::pcm_u8);
            case ::SF_FORMAT_PCM_16:
                return static_cast<int>(audio_codec::pcm_s16);
            case ::SF_FORMAT_PCM_24:
                return static_cast<int>(audio_codec::pcm_s24);
            case ::SF_FORMAT_PCM_32:
                return static_cast<int>(audio_codec::pcm_s32);
            case ::SF_FORMAT_FLOAT:
                return static_cast<int>(audio_codec::pcm_f32);
            case ::SF_FORMAT_DOUBLE:
                return static_cast<int>(audio_codec::pcm_f64);
            case ::SF_FORMAT_ULAW:
                return static_cast<int>(audio_codec::ulaw);
            case ::SF_FORMAT_ALAW:
                return static_cast<int>(audio_codec::alaw);
            case ::SF_FORMAT_IMA_ADPCM:
                return static_cast<int>(audio_codec::ima_adpcm);
            case ::SF_FORMAT_MS_ADPCM:
                return static_cast<int>(audio_codec::ms_adpcm);
            case ::SF_FORMAT_GSM610:
                return static_cast<int>(audio_codec::gsm610);
            case ::SF_FORMAT_VOX_ADPCM:
                return static_cast<int>(audio_codec::vox_adpcm);
            case ::SF_FORMAT_VORBIS:
                return static_cast<int>(audio_codec::vorbis);
            default:
                break;
            }
            return -1;
        }
    );

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_to_sndfile_endianness_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr
    lookup_table<int, 3> audio_to_sndfile_endianness_table {
        ::SF_ENDIAN_FILE,
        ::SF_ENDIAN_BIG,
        ::SF_ENDIAN_LITTLE
    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sndfile_to_audio_endianness_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr
    lookup_table<int, sndfile_endianness_count>
    sndfile_to_audio_endianness_table = make_lookup_table<int, sndfile_endianness_count>(
        [](std::size_t n) consteval -> int {
            switch (n << sndfile_endianness_shift) {
            case ::SF_ENDIAN_FILE:
                return static_cast<int>(audio_endianness::file);
            case ::SF_ENDIAN_BIG:
                return static_cast<int>(audio_endianness::big);
            case ::SF_ENDIAN_LITTLE:
                return static_cast<int>(audio_endianness::little);
            default:
                break;
            }
            return -1;
        }
    );

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_to_sndfile_format_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr
    lookup_table<int, 19> audio_to_sndfile_format_table {
        ::SF_FORMAT_AIFF,
        ::SF_FORMAT_AU,
        ::SF_FORMAT_AVR,
        ::SF_FORMAT_CAF,
        ::SF_FORMAT_HTK,
        ::SF_FORMAT_IRCAM,
        ::SF_FORMAT_MAT4,
        ::SF_FORMAT_MAT5,
        ::SF_FORMAT_NIST,
        ::SF_FORMAT_OGG,
        ::SF_FORMAT_PAF,
        ::SF_FORMAT_PVF,
        ::SF_FORMAT_RAW,
        ::SF_FORMAT_SVX,
        ::SF_FORMAT_VOC,
        ::SF_FORMAT_W64,
        ::SF_FORMAT_WAV,
        ::SF_FORMAT_WAVEX,
        ::SF_FORMAT_WVE
    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sndfile_to_audio_format_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr
    lookup_table<int, sndfile_format_count>
    sndfile_to_audio_format_table = make_lookup_table<int, sndfile_format_count>(
        [](std::size_t n) consteval -> int {
            switch (n << sndfile_format_shift) {
            case ::SF_FORMAT_AIFF:
                return static_cast<int>(audio_format::aiff);
            case ::SF_FORMAT_AU:
                return static_cast<int>(audio_format::au);
            case ::SF_FORMAT_AVR:
                return static_cast<int>(audio_format::avr);
            case ::SF_FORMAT_CAF:
                return static_cast<int>(audio_format::caf);
            case ::SF_FORMAT_HTK:
                return static_cast<int>(audio_format::htk);
            case ::SF_FORMAT_IRCAM:
                return static_cast<int>(audio_format::ircam);
            case ::SF_FORMAT_MAT4:
                return static_cast<int>(audio_format::mat4);
            case ::SF_FORMAT_MAT5:
                return static_cast<int>(audio_format::mat5);
            case ::SF_FORMAT_NIST:
                return static_cast<int>(audio_format::nist);
            case ::SF_FORMAT_OGG:
                return static_cast<int>(audio_format::ogg);
            case ::SF_FORMAT_PAF:
                return static_cast<int>(audio_format::paf);
            case ::SF_FORMAT_PVF:
                return static_cast<int>(audio_format::pvf);
            case ::SF_FORMAT_RAW:
                return static_cast<int>(audio_format::raw);
            case ::SF_FORMAT_SVX:
                return static_cast<int>(audio_format::svx);
            case ::SF_FORMAT_VOC:
                return static_cast<int>(audio_format::voc);
            case ::SF_FORMAT_W64:
                return static_cast<int>(audio_format::w64);
            case ::SF_FORMAT_WAV:
                return static_cast<int>(audio_format::wav);
            case ::SF_FORMAT_WAVEX:
                return static_cast<int>(audio_format::wavex);
            case ::SF_FORMAT_WVE:
                return static_cast<int>(audio_format::wve);
            default:
                break;
            }
            return -1;
        }
    );

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_to_sndfile_seek_origin_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr
    lookup_table<int, 3> audio_to_sndfile_seek_origin_table {
        SEEK_CUR,
        SEEK_SET,
        SEEK_END
    };

}
