/**
 * @file
 *
 * Base module for all things audio.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:audio;

import :audio_core;
import :audio_gen;
import :audio_io;

namespace SYNTHCLONE_LIB_NAMESPACE {

    export using synthclone::audio_channel_count;
    export using synthclone::audio_codec;
    export using synthclone::audio_codec_count;
    export using synthclone::audio_copier;
    export using synthclone::audio_duration;
    export using synthclone::audio_endianness;
    export using synthclone::audio_endianness_count;
    export using synthclone::audio_error;
    export using synthclone::audio_format;
    export using synthclone::audio_format_count;
    export using synthclone::audio_frame_count;
    export using synthclone::audio_input_stream;
    export using synthclone::audio_output_stream;
    export using synthclone::audio_resampler;
    export using synthclone::audio_sample;
    export using synthclone::audio_sample_rate;
    export using synthclone::audio_seek_offset;
    export using synthclone::audio_seek_origin;
    export using synthclone::audio_seek_origin_count;
    export using synthclone::audio_traits;

    export using synthclone::get_extension;
    export using synthclone::get_identifier;
    export using synthclone::is_streamable;
    export using synthclone::to_dbfs;

}
