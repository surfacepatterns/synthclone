/**
 * @file
 *
 * Contains generated MIDI enums and tables.
 *
 * @note
 *   This code was auto-generated at:
 *
 *       2025-12-04 19:02:11.568149+00:00
 *
 *   Do *not* edit this code.  Any changes made to this code will be lost.
 *
 *   Channel mode lookup data is a slightly modified version of the data here:
 *
 *       https://midi.org/midi-1-0-control-change-messages
 *
 *   MIDI note lookup data is a slightly modified version of the data here:
 *
 *       https://soundprogramming.net/file-formats/midi-note-frequencies/
 */

module;

#include <synthclone/config.h>

export module synthclone.core:midi_gen;

import std;

import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_control_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains the default control types as interpreted from the MIDI standard.
     */

    export
    enum class midi_control_type: std::uint_least8_t {

        /**
         * Indicates that there is not a defined type for the control.
         */

        undefined = 0,

        /**
         * Indicates a command that can be initiated by sending a 0 control value.
         */

        command = 1,

        /**
         * Indicates a continuous controller.
         */

        continuous = 2,

        /**
         * Turns local control on (127) or off (0).
         */

        local = 3,

        /**
         * Turns on mono mode, where the control value is set to the number of channels, or to 0 if the number of channels equals the number of voices on the receiver.
         */

        mono_mode = 4,

        /**
         * Changes data by one step (increment/decrement).
         */

        step = 5,

        /**
         * Indicates a controller that switches between two different states, where one of the states can be set with values 0 - 63, and the other state can be set with values 64 - 127.
         */

        toggle = 6

    };

    /**
     * The number of `midi_control_type` elements
     */

    export
    constexpr
    std::size_t midi_control_type_count = 7;

    constexpr
    lookup_table<std::string_view, 7> midi_control_type_identifier_table {
        std::string_view("undefined"),
        std::string_view("command"),
        std::string_view("continuous"),
        std::string_view("local"),
        std::string_view("mono_mode"),
        std::string_view("step"),
        std::string_view("toggle")
    };

    /**
     * Gets the identifier for the given `midi_control_type` element.
     * 
     * @param element
     *   The `midi_control_type` element.
     * 
     * @return
     *   The identifier.
     */

    export
    constexpr
    std::string_view
    get_identifier(midi_control_type element)
    {
        return midi_control_type_identifier_table.at(
            static_cast<std::size_t>(element));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_control_channel_mode_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr
    lookup_table<bool, 128> midi_control_channel_mode_table {
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        false,
        true,
        true,
        true,
        true,
        true,
        true,
        true,
        true
    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_control_name_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr
    lookup_table<std::string_view, 128> midi_control_name_table {
        std::string_view("Bank Select"),
        std::string_view("Modulation Wheel or Lever"),
        std::string_view("Breath Controller"),
        std::string_view("Undefined"),
        std::string_view("Foot Controller"),
        std::string_view("Portamento Time"),
        std::string_view("Data Entry MSB"),
        std::string_view("Channel Volume"),
        std::string_view("Balance"),
        std::string_view("Undefined"),
        std::string_view("Pan"),
        std::string_view("Expression Controller"),
        std::string_view("Effect Control 1"),
        std::string_view("Effect Control 2"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("General Purpose Controller 1"),
        std::string_view("General Purpose Controller 2"),
        std::string_view("General Purpose Controller 3"),
        std::string_view("General Purpose Controller 4"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("LSB for Control 0 (Bank Select)"),
        std::string_view("LSB for Control 1 (Modulation Wheel or Lever)"),
        std::string_view("LSB for Control 2 (Breath Controller)"),
        std::string_view("LSB for Control 3 (Undefined)"),
        std::string_view("LSB for Control 4 (Foot Controller)"),
        std::string_view("LSB for Control 5 (Portamento Time)"),
        std::string_view("LSB for Control 6 (Data Entry)"),
        std::string_view("LSB for Control 7 (Channel Volume)"),
        std::string_view("LSB for Control 8 (Balance)"),
        std::string_view("LSB for Control 9 (Undefined)"),
        std::string_view("LSB for Control 10 (Pan)"),
        std::string_view("LSB for Control 11 (Expression Controller)"),
        std::string_view("LSB for Control 12 (Effect control 1)"),
        std::string_view("LSB for Control 13 (Effect control 2)"),
        std::string_view("LSB for Control 14 (Undefined)"),
        std::string_view("LSB for Control 15 (Undefined)"),
        std::string_view("LSB for Control 16 (General Purpose Controller 1)"),
        std::string_view("LSB for Control 17 (General Purpose Controller 2)"),
        std::string_view("LSB for Control 18 (General Purpose Controller 3)"),
        std::string_view("LSB for Control 19 (General Purpose Controller 4)"),
        std::string_view("LSB for Control 20 (Undefined)"),
        std::string_view("LSB for Control 21 (Undefined)"),
        std::string_view("LSB for Control 22 (Undefined)"),
        std::string_view("LSB for Control 23 (Undefined)"),
        std::string_view("LSB for Control 24 (Undefined)"),
        std::string_view("LSB for Control 25 (Undefined)"),
        std::string_view("LSB for Control 26 (Undefined)"),
        std::string_view("LSB for Control 27 (Undefined)"),
        std::string_view("LSB for Control 28 (Undefined)"),
        std::string_view("LSB for Control 29 (Undefined)"),
        std::string_view("LSB for Control 30 (Undefined)"),
        std::string_view("LSB for Control 31 (Undefined)"),
        std::string_view("Damper Pedal on/off (Sustain)"),
        std::string_view("Portamento On/Off"),
        std::string_view("Sostenuto On/Off"),
        std::string_view("Soft Pedal On/Off"),
        std::string_view("Legato Footswitch"),
        std::string_view("Hold 2"),
        std::string_view("Sound Controller 1 (default: Sound Variation)"),
        std::string_view("Sound Controller 2 (default: Timbre/Harmonic Intens.)"),
        std::string_view("Sound Controller 3 (default: Release Time)"),
        std::string_view("Sound Controller 4 (default: Attack Time)"),
        std::string_view("Sound Controller 5 (default: Brightness)"),
        std::string_view("Sound Controller 6 (default: Decay Time)"),
        std::string_view("Sound Controller 7 (default: Vibrato Rate)"),
        std::string_view("Sound Controller 8 (default: Vibrato Depth)"),
        std::string_view("Sound Controller 9 (default: Vibrato Delay)"),
        std::string_view("Sound Controller 10 (default undefined)"),
        std::string_view("General Purpose Controller 5"),
        std::string_view("General Purpose Controller 6"),
        std::string_view("General Purpose Controller 7"),
        std::string_view("General Purpose Controller 8"),
        std::string_view("Portamento Control"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("High Resolution Velocity Prefix"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Effects 1 Depth (default: Reverb Send Level)"),
        std::string_view("Effects 2 Depth"),
        std::string_view("Effects 3 Depth (default: Chorus Send Level)"),
        std::string_view("Effects 4 Depth"),
        std::string_view("Effects 5 Depth"),
        std::string_view("Data Increment (Data Entry +1)"),
        std::string_view("Data Decrement (Data Entry -1)"),
        std::string_view("Non-Registered Parameter Number (NRPN) \U00002013 LSB"),
        std::string_view("Non-Registered Parameter Number (NRPN) \U00002013 MSB"),
        std::string_view("Registered Parameter Number (RPN) \U00002013 LSB"),
        std::string_view("Registered Parameter Number (RPN) \U00002013 MSB"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("Undefined"),
        std::string_view("[Channel Mode Message] All Sound Off"),
        std::string_view("[Channel Mode Message] Reset All Controllers"),
        std::string_view("[Channel Mode Message] Local Control On/Off"),
        std::string_view("[Channel Mode Message] All Notes Off"),
        std::string_view("[Channel Mode Message] Omni Mode Off (+ all notes off)"),
        std::string_view("[Channel Mode Message] Omni Mode On (+ all notes off)"),
        std::string_view("[Channel Mode Message] Mono Mode On (+ poly off, + all notes off)"),
        std::string_view("[Channel Mode Message] Poly Mode On (+ mono off, +all notes off)")
    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_control_type_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr
    lookup_table<midi_control_type, 128> midi_control_type_table {
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::toggle,
        midi_control_type::toggle,
        midi_control_type::toggle,
        midi_control_type::toggle,
        midi_control_type::toggle,
        midi_control_type::toggle,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::continuous,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::step,
        midi_control_type::step,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::continuous,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::undefined,
        midi_control_type::command,
        midi_control_type::command,
        midi_control_type::local,
        midi_control_type::command,
        midi_control_type::command,
        midi_control_type::command,
        midi_control_type::mono_mode,
        midi_control_type::command
    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_note_name_table
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    constexpr
    lookup_table<std::string_view, 128> midi_note_name_table {
        std::string_view("C-1"),
        std::string_view("C#-1"),
        std::string_view("D-1"),
        std::string_view("D#-1"),
        std::string_view("E-1"),
        std::string_view("F-1"),
        std::string_view("F#-1"),
        std::string_view("G-1"),
        std::string_view("G#-1"),
        std::string_view("A-1"),
        std::string_view("A#-1"),
        std::string_view("B-1"),
        std::string_view("C0"),
        std::string_view("C#0"),
        std::string_view("D0"),
        std::string_view("D#0"),
        std::string_view("E0"),
        std::string_view("F0"),
        std::string_view("F#0"),
        std::string_view("G0"),
        std::string_view("G#0"),
        std::string_view("A0"),
        std::string_view("A#0"),
        std::string_view("B0"),
        std::string_view("C1"),
        std::string_view("C#1"),
        std::string_view("D1"),
        std::string_view("D#1"),
        std::string_view("E1"),
        std::string_view("F1"),
        std::string_view("F#1"),
        std::string_view("G1"),
        std::string_view("G#1"),
        std::string_view("A1"),
        std::string_view("A#1"),
        std::string_view("B1"),
        std::string_view("C2"),
        std::string_view("C#2"),
        std::string_view("D2"),
        std::string_view("D#2"),
        std::string_view("E2"),
        std::string_view("F2"),
        std::string_view("F#2"),
        std::string_view("G2"),
        std::string_view("G#2"),
        std::string_view("A2"),
        std::string_view("A#2"),
        std::string_view("B2"),
        std::string_view("C3"),
        std::string_view("C#3"),
        std::string_view("D3"),
        std::string_view("D#3"),
        std::string_view("E3"),
        std::string_view("F3"),
        std::string_view("F#3"),
        std::string_view("G3"),
        std::string_view("G#3"),
        std::string_view("A3"),
        std::string_view("A#3"),
        std::string_view("B3"),
        std::string_view("C4"),
        std::string_view("C#4"),
        std::string_view("D4"),
        std::string_view("D#4"),
        std::string_view("E4"),
        std::string_view("F4"),
        std::string_view("F#4"),
        std::string_view("G4"),
        std::string_view("G#4"),
        std::string_view("A4"),
        std::string_view("A#4"),
        std::string_view("B4"),
        std::string_view("C5"),
        std::string_view("C#5"),
        std::string_view("D5"),
        std::string_view("D#5"),
        std::string_view("E5"),
        std::string_view("F5"),
        std::string_view("F#5"),
        std::string_view("G5"),
        std::string_view("G#5"),
        std::string_view("A5"),
        std::string_view("A#5"),
        std::string_view("B5"),
        std::string_view("C6"),
        std::string_view("C#6"),
        std::string_view("D6"),
        std::string_view("D#6"),
        std::string_view("E6"),
        std::string_view("F6"),
        std::string_view("F#6"),
        std::string_view("G6"),
        std::string_view("G#6"),
        std::string_view("A6"),
        std::string_view("A#6"),
        std::string_view("B6"),
        std::string_view("C7"),
        std::string_view("C#7"),
        std::string_view("D7"),
        std::string_view("D#7"),
        std::string_view("E7"),
        std::string_view("F7"),
        std::string_view("F#7"),
        std::string_view("G7"),
        std::string_view("G#7"),
        std::string_view("A7"),
        std::string_view("A#7"),
        std::string_view("B7"),
        std::string_view("C8"),
        std::string_view("C#8"),
        std::string_view("D8"),
        std::string_view("D#8"),
        std::string_view("E8"),
        std::string_view("F8"),
        std::string_view("F#8"),
        std::string_view("G8"),
        std::string_view("G#8"),
        std::string_view("A8"),
        std::string_view("A#8"),
        std::string_view("B8"),
        std::string_view("C9"),
        std::string_view("C#9"),
        std::string_view("D9"),
        std::string_view("D#9"),
        std::string_view("E9"),
        std::string_view("F9"),
        std::string_view("F#9"),
        std::string_view("G9")
    };

}
