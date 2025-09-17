/**
 * @file
 *
 * Base module for all things MIDI.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:midi;

import :midi_core;
import :midi_gen;

namespace SYNTHCLONE_LIB_NAMESPACE {

    export using synthclone::midi_aftertouch;
    export using synthclone::midi_channel;
    export using synthclone::midi_channel_pressure;
    export using synthclone::midi_control_array;
    export using synthclone::midi_control_index;
    export using synthclone::midi_control_type;
    export using synthclone::midi_control_value;
    export using synthclone::midi_note;
    export using synthclone::midi_velocity;

    export using synthclone::get_identifier;
    export using synthclone::get_name;
    export using synthclone::get_type;
    export using synthclone::is_channel_mode;

}
