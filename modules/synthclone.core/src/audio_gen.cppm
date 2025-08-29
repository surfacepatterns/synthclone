/**
 * @file
 *
 * Contains audio type enumerations and supporting functionality.
 *
 * @note
 *   This code was auto-generated at:
 *
 *       2025-08-29 22:06:25.606228+00:00
 *
 *   Do *not* edit this code.  Any changes made to this code will be lost.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:audio_gen;

import std;

import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_format
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains audio formats supported by audio streams
     */

    export
    enum class audio_format: std::uint_least16_t {

        /**
         * Apple/SGI AIFF format
         */

        aiff = 0,

        /**
         * Sun/NeXT AU format
         */

        au = 1,

        /**
         * Audio Visual Research format
         */

        avr = 2,

        /**
         * Core Audio File format
         */

        caf = 3,

        /**
         * HMM Tool Kit format
         */

        htk = 4,

        /**
         * Berkeley/IRCAM/CARL format
         */

        ircam = 5,

        /**
         * Matlab 4.2 / GNU Octave 2.0 format
         */

        mat4 = 6,

        /**
         * Matlab 5.0 / GNU Octave 2.1 format
         */

        mat5 = 7,

        /**
         * Sphere NIST format
         */

        nist = 8,

        /**
         * Xiph OGG container format
         */

        ogg = 9,

        /**
         * Ensoniq PARIS file format
         */

        paf = 10,

        /**
         * Portable Voice Format
         */

        pvf = 11,

        /**
         * Raw data
         */

        raw = 12,

        /**
         * Amiga IFF / SVX8 / SV16 format
         */

        svx = 13,

        /**
         * VOC file format
         */

        voc = 14,

        /**
         * Sonic Foundry’s 64 bit RIFF/WAV format
         */

        w64 = 15,

        /**
         * Microsoft WAV format
         */

        wav = 16,

        /**
         * MS WAVE with WAVEFORMATEX
         */

        wavex = 17,

        /**
         * Psion WVE format
         */

        wve = 18

    };

    /**
     * The number of `audio_format` elements
     */

    export
    constexpr
    std::size_t audio_format_count = 19;

    constexpr
    lookup_table<std::string_view, 19> audio_format_identifier_table {
        std::string_view("aiff"),
        std::string_view("au"),
        std::string_view("avr"),
        std::string_view("caf"),
        std::string_view("htk"),
        std::string_view("ircam"),
        std::string_view("mat4"),
        std::string_view("mat5"),
        std::string_view("nist"),
        std::string_view("ogg"),
        std::string_view("paf"),
        std::string_view("pvf"),
        std::string_view("raw"),
        std::string_view("svx"),
        std::string_view("voc"),
        std::string_view("w64"),
        std::string_view("wav"),
        std::string_view("wavex"),
        std::string_view("wve")
    };

    /**
     * Gets the identifier for the given `audio_format` element.
     * 
     * @param element
     *   The `audio_format` element.
     * 
     * @return
     *   The identifier.
     */

    export
    constexpr
    std::string_view
    get_identifier(audio_format element)
    {
        return audio_format_identifier_table.at(
            static_cast<std::size_t>(element));
    }

    export
    constexpr
    lookup_table<std::string_view, 19> audio_format_extension_table {
        std::string_view(".aiff"),
        std::string_view(".au"),
        std::string_view(".avr"),
        std::string_view(".caf"),
        std::string_view(".htk"),
        std::string_view(".sf"),
        std::string_view(".mat"),
        std::string_view(".mat"),
        std::string_view(".wav"),
        std::string_view(".ogg"),
        std::string_view(".paf"),
        std::string_view(".pvf"),
        std::string_view(".raw"),
        std::string_view(".iff"),
        std::string_view(".voc"),
        std::string_view(".w64"),
        std::string_view(".wav"),
        std::string_view(".wav"),
        std::string_view(".wve")
    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_codec
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains audio codecs supported by audio streams
     */

    export
    enum class audio_codec: std::uint_least16_t {

        /**
         * Signed 8-bit integer PCM
         */

        pcm_s8 = 0,

        /**
         * Unsigned 8-bit integer PCM
         */

        pcm_u8 = 1,

        /**
         * Signed 16-bit integer PCM
         */

        pcm_s16 = 2,

        /**
         * Signed 24-bit integer PCM
         */

        pcm_s24 = 3,

        /**
         * Signed 32-bit integer PCM
         */

        pcm_s32 = 4,

        /**
         * 32-bit floating point PCM
         */

        pcm_f32 = 5,

        /**
         * 64-bit floating point PCM
         */

        pcm_f64 = 6,

        /**
         * U-law encoded data
         */

        ulaw = 7,

        /**
         * A-law encoded data
         */

        alaw = 8,

        /**
         * IMA ADPCM encoded data
         */

        ima_adpcm = 9,

        /**
         * Microsoft ADPCM encoded data
         */

        ms_adpcm = 10,

        /**
         * GSM 6.10 encoded data
         */

        gsm610 = 11,

        /**
         * Oki Dialogic ADPCM encoded data
         */

        vox_adpcm = 12,

        /**
         * Xiph Vorbis encoded data
         */

        vorbis = 13

    };

    /**
     * The number of `audio_codec` elements
     */

    export
    constexpr
    std::size_t audio_codec_count = 14;

    constexpr
    lookup_table<std::string_view, 14> audio_codec_identifier_table {
        std::string_view("pcm_s8"),
        std::string_view("pcm_u8"),
        std::string_view("pcm_s16"),
        std::string_view("pcm_s24"),
        std::string_view("pcm_s32"),
        std::string_view("pcm_f32"),
        std::string_view("pcm_f64"),
        std::string_view("ulaw"),
        std::string_view("alaw"),
        std::string_view("ima_adpcm"),
        std::string_view("ms_adpcm"),
        std::string_view("gsm610"),
        std::string_view("vox_adpcm"),
        std::string_view("vorbis")
    };

    /**
     * Gets the identifier for the given `audio_codec` element.
     * 
     * @param element
     *   The `audio_codec` element.
     * 
     * @return
     *   The identifier.
     */

    export
    constexpr
    std::string_view
    get_identifier(audio_codec element)
    {
        return audio_codec_identifier_table.at(
            static_cast<std::size_t>(element));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_endianness
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains audio endianness types supported by audio streams
     */

    export
    enum class audio_endianness: std::uint_least8_t {

        /**
         * Default file-endianness.
         */

        file = 0,

        /**
         * Big endian.
         */

        big = 1,

        /**
         * Little endian.
         */

        little = 2

    };

    /**
     * The number of `audio_endianness` elements
     */

    export
    constexpr
    std::size_t audio_endianness_count = 3;

    constexpr
    lookup_table<std::string_view, 3> audio_endianness_identifier_table {
        std::string_view("file"),
        std::string_view("big"),
        std::string_view("little")
    };

    /**
     * Gets the identifier for the given `audio_endianness` element.
     * 
     * @param element
     *   The `audio_endianness` element.
     * 
     * @return
     *   The identifier.
     */

    export
    constexpr
    std::string_view
    get_identifier(audio_endianness element)
    {
        return audio_endianness_identifier_table.at(
            static_cast<std::size_t>(element));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_seek_origin
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Represents the seek origin for audio seek operations.
     */

    export
    enum class audio_seek_origin: std::uint_least8_t {

        /**
         * Seek from the current frame in the audio stream.
         */

        current = 0,

        /**
         * Seek from the first frame in the audio stream.
         */

        start = 1,

        /**
         * Seek from just after the last frame in the audio stream.
         */

        end = 2

    };

    /**
     * The number of `audio_seek_origin` elements
     */

    export
    constexpr
    std::size_t audio_seek_origin_count = 3;

    constexpr
    lookup_table<std::string_view, 3> audio_seek_origin_identifier_table {
        std::string_view("current"),
        std::string_view("start"),
        std::string_view("end")
    };

    /**
     * Gets the identifier for the given `audio_seek_origin` element.
     * 
     * @param element
     *   The `audio_seek_origin` element.
     * 
     * @return
     *   The identifier.
     */

    export
    constexpr
    std::string_view
    get_identifier(audio_seek_origin element)
    {
        return audio_seek_origin_identifier_table.at(
            static_cast<std::size_t>(element));
    }

}
