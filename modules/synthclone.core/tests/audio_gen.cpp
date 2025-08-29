// This code was auto-generated at:
//
//     2025-08-29 22:06:25.606572+00:00
//
// Do *not* edit this code.  Any changes made to this code will be lost.

#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;

BOOST_AUTO_TEST_SUITE(audio_gen)

BOOST_AUTO_TEST_CASE(audio_codec_identifiers)
{
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::pcm_s8), std::string_view("pcm_s8"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::pcm_u8), std::string_view("pcm_u8"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::pcm_s16), std::string_view("pcm_s16"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::pcm_s24), std::string_view("pcm_s24"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::pcm_s32), std::string_view("pcm_s32"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::pcm_f32), std::string_view("pcm_f32"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::pcm_f64), std::string_view("pcm_f64"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::ulaw), std::string_view("ulaw"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::alaw), std::string_view("alaw"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::ima_adpcm), std::string_view("ima_adpcm"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::ms_adpcm), std::string_view("ms_adpcm"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::gsm610), std::string_view("gsm610"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::vox_adpcm), std::string_view("vox_adpcm"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_codec::vorbis), std::string_view("vorbis"));
}

BOOST_AUTO_TEST_CASE(audio_endianness_identifiers)
{
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_endianness::file), std::string_view("file"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_endianness::big), std::string_view("big"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_endianness::little), std::string_view("little"));
}

BOOST_AUTO_TEST_CASE(audio_format_extensions)
{
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::aiff), std::string_view(".aiff"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::au), std::string_view(".au"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::avr), std::string_view(".avr"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::caf), std::string_view(".caf"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::htk), std::string_view(".htk"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::ircam), std::string_view(".sf"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::mat4), std::string_view(".mat"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::mat5), std::string_view(".mat"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::nist), std::string_view(".wav"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::ogg), std::string_view(".ogg"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::paf), std::string_view(".paf"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::pvf), std::string_view(".pvf"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::raw), std::string_view(".raw"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::svx), std::string_view(".iff"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::voc), std::string_view(".voc"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::w64), std::string_view(".w64"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::wav), std::string_view(".wav"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::wavex), std::string_view(".wav"));
    synthclone::verify_eq(synthclone::get_extension(synthclone::audio_format::wve), std::string_view(".wve"));
}

BOOST_AUTO_TEST_CASE(audio_format_identifiers)
{
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::aiff), std::string_view("aiff"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::au), std::string_view("au"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::avr), std::string_view("avr"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::caf), std::string_view("caf"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::htk), std::string_view("htk"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::ircam), std::string_view("ircam"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::mat4), std::string_view("mat4"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::mat5), std::string_view("mat5"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::nist), std::string_view("nist"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::ogg), std::string_view("ogg"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::paf), std::string_view("paf"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::pvf), std::string_view("pvf"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::raw), std::string_view("raw"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::svx), std::string_view("svx"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::voc), std::string_view("voc"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::w64), std::string_view("w64"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::wav), std::string_view("wav"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::wavex), std::string_view("wavex"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_format::wve), std::string_view("wve"));
}

BOOST_AUTO_TEST_CASE(audio_seek_origin_identifiers)
{
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_seek_origin::current), std::string_view("current"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_seek_origin::start), std::string_view("start"));
    synthclone::verify_eq(synthclone::get_identifier(synthclone::audio_seek_origin::end), std::string_view("end"));
}

BOOST_AUTO_TEST_SUITE_END()
