module;

#include <boost/test/unit_test.hpp>

export module synthclone.core.test:audio_encoder;

import std;

import synthclone.core;

import :frequency;
import :reference;

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_audio_encoder()
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
        bool ExpectEos,
        double PeakThreshold
    >
    void
    verify_audio_encoder()
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_audio_encoder<"
                "{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}>()",
                get_identifier(Format), get_identifier(Codec),
                get_identifier(Endianness), SampleRate, ChannelCount,
                FrameOffset, ExpectEos, PeakThreshold));

        constexpr audio_traits traits(
            Format, Codec, Endianness, SampleRate, ChannelCount);

        BOOST_REQUIRE(is_streamable(traits));

        const auto& reference_audio =
            get_reference_audio<SampleRate, ChannelCount>();

        temporary_file temp_file;
        const auto& temp_path = temp_file.path();

        {
            audio_output_stream stream(temp_path, traits);
            stream.write(reference_audio);
        }

        const auto encoded_frequencies = load_encoded_reference_frequencies<
            Format,
            Codec,
            Endianness,
            SampleRate,
            ChannelCount,
            FrameOffset,
            ExpectEos
        >(temp_path);

        const auto& reference_frequencies = get_reference_frequencies<
            SampleRate,
            ChannelCount
        >();

        verify_frequencies<PeakThreshold>(
            reference_frequencies, encoded_frequencies,
            std::make_index_sequence<ChannelCount>());
    }

}
