#include <filesystem>

#include <boost/interprocess/mapped_region.hpp>

#include <boost/test/unit_test.hpp>

import synthclone.core;
import synthclone.core.tests;
import synthclone.test;
import synthclone.util;

// XXX: How do we write reliable audio stream tests for `read()` and `write()`
// errors?

namespace {

    std::pair<synthclone::audio_frame_count, synthclone::audio_frame_count>
    copy(
        synthclone::audio_copier& copier,
        synthclone::audio_input_stream& input_stream,
        synthclone::audio_output_stream& output_stream,
        synthclone::audio_frame_count max_frames =
            std::numeric_limits<synthclone::audio_frame_count>::max()
    )
    {
        synthclone::audio_frame_count input_frames_processed = 0;
        synthclone::audio_frame_count output_frames_processed = 0;

        for (
            auto [frames_read, frames_written]:
            copier.copy(input_stream, output_stream, max_frames)
        ) {
            input_frames_processed += frames_read;
            output_frames_processed += frames_written;
        }

        return { input_frames_processed, output_frames_processed };
    }

    template<
        synthclone::audio_sample_rate::scalar_type InputSampleRate,
        synthclone::audio_sample_rate::scalar_type OutputSampleRate,
        double PeakThreshold
    >
    void
    verify_resampled_copy(synthclone::audio_copier& copier)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_resampled_copy<{0}, {1}, {2}>("
                "synthclone::audio_copier&)",
                InputSampleRate, OutputSampleRate, PeakThreshold));

        constexpr synthclone::audio_traits traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_f64,
            synthclone::audio_endianness::little, OutputSampleRate, 1);

        synthclone::temporary_file output_file;
        const auto& output_path = output_file.path();

        {
            auto input_stream = synthclone::make_reference_audio_input_stream<
                InputSampleRate,
                1
            >();
            synthclone::audio_output_stream output_stream(output_path, traits);

            auto [frames_read, frames_written] = copy(
                copier, input_stream, output_stream);

            BOOST_CHECK_EQUAL(InputSampleRate, frames_read);
            BOOST_CHECK_EQUAL(OutputSampleRate, frames_written);
        }

        const auto& reference_frequencies =
            synthclone::get_reference_frequencies<OutputSampleRate, 1>();

        const auto copied_audio =
            synthclone::load_reference_audio<OutputSampleRate, 1>(output_path);
        const auto copied_frequencies = synthclone::load_frequencies<1>(
            copied_audio);

        verify_frequencies<PeakThreshold>(
            reference_frequencies, copied_frequencies);
    }

    template<
        synthclone::audio_sample_rate::scalar_type InputSampleRate,
        synthclone::audio_sample_rate::scalar_type OutputSampleRate,
        double PeakThreshold,
        std::size_t... Indices
    >
    void
    verify_resampled_copies(std::index_sequence<Indices...>)
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_resampled_copies<{0}, {1}, {2}, ...>()",
                InputSampleRate, OutputSampleRate, PeakThreshold));

        synthclone::audio_copier copier;

        (
            (
                static_cast<void>(Indices),
                verify_resampled_copy<
                    InputSampleRate,
                    OutputSampleRate,
                    PeakThreshold
                >(copier)
            ),
            ...
        );
    }

    template<
        std::size_t N,
        synthclone::audio_sample_rate::scalar_type InputSampleRate,
        synthclone::audio_sample_rate::scalar_type OutputSampleRate,
        double PeakThreshold
    >
    void
    verify_resampled_copies()
    {
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::verify_resampled_copies<{0}, {1}, {2}, {3}>()",
                N, InputSampleRate, OutputSampleRate, PeakThreshold));

        verify_resampled_copies<
            InputSampleRate,
            OutputSampleRate,
            PeakThreshold
        >(std::make_index_sequence<N>());
    }

}

BOOST_AUTO_TEST_SUITE(audio_io)

BOOST_AUTO_TEST_CASE(copier_downsampled_copies)
{
    verify_resampled_copies<10, 48000, 8000, 9.526827348100687e-06>();
}

BOOST_AUTO_TEST_CASE(copier_empty_file)
{
    synthclone::temporary_file input_file;
    const auto& input_path = input_file.path();

    synthclone::temporary_file output_file;
    const auto& output_path = output_file.path();

    {
        synthclone::audio_input_stream input_stream(
            input_path,
            synthclone::audio_traits(
                synthclone::audio_format::raw,
                synthclone::audio_codec::pcm_f32,
                synthclone::audio_endianness::little, 48000, 1));
        synthclone::audio_output_stream output_stream(
            output_path,
            synthclone::audio_traits(
                synthclone::audio_format::raw,
                synthclone::audio_codec::pcm_f32,
                synthclone::audio_endianness::little, 44100, 1));

        synthclone::audio_copier copier;
        auto [frames_read, frames_written] = copy(
            copier, input_stream, output_stream);

        BOOST_CHECK_EQUAL(0, frames_read);
        BOOST_CHECK_EQUAL(0, frames_written);
    }

    BOOST_CHECK_EQUAL(0, std::filesystem::file_size(output_path));
}

BOOST_AUTO_TEST_CASE(copier_errors)
{
    synthclone::temporary_file input_file;
    const auto& input_path = input_file.path();

    synthclone::temporary_file output_file;
    const auto& output_path = output_file.path();

    synthclone::audio_input_stream input_stream(
        input_path,
        synthclone::audio_traits(
            synthclone::audio_format::raw,
            synthclone::audio_codec::pcm_f32,
            synthclone::audio_endianness::little, 48000, 1));
    synthclone::audio_output_stream output_stream(
        output_path,
        synthclone::audio_traits(
            synthclone::audio_format::raw,
            synthclone::audio_codec::pcm_f32,
            synthclone::audio_endianness::little, 48000, 2));

    synthclone::audio_copier copier;
    auto generator = copier.copy(input_stream, output_stream);

    BOOST_CHECK_THROW(generator.begin(), synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(copier_resampled_truncated_copying)
{
    synthclone::temporary_file output_file;
    const auto& output_path = output_file.path();

    {
        auto input_stream =
            synthclone::make_reference_audio_input_stream<8000, 1>();
        synthclone::audio_output_stream output_stream(
            output_path,
            synthclone::audio_traits(
                synthclone::audio_format::raw,
                synthclone::audio_codec::pcm_f64,
                synthclone::audio_endianness::little, 16000, 1));

        synthclone::audio_copier copier;
        auto [frames_read, frames_written] = copy(
            copier, input_stream, output_stream, 4000);

        BOOST_CHECK_EQUAL(4000, frames_read);
        BOOST_CHECK_EQUAL(8000, frames_written);
    }

    {
        auto input_stream =
            synthclone::make_reference_audio_input_stream<8000, 1>();
        synthclone::audio_output_stream output_stream(
            output_path,
            synthclone::audio_traits(
                synthclone::audio_format::raw,
                synthclone::audio_codec::pcm_f64,
                synthclone::audio_endianness::little, 16000, 1));

        auto input_buffer_size =
            boost::interprocess::mapped_region::get_page_size() /
            sizeof(synthclone::audio_sample);

        synthclone::audio_copier copier;
        auto [frames_read, frames_written] = copy(
            copier, input_stream, output_stream, input_buffer_size);

        BOOST_CHECK_EQUAL(input_buffer_size, frames_read);
        BOOST_CHECK_EQUAL(input_buffer_size * 2, frames_written);
    }

}

BOOST_AUTO_TEST_CASE(copier_simple_copy)
{
    synthclone::temporary_file output_file;
    const auto& output_path = output_file.path();

    {
        auto input_stream =
            synthclone::make_reference_audio_input_stream<8000, 1>();
        synthclone::audio_output_stream output_stream(
            output_path, input_stream.traits());

        synthclone::audio_copier copier;
        auto [frames_read, frames_written] = copy(
            copier, input_stream, output_stream);

        BOOST_CHECK_EQUAL(8000, frames_read);
        BOOST_CHECK_EQUAL(8000, frames_written);
    }

    const auto& reference_audio = synthclone::get_reference_audio<8000, 1>();
    auto copied_audio = synthclone::load_reference_audio<8000, 1>(output_path);

    BOOST_CHECK(std::ranges::equal(reference_audio, copied_audio));
}

BOOST_AUTO_TEST_CASE(copier_simple_truncated_copy)
{
    synthclone::temporary_file output_file;
    const auto& output_path = output_file.path();

    {
        auto input_stream =
            synthclone::make_reference_audio_input_stream<8000, 1>();
        synthclone::audio_output_stream output_stream(
            output_path, input_stream.traits());

        synthclone::audio_copier copier;
        auto [frames_read, frames_written] = copy(
            copier, input_stream, output_stream, 4000);

        BOOST_CHECK_EQUAL(4000, frames_read);
        BOOST_CHECK_EQUAL(4000, frames_written);
    }

    const auto& reference_audio = synthclone::get_reference_audio<8000, 1>();
    auto copied_audio = synthclone::load_reference_audio<4000, 1>(output_path);

    BOOST_CHECK(
        std::ranges::equal(
            std::span(reference_audio.cbegin(), 4000), copied_audio));
}

BOOST_AUTO_TEST_CASE(copier_upsampled_copies)
{
    verify_resampled_copies<10, 8000, 48000, 2.403147090675681e-05>();
}

BOOST_AUTO_TEST_CASE(input_stream_content_type_errors)
{
    synthclone::temporary_file output_file;
    const auto& output_path = output_file.path();

    {
        const auto& audio = synthclone::get_reference_audio<8000, 1>();
        synthclone::audio_output_stream output_stream(
            output_path,
            synthclone::audio_traits(
                synthclone::audio_format::wav,
                synthclone::audio_codec::pcm_s16,
                synthclone::audio_endianness::big, 8000, 1));
        output_stream.write(audio);
    }

    {
        const synthclone::audio_traits traits(
            synthclone::audio_format::ogg, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::big, 8000, 1);
        BOOST_CHECK_THROW(
            synthclone::audio_input_stream input_stream(output_path, traits),
            synthclone::audio_error);
    }

    {
        const synthclone::audio_traits traits(
            synthclone::audio_format::wav, synthclone::audio_codec::pcm_s32,
            synthclone::audio_endianness::big, 8000, 1);
        BOOST_CHECK_THROW(
            synthclone::audio_input_stream input_stream(output_path, traits),
            synthclone::audio_error);
    }

    {
        const synthclone::audio_traits traits(
            synthclone::audio_format::wav, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::little, 8000, 1);
        BOOST_CHECK_THROW(
            synthclone::audio_input_stream input_stream(output_path, traits),
            synthclone::audio_error);
    }

    {
        const synthclone::audio_traits traits(
            synthclone::audio_format::wav, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::big, 16000, 1);
        BOOST_CHECK_THROW(
            synthclone::audio_input_stream input_stream(output_path, traits),
            synthclone::audio_error);
    }

    {
        const synthclone::audio_traits traits(
            synthclone::audio_format::wav, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::big, 8000, 2);
        BOOST_CHECK_THROW(
            synthclone::audio_input_stream input_stream(output_path, traits),
            synthclone::audio_error);
    }
}

BOOST_AUTO_TEST_CASE(input_stream_content_type_inference)
{
    synthclone::temporary_file output_file;
    const auto& output_path = output_file.path();

    const synthclone::audio_traits traits(
        synthclone::audio_format::wav, synthclone::audio_codec::pcm_s16,
        synthclone::audio_endianness::big, 8000, 1);

    {
        const auto& audio = synthclone::get_reference_audio<8000, 1>();
        synthclone::audio_output_stream output_stream(output_path, traits);
        output_stream.write(audio);
    }

    synthclone::audio_input_stream input_stream(output_path);
    synthclone::verify_eq(traits, input_stream.traits());
}

BOOST_AUTO_TEST_CASE(input_stream_content_type_inference_error)
{
    synthclone::temporary_file output_file;
    const auto& output_path = output_file.path();

    const synthclone::audio_traits traits(
        synthclone::audio_format::raw, synthclone::audio_codec::pcm_s16,
        synthclone::audio_endianness::little, 8000, 1);

    {
        const auto& audio = synthclone::get_reference_audio<8000, 1>();
        synthclone::audio_output_stream output_stream(output_path, traits);
        output_stream.write(audio);
    }

    BOOST_CHECK_THROW(
        synthclone::audio_input_stream input_stream(output_path),
        synthclone::audio_error);
}

BOOST_AUTO_TEST_CASE(input_stream_move_ops)
{
    auto input_stream =
        synthclone::make_reference_audio_input_stream<8000, 1>();
    const synthclone::audio_traits traits(input_stream.traits());

    synthclone::audio_input_stream input_stream_2(std::move(input_stream));

    synthclone::verify_eq(traits, input_stream_2.traits());

    input_stream = std::move(input_stream_2);

    synthclone::verify_eq(traits, input_stream.traits());
}

BOOST_AUTO_TEST_CASE(input_stream_not_seekable)
{
    synthclone::temporary_file output_file;
    const auto& output_path = output_file.path();

    synthclone::audio_traits traits(
        synthclone::audio_format::raw, synthclone::audio_codec::vox_adpcm,
        synthclone::audio_endianness::little, 8000, 1);

    {
        const auto& audio = synthclone::get_reference_audio<8000, 1>();
        synthclone::audio_output_stream output_stream(output_path, traits);
        output_stream.write(audio);
    }

    synthclone::audio_input_stream input_stream(output_path, traits);

    BOOST_CHECK(! input_stream.seekable());
    BOOST_CHECK_THROW(
        input_stream.seek(1, synthclone::audio_seek_origin::current),
        synthclone::audio_error);
}

BOOST_AUTO_TEST_CASE(input_stream_ops_after_close)
{
    synthclone::temporary_file file;

    synthclone::audio_input_stream input_stream(
        file.path(),
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_f64,
            48000U, 1U));
    input_stream.close();

    BOOST_CHECK(input_stream.closed());
    BOOST_CHECK_THROW(input_stream.close(), synthclone::verification_error);
    BOOST_CHECK_THROW(
        input_stream.seek(0, synthclone::audio_seek_origin::current),
        synthclone::verification_error);

    std::array<synthclone::audio_sample, 1> sample;

    BOOST_CHECK_THROW(
        input_stream.read(sample), synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(input_stream_seek_and_tell)
{
    auto input_stream =
        synthclone::make_reference_audio_input_stream<8000, 1>();

    BOOST_CHECK_EQUAL(0, input_stream.tell());

    BOOST_CHECK_EQUAL(
        4000, input_stream.seek(4000, synthclone::audio_seek_origin::current));
    BOOST_CHECK_EQUAL(4000, input_stream.tell());

    BOOST_CHECK_EQUAL(
        2000,
        input_stream.seek(-2000, synthclone::audio_seek_origin::current));
    BOOST_CHECK_EQUAL(2000, input_stream.tell());

    BOOST_CHECK_EQUAL(
        3000, input_stream.seek(3000, synthclone::audio_seek_origin::start));
    BOOST_CHECK_EQUAL(3000, input_stream.tell());

    BOOST_CHECK_EQUAL(
        8000, input_stream.seek(0, synthclone::audio_seek_origin::end));
    BOOST_CHECK_EQUAL(8000, input_stream.tell());
}

BOOST_AUTO_TEST_CASE(input_stream_unbalanced_read)
{
    synthclone::temporary_file file;

    synthclone::audio_input_stream input_stream(
        file.path(),
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_f64,
            48000U, 2U));

    std::array<synthclone::audio_sample, 3> samples;

    BOOST_CHECK_THROW(
        input_stream.read(samples), synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(output_stream_not_seekable)
{
    synthclone::temporary_file output_file;
    const auto& output_path = output_file.path();

    synthclone::audio_output_stream output_stream(
        output_path,
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::vox_adpcm,
            synthclone::audio_endianness::little, 8000, 1));

    BOOST_CHECK(! output_stream.seekable());
    BOOST_CHECK_THROW(
        output_stream.seek(1, synthclone::audio_seek_origin::current),
        synthclone::audio_error);
}

BOOST_AUTO_TEST_CASE(output_stream_ops_after_close)
{
    synthclone::temporary_file file;
    synthclone::audio_traits traits(
        synthclone::audio_format::raw, synthclone::audio_codec::pcm_f64,
        48000U, 1U);

    synthclone::audio_output_stream output_stream(file.path(), traits);
    output_stream.close();

    BOOST_CHECK(output_stream.closed());
    BOOST_CHECK_THROW(output_stream.close(), synthclone::verification_error);
    BOOST_CHECK_THROW(
        output_stream.seek(0, synthclone::audio_seek_origin::current),
        synthclone::verification_error);

    std::array<synthclone::audio_sample, 1> sample;
    sample[0] = 0.0;

    BOOST_CHECK_THROW(
        output_stream.write(sample), synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(output_stream_seek_and_tell)
{
    synthclone::temporary_file output_file;
    const auto& output_path = output_file.path();

    const auto& audio = synthclone::get_reference_audio<8000, 1>();
    synthclone::audio_output_stream output_stream(
        output_path,
        synthclone::audio_traits(
            synthclone::audio_format::wav, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::big, 8000, 1));
    output_stream.write(audio);

    BOOST_CHECK_EQUAL(8000, output_stream.tell());

    BOOST_CHECK_EQUAL(
        0, output_stream.seek(0, synthclone::audio_seek_origin::start));
    BOOST_CHECK_EQUAL(0, output_stream.tell());

    BOOST_CHECK_EQUAL(
        4000,
        output_stream.seek(4000, synthclone::audio_seek_origin::current));
    BOOST_CHECK_EQUAL(4000, output_stream.tell());

    BOOST_CHECK_EQUAL(
        2000,
        output_stream.seek(-2000, synthclone::audio_seek_origin::current));
    BOOST_CHECK_EQUAL(2000, output_stream.tell());

    BOOST_CHECK_EQUAL(
        3000, output_stream.seek(3000, synthclone::audio_seek_origin::start));
    BOOST_CHECK_EQUAL(3000, output_stream.tell());

    BOOST_CHECK_EQUAL(
        8000, output_stream.seek(0, synthclone::audio_seek_origin::end));
    BOOST_CHECK_EQUAL(8000, output_stream.tell());
}

BOOST_AUTO_TEST_CASE(output_stream_unbalanced_write)
{
    synthclone::temporary_file file;

    synthclone::audio_output_stream output_stream(
        file.path(),
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_f64,
            48000U, 2U));

    std::array<synthclone::audio_sample, 3> samples;
    samples[0] = 0.0;
    samples[1] = 1.0;
    samples[0] = 0.0;

    BOOST_CHECK_THROW(
        output_stream.write(samples), synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(streams_simple)
{
    synthclone::temporary_file file;
    synthclone::audio_traits traits(
        synthclone::audio_format::raw, synthclone::audio_codec::pcm_f64,
        synthclone::audio_endianness::little, 48000U, 1U);

    std::array<synthclone::audio_sample, 1000> samples;
    for (int i = 0; i < 1000; ++i) {
        samples[i] = static_cast<double>(i) / 1000.0;
    }

    synthclone::audio_output_stream output_stream(file.path(), traits);

    synthclone::verify_eq(output_stream.traits(), traits);

    output_stream.write(samples);
    output_stream.close();

    BOOST_CHECK(output_stream.closed());

    synthclone::audio_input_stream input_stream(
        file.path(),
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_f64,
            synthclone::audio_endianness::little, 48000U, 1U));

    synthclone::verify_eq(input_stream.traits(), traits);

    std::array<synthclone::audio_sample, 1000> actual_samples;

    synthclone::verify_eq(input_stream.read(actual_samples), 1000);

    // XXX: Perhaps we should create a `synthclone.test` wrapper for this
    // macro.
    BOOST_CHECK_EQUAL_COLLECTIONS(
        samples.begin(), samples.end(), actual_samples.begin(),
        actual_samples.end());

    synthclone::verify_eq(input_stream.read(actual_samples), 0);

    input_stream.close();

    BOOST_CHECK(input_stream.closed());
}

BOOST_AUTO_TEST_SUITE_END()
