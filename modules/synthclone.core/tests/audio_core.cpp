#include <boost/test/unit_test.hpp>

import synthclone.core;
import synthclone.core.tests;
import synthclone.test;
import synthclone.util;

namespace {

    template<class T, typename T::scalar_type Min, typename T::scalar_type Max>
    void
    verify_scalar_constructor()
    {
        synthclone::verify_eq(T{Min}, Min);
        synthclone::verify_eq(T{Max}, Max);

        BOOST_CHECK_THROW(T{Max + 1}, synthclone::verification_error);
        BOOST_CHECK_THROW(T{Min - 1}, synthclone::verification_error);
    }

}

BOOST_AUTO_TEST_SUITE(audio_core)

BOOST_AUTO_TEST_CASE(channel_count_constructor)
{
    verify_scalar_constructor<synthclone::audio_channel_count, 1, 128>();
}

BOOST_AUTO_TEST_CASE(duration_constructor)
{
    verify_scalar_constructor<synthclone::audio_duration, 0, 300000000000>();
}

BOOST_AUTO_TEST_CASE(resampler_buffer_overlap_error)
{
    synthclone::audio_resampler resampler(8000, 16000, 1);
    synthclone::fixed_buffer<synthclone::audio_sample, 1> buffer;

    BOOST_CHECK_THROW(
        resampler.resample(buffer, buffer), synthclone::audio_error);
}

BOOST_AUTO_TEST_CASE(resampler_flush_error)
{
    synthclone::audio_resampler resampler(8000, 16000, 1);
    synthclone::fixed_buffer<synthclone::audio_sample, 1> input_buffer;
    synthclone::fixed_buffer<synthclone::audio_sample, 1> output_buffer;

    BOOST_CHECK_EQUAL(0, resampler.flush(output_buffer));
    BOOST_CHECK_THROW(
        resampler.resample(input_buffer, output_buffer),
        synthclone::verification_error);

    resampler.reset();

    BOOST_CHECK_NO_THROW(resampler.resample(input_buffer, output_buffer));
}

BOOST_AUTO_TEST_CASE(sample_rate_constructor)
{
    verify_scalar_constructor<synthclone::audio_sample_rate, 4000, 1024000>();
}

BOOST_AUTO_TEST_CASE(traits)
{
    synthclone::audio_traits traits_1(
        synthclone::audio_format::wav, synthclone::audio_codec::pcm_s16,
        16000U, 2U);

    synthclone::verify_audio_traits(
        traits_1, synthclone::audio_format::wav,
        synthclone::audio_codec::pcm_s16, synthclone::audio_endianness::file,
        16000U, 2U);

    synthclone::audio_traits traits_2(
        synthclone::audio_format::raw, synthclone::audio_codec::pcm_f64,
        synthclone::audio_endianness::big, 44100U, 1U);

    synthclone::verify_audio_traits(
        traits_2, synthclone::audio_format::raw,
        synthclone::audio_codec::pcm_f64, synthclone::audio_endianness::big,
        44100U, 1U);

    synthclone::audio_traits traits_3(traits_1);

    synthclone::verify_audio_traits(
        traits_3, synthclone::audio_format::wav,
        synthclone::audio_codec::pcm_s16, synthclone::audio_endianness::file,
        16000U, 2U);

    traits_1 = traits_2;

    synthclone::verify_audio_traits(
        traits_1, synthclone::audio_format::raw,
        synthclone::audio_codec::pcm_f64, synthclone::audio_endianness::big,
        44100U, 1U);

    synthclone::verify_ne(
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::little, 8000, 1),
        synthclone::audio_traits(
            synthclone::audio_format::wav, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::little, 8000, 1));

    synthclone::verify_ne(
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::little, 8000, 1),
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_s32,
            synthclone::audio_endianness::little, 8000, 1));

    synthclone::verify_ne(
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::little, 8000, 1),
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::big, 8000, 1));

    synthclone::verify_ne(
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::little, 8000, 1),
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::little, 16000, 1));

    synthclone::verify_ne(
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::little, 8000, 1),
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_s16,
            synthclone::audio_endianness::little, 8000, 2));
}

BOOST_AUTO_TEST_SUITE_END()
