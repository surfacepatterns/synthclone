/**
 * @file
 *
 * Contains MIDI type definitions.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:midi_core;

import std;

import synthclone.util;

import :midi_gen;

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_aftertouch
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    std::uint_least8_t
    verify_midi_aftertouch(const std::uint_least8_t n)
    {
        verify(n <= 127, "{0}: invalid MIDI aftertouch value", n);
        return n;
    }

    /**
     * Contains a valid MIDI aftertouch value.
     */

    export
    class midi_aftertouch final: public uint_least8_proxy<midi_aftertouch> {

    public:

        /**
         * Constructs a `midi_aftertouch` instance.
         *
         * @param n
         *   The MIDI aftertouch value.
         */

        constexpr
        midi_aftertouch(const std::uint_least8_t n):
            uint_least8_proxy<midi_aftertouch>(verify_midi_aftertouch(n))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_channel
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    std::uint_least8_t
    verify_midi_channel(const std::uint_least8_t n)
    {
        verify(n <= 15, "{0}: invalid MIDI channel value", n);
        return n;
    }

    /**
     * Contains a valid MIDI channel.
     *
     * @note
     *   The MIDI channel value is the nybble representation of the MIDI
     *   channel in a MIDI message.  In a MIDI message, channel 1 is
     *   represented by the nybble 0000, channel 2 is represented by the nybble
     *   0001, and so forth.
     */

    export
    class midi_channel final: public uint_least8_proxy<midi_channel> {

    public:

        /**
         * Constructs a `midi_channel` instance.
         *
         * @param n
         *   The MIDI channel value.
         */

        constexpr
        midi_channel(const std::uint_least8_t n):
            uint_least8_proxy<midi_channel>(verify_midi_channel(n))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_control_index
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    std::uint_least8_t
    verify_midi_control_index(const std::uint_least8_t n)
    {
        verify(n <= 127, "{0}: invalid MIDI control index", n);
        return n;
    }

    /**
     * Contains a valid MIDI control index.
     */

    export
    class midi_control_index final:
        public uint_least8_proxy<midi_control_index> {

    public:

        /**
         * Constructs a `midi_control_index` instance.
         *
         * @param n
         *   The MIDI control index.
         */

        inline
        midi_control_index(const std::uint_least8_t n):
            uint_least8_proxy<midi_control_index>(verify_midi_control_index(n))
        {
            // empty
        }

    };

    /**
     * Gets the standard control name corresponding to the given control index.
     *
     * @param index
     *   The control index.
     *
     * @return
     *   The control name.
     */

    export
    constexpr
    std::string_view
    get_name(midi_control_index index) noexcept
    {
        return midi_control_name_table[index.value()];
    }

    /**
     * Gets the control type corresponding to the given control index.
     *
     * @param index
     *   The control index.
     *
     * @return
     *   The control type.
     */

    export
    constexpr
    midi_control_type
    get_type(midi_control_index index) noexcept
    {
        return midi_control_type_table[index.value()];
    }

    /**
     * Gets a boolean indicating whether or not the control index is associated
     * with a channel mode control.
     *
     * @param index
     *   The control index.
     *
     * @return
     *   The boolean indicator.
     */

    export
    constexpr
    bool
    is_channel_mode(midi_control_index index) noexcept
    {
        return midi_control_channel_mode_table[index.value()];
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_control_value
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    std::uint_least8_t
    verify_midi_control_value(const std::uint_least8_t n)
    {
        verify(n <= 127, "{0}: invalid MIDI control value", n);
        return n;
    }

    /**
     * Contains a valid MIDI control value.
     */

    export
    class midi_control_value final:
        public uint_least8_proxy<midi_control_value> {

    public:

        /**
         * Constructs a `midi_control_value` instance.
         *
         * @param n
         *   The MIDI control value.
         */

        constexpr
        midi_control_value(const std::uint_least8_t n):
            uint_least8_proxy<midi_control_value>(verify_midi_control_value(n))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_note
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    std::uint_least8_t
    verify_midi_note(const std::uint_least8_t n)
    {
        verify(n <= 127, "{0}: invalid MIDI note", n);
        return n;
    }

    /**
     * Contains a valid MIDI note value.
     */

    export
    class midi_note final: public uint_least8_proxy<midi_note> {

    public:

        /**
         * Constructs a `midi_note` instance.
         *
         * @param n
         *   The MIDI note value.
         */

        constexpr
        midi_note(const std::uint_least8_t n):
            uint_least8_proxy<midi_note>(verify_midi_note(n))
        {
            // empty
        }

    };

    /**
     * Gets the standard note name corresponding to the given note.
     *
     * @param note
     *   The note.
     *
     * @return
     *   The note name.
     */

    export
    constexpr
    std::string_view
    get_name(midi_note note) noexcept
    {
        return midi_note_name_table[note.value()];
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_velocity
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    std::uint_least8_t
    verify_midi_velocity(const std::uint_least8_t n)
    {
        verify(n <= 127, "{0}: invalid MIDI velocity", n);
        return n;
    }

    /**
     * Contains a valid MIDI velocity value.
     */

    export
    class midi_velocity final: public uint_least8_proxy<midi_velocity> {

    public:

        /**
         * Constructs a `midi_velocity` instance.
         *
         * @param n
         *   The MIDI velocity value.
         */

        constexpr
        midi_velocity(const std::uint_least8_t n):
            uint_least8_proxy<midi_velocity>(verify_midi_velocity(n))
        {
            // empty
        }

    };

}
