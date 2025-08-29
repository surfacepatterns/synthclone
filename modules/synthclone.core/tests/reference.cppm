module;

#include <boost/test/unit_test.hpp>

export module synthclone.core.tests:reference;

import std;

import synthclone.core;

import :audio;
import :frequency;

///////////////////////////////////////////////////////////////////////////////
// synthclone::load_reference_audio()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<
        audio_sample_rate::scalar_type SampleRate,
        audio_channel_count::scalar_type ChannelCount
    >
    auto
    load_reference_audio(const std::filesystem::path& path)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::load_reference_audio<{0}, {1}>({2:?})",
                SampleRate, ChannelCount, path.string()));

        return load_audio<
            audio_format::raw,
            audio_codec::pcm_f64,
            audio_endianness::little,
            SampleRate,
            ChannelCount,
            SampleRate
        >(path);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_reference_audio()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<
        audio_sample_rate::scalar_type SampleRate,
        audio_channel_count::scalar_type ChannelCount
    >
    std::filesystem::path
    generate_reference_audio_path()
    {
        return std::filesystem::path(
            std::format(
                "data/audio/reference/{0}Hz-{1}ch-pcm_f64-le.raw", SampleRate,
                ChannelCount));
    }

    template<
        audio_sample_rate::scalar_type SampleRate,
        audio_channel_count::scalar_type ChannelCount
    >
    auto
    load_reference_audio()
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::load_reference_audio<{0}, {1}>()", SampleRate,
                ChannelCount));

        return load_audio<
            audio_format::raw,
            audio_codec::pcm_f64,
            audio_endianness::little,
            SampleRate,
            ChannelCount,
            SampleRate
        >(generate_reference_audio_path<SampleRate, ChannelCount>());
    }

    // There's some cost to loading reference audio, and we're going to use the
    // same bits of reference audio over and over again across tests; so, we
    // cache reference audio once it's loaded.

    export
    template<
        audio_sample_rate::scalar_type SampleRate,
        audio_channel_count::scalar_type ChannelCount
    >
    const auto&
    get_reference_audio()
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::get_reference_audio<{0}, {1}>()", SampleRate,
                ChannelCount));

        static const auto buffer(
            load_reference_audio<SampleRate, ChannelCount>());
        return buffer;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_reference_frequencies()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<
        audio_sample_rate::scalar_type SampleRate,
        audio_channel_count::scalar_type ChannelCount
    >
    auto
    load_reference_frequencies()
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::load_reference_frequencies<{0}, {1}>()",
                SampleRate, ChannelCount));

        return load_frequencies<ChannelCount>(
            get_reference_audio<SampleRate, ChannelCount>());
    }

    // There's some cost to loading refeence frequencies, and we're going to
    // use the sets of reference frequencies over and over again across tests;
    // so, we cache reference frequencies once they're calculated.

    export
    template<
        audio_sample_rate::scalar_type SampleRate,
        audio_channel_count::scalar_type ChannelCount
    >
    const auto&
    get_reference_frequencies()
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::get_reference_frequencies<{0}, {1}>()",
                SampleRate, ChannelCount));

        static const auto frequencies_tuple(
            load_reference_frequencies<SampleRate, ChannelCount>());
        return frequencies_tuple;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::load_encoded_reference_audio()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<
        audio_format Format,
        audio_codec Codec,
        audio_endianness Endianness,
        audio_sample_rate::scalar_type SampleRate,
        audio_channel_count::scalar_type ChannelCount,
        audio_frame_count FrameOffset,
        bool ExpectEos
    >
    audio_sample_buffer<SampleRate * ChannelCount>
    load_encoded_reference_audio(const std::filesystem::path& path)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::load_encoded_reference_audio<"
                "{0}, {1}, {2}, {3}, {4}, {5}, {6}"
                ">({7:?})",
                get_identifier(Format), get_identifier(Codec),
                get_identifier(Endianness), SampleRate, ChannelCount,
                FrameOffset, ExpectEos, path.string()));

        return load_audio<
            Format,
            Codec,
            Endianness,
            SampleRate,
            ChannelCount,
            SampleRate,
            FrameOffset,
            ExpectEos
        >(path);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::load_encoded_reference_frequencies()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<
        audio_format Format,
        audio_codec Codec,
        audio_endianness Endianness,
        audio_sample_rate::scalar_type SampleRate,
        audio_channel_count::scalar_type ChannelCount,
        audio_frame_count FrameOffset,
        bool ExpectEos
    >
    auto
    load_encoded_reference_frequencies(const std::filesystem::path& path)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::load_encoded_reference_frequencies<"
                "{0}, {1}, {2}, {3}, {4}, {5}, {6}"
                ">({7:?})",
                get_identifier(Format), get_identifier(Codec),
                get_identifier(Endianness), SampleRate, ChannelCount,
                FrameOffset, ExpectEos, path.string()));

        return load_frequencies<ChannelCount>(
            load_encoded_reference_audio<
                Format,
                Codec,
                Endianness,
                SampleRate,
                ChannelCount,
                FrameOffset,
                ExpectEos
            >(path));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::make_reference_audio_input_stream()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<
        audio_sample_rate::scalar_type SampleRate,
        audio_channel_count::scalar_type ChannelCount
    >
    audio_input_stream
    make_reference_audio_input_stream()
    {
        constexpr audio_traits traits(
            audio_format::raw, audio_codec::pcm_f64, audio_endianness::little,
            SampleRate, ChannelCount);

        return audio_input_stream(
            generate_reference_audio_path<SampleRate, ChannelCount>(), traits);
    }

}
