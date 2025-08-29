module;

#include <boost/test/unit_test.hpp>

export module synthclone.core.tests:audio_resampler;

import std;

import synthclone.core;

import :audio;
import :frequency;
import :reference;

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_resampled_audio()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<
        audio_sample_rate::scalar_type InputSampleRate,
        audio_sample_rate::scalar_type OutputSampleRate,
        audio_channel_count::scalar_type ChannelCount,
        std::size_t InputBufferSize,
        std::size_t OutputBufferSize,
        double PeakThreshold
    >
    void
    verify_resampled_audio()
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_resampled_audio<"
                "{0}, {1}, {2}, {3}, {4}, {5}>()",
                InputSampleRate, OutputSampleRate, ChannelCount,
                InputBufferSize, OutputBufferSize, PeakThreshold));

        constexpr std::size_t ideal_output_sample_count =
            OutputSampleRate * ChannelCount;
        constexpr std::size_t maximum_output_sample_count =
            ideal_output_sample_count + ChannelCount;
        constexpr std::size_t minimum_output_sample_count =
            ideal_output_sample_count - ChannelCount;

        audio_resampler resampler(
            InputSampleRate, OutputSampleRate, ChannelCount);

        const auto& input_audio =
            get_reference_audio<InputSampleRate, ChannelCount>();
        fixed_buffer<audio_sample, maximum_output_sample_count> output_audio;

        const auto* input_data = input_audio.data();
        const auto input_size = input_audio.size();

        auto* output_data = output_audio.data();
        const auto output_size = output_audio.size();

        std::size_t output_index = 0;

        // Process resampler I/O.
        for (std::size_t input_index = 0; input_index < input_size;) {

            BOOST_REQUIRE_LT(output_index, output_size);

            std::span<const audio_sample> input_span(
                input_data + input_index,
                std::min(
                    InputBufferSize * ChannelCount, input_size - input_index));
            std::span<audio_sample> output_span(
                output_data + output_index,
                std::min(
                    OutputBufferSize * ChannelCount,
                    output_size - output_index));

            const auto [frames_used, frames_generated] = resampler.resample(
                input_span, output_span);

            BOOST_REQUIRE_MESSAGE(
                (frames_used != 0) || (frames_generated != 0),
                "resampler did not make progress");

            const auto samples_generated = frames_generated * ChannelCount;
            const auto samples_used = frames_used * ChannelCount;

            BOOST_REQUIRE_LE(samples_used, input_span.size());
            BOOST_REQUIRE_LE(samples_generated, output_span.size());

            input_index += samples_used;
            output_index += samples_generated;
        }

        // Flush the resampler.
        while (output_index < output_size) {

            std::span<audio_sample> output_span(
                output_data + output_index,
                std::min(
                    OutputBufferSize * ChannelCount,
                    output_size - output_index));

            const auto frames_generated = resampler.flush(output_span);
            if (frames_generated == 0) {
                goto found_eoi;
            }

            {
                const auto samples_generated = frames_generated * ChannelCount;
                BOOST_REQUIRE_LE(samples_generated, output_span.size());
                output_index += samples_generated;
            }
        }

        // Make sure the resampler has no more input to process.
        {
            std::array<audio_sample, ChannelCount> tmp;
            const auto frames_generated = resampler.flush(tmp);
            BOOST_CHECK_EQUAL(0, frames_generated);
        }

    found_eoi:
        switch (output_index) {
        case minimum_output_sample_count:
            std::fill(
                output_data + output_index,
                output_data + ideal_output_sample_count, 0.0);
            [[fallthrough]];
        case maximum_output_sample_count:
        case ideal_output_sample_count:
            break;
        default:
            BOOST_FAIL(
                std::format(
                    "resampled output index ({0}) is not between {1} and {2}",
                    output_index, minimum_output_sample_count,
                    maximum_output_sample_count));
        }

        const auto output_frequencies = load_frequencies<ChannelCount>(
            std::span<const audio_sample, ideal_output_sample_count>(
                output_data, ideal_output_sample_count));

        const auto& reference_frequencies = get_reference_frequencies<
            OutputSampleRate,
            ChannelCount
        >();

        verify_frequencies<PeakThreshold>(
            reference_frequencies, output_frequencies,
            std::make_index_sequence<ChannelCount>());
    }

}
