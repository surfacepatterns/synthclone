#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.core.tests;
import synthclone.test;

namespace {

    template<std::uint8_t... Values>
    synthclone::midi_control_array
    make_sample_control_array(
        std::integer_sequence<std::uint_least8_t, Values...>
    )
    {
        return synthclone::midi_control_array {
            Values...
        };
    }

    void
    verify_zone_capture_params(
        const synthclone::zone_capture_params& params,
        synthclone::midi_channel expected_channel,
        synthclone::midi_note expected_note,
        synthclone::midi_velocity expected_velocity,
        std::optional<synthclone::midi_aftertouch> expected_aftertouch,
        std::optional<synthclone::midi_channel_pressure> expected_pressure,
        const synthclone::midi_control_array& expected_controls,
        synthclone::audio_duration expected_sample_duration,
        synthclone::audio_duration expected_release_duration
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_zone_capture_params", params, expected_channel,
                expected_note, expected_velocity, expected_aftertouch,
                expected_pressure, expected_controls, expected_sample_duration,
                expected_release_duration));

        synthclone::verify_eq(expected_channel, params.channel());
        synthclone::verify_eq(expected_note, params.note());
        synthclone::verify_eq(expected_velocity, params.velocity());
        synthclone::verify_eq(expected_aftertouch, params.aftertouch());
        synthclone::verify_eq(expected_pressure, params.channel_pressure());
        synthclone::verify_eq(expected_controls, params.controls());
        synthclone::verify_eq(
            expected_sample_duration, params.sample_duration());
        synthclone::verify_eq(
            expected_release_duration, params.release_duration());
    }

    void
    verify_zone_capture_params(
        const synthclone::zone_capture_params& expected_params,
        const synthclone::zone_capture_params& actual_params
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_zone_capture_params", expected_params, actual_params));

        verify_zone_capture_params(
            actual_params, expected_params.channel(), expected_params.note(),
            expected_params.velocity(), expected_params.aftertouch(),
            expected_params.channel_pressure(), expected_params.controls(),
            expected_params.sample_duration(),
            expected_params.release_duration());
    }

}

BOOST_AUTO_TEST_SUITE(zone)

BOOST_AUTO_TEST_CASE(zone_capture_params)
{
    synthclone::midi_control_array unset_controls;
    synthclone::zone_capture_params params_1(
        {
            .channel = 1,
            .note = 40,
            .velocity = 100,
            .sample_duration = 1,
            .release_duration = 2
        });

    verify_zone_capture_params(
        params_1, 1, 40, 100, std::nullopt, std::nullopt, unset_controls, 1,
        2);

    synthclone::midi_control_array set_controls = make_sample_control_array(
        std::make_integer_sequence<std::uint_least8_t, 128>{});
    synthclone::zone_capture_params params_2(
        {
            .channel = 10,
            .note = 80,
            .velocity = 127,
            .aftertouch = 72,
            .channel_pressure = 84,
            .controls = set_controls,
            .sample_duration = 4,
            .release_duration = 8
        });
    verify_zone_capture_params(
        params_2, 10, 80, 127, 72, 84, set_controls, 4, 8);

    synthclone::zone_capture_params params_3(params_1);

    verify_zone_capture_params(params_1, params_3);

    params_1 = params_2;

    verify_zone_capture_params(params_2, params_1);
}

BOOST_AUTO_TEST_CASE(zone_port_params)
{
    synthclone::midi_control_array unset_controls;
    synthclone::zone_port_params params_1(
        {
            .channel = 1,
            .note = 40,
            .velocity = 100
        });

    synthclone::verify_zone_port_params(
        params_1, 1, 40, 100, std::nullopt, std::nullopt, unset_controls,
        std::nullopt);

    synthclone::midi_control_array set_controls = make_sample_control_array(
        std::make_integer_sequence<std::uint_least8_t, 128>{});
    synthclone::zone_port_params params_2(
        {
            .channel = 10,
            .note = 80,
            .velocity = 127,
            .aftertouch = 72,
            .channel_pressure = 84,
            .controls = set_controls,
            .input_stream =
                synthclone::make_reference_audio_input_stream<8000, 1>()
        });
    synthclone::verify_zone_port_params(
        params_2, 10, 80, 127, 72, 84, set_controls,
        synthclone::make_reference_audio_input_stream<8000, 1>());

    synthclone::zone_port_params params_3(std::move(params_1));

    synthclone::verify_zone_port_params(
        params_1, 1, 40, 100, std::nullopt, std::nullopt, unset_controls,
        std::nullopt);

    // The stream is mutated when we verify zone index equality.
    (*(params_2.input_stream())).seek(0, synthclone::audio_seek_origin::start);
    params_1 = std::move(params_2);

    synthclone::verify_zone_port_params(
        params_1, 10, 80, 127, 72, 84, set_controls,
        synthclone::make_reference_audio_input_stream<8000, 1>());
}

BOOST_AUTO_TEST_SUITE_END()
