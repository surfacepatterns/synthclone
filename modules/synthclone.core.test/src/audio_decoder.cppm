module;

#include <boost/test/unit_test.hpp>

export module synthclone.core.test:audio_decoder;

import std;

import synthclone.core;

import :frequency;
import :reference;

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_encoded_reference_audio()
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
    verify_encoded_reference_audio(const std::filesystem::path& path)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_encoded_reference_audio<"
                "{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}"
                ">({8:?})",
                get_identifier(Format), get_identifier(Codec),
                get_identifier(Endianness), SampleRate, ChannelCount,
                FrameOffset, ExpectEos, PeakThreshold, path.string()));

        const auto encoded_frequencies = load_encoded_reference_frequencies<
            Format,
            Codec,
            Endianness,
            SampleRate,
            ChannelCount,
            FrameOffset,
            ExpectEos
        >(path);

        const auto& reference_frequencies = get_reference_frequencies<
            SampleRate,
            ChannelCount
        >();

        verify_frequencies<PeakThreshold>(
            reference_frequencies, encoded_frequencies,
            std::make_index_sequence<ChannelCount>());
    }

}
