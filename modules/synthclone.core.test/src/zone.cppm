module;

#include <boost/test/unit_test.hpp>

export module synthclone.core.test:zone;

import std;

import synthclone.core;
import synthclone.test;

import :audio;

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_zone_port_params()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    void
    verify_zone_port_params(
        zone_port_params& params,
        midi_channel expected_channel,
        midi_note expected_note,
        midi_velocity expected_velocity,
        std::optional<midi_aftertouch> expected_aftertouch,
        std::optional<midi_channel_pressure> expected_pressure,
        const midi_control_array& expected_controls,
        std::optional<audio_input_stream> expected_input_stream
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_zone_port_params", params,
                expected_channel, expected_note, expected_velocity,
                expected_aftertouch, expected_pressure, expected_controls,
                expected_input_stream));

        verify_eq(expected_channel, params.channel());
        verify_eq(expected_note, params.note());
        verify_eq(expected_velocity, params.velocity());
        verify_eq(expected_aftertouch, params.aftertouch());
        verify_eq(expected_pressure, params.channel_pressure());
        verify_eq(expected_controls, params.controls());

        auto& actual_input_stream = params.input_stream();
        if (! expected_input_stream) {
            BOOST_CHECK(! actual_input_stream);
        } else {
            verify_audio_input_stream(
                *expected_input_stream, *actual_input_stream);
        }
    }

}
