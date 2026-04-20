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

    constexpr std::uint_least8_t midi_data_byte_upper_bound = 127;

    constexpr
    bool
    is_valid_midi_data_byte(const std::uint_least8_t n)
    {
        return n <= midi_data_byte_upper_bound;
    }

    constexpr
    std::uint_least8_t
    verify_midi_aftertouch(const std::uint_least8_t n)
    {
        verify(
            is_valid_midi_data_byte(n),
            "{0} is not a valid MIDI aftertouch value", n);
        return n;
    }


    /**
     * Contains a valid MIDI aftertouch value.
     */

    export
    class midi_aftertouch final: public uint_least8_proxy<midi_aftertouch> {

    public:

        /**
         * Creates a new `midi_aftertouch` instance.
         *
         * @param n
         *   The MIDI aftertouch value.
         *
         * @returns
         *   The new `midi_aftertouch` instance, or a `std::error_code`
         *   instance if the value is not valid.
         */

        static constexpr
        std::expected<midi_aftertouch, std::error_code>
        create(const std::uint_least8_t n) noexcept
        {
            if (! is_valid_midi_data_byte(n)) {
                return std::unexpected(
                    std::make_error_code(std::errc::invalid_argument));
            }
            return midi_aftertouch(n, validated);
        }

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

    private:

        constexpr
        midi_aftertouch(const std::uint_least8_t n, validated_t):
            uint_least8_proxy<midi_aftertouch>(n)
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_channel
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr std::uint_least8_t midi_channel_upper_bound = 15;

    constexpr
    bool
    is_valid_midi_channel(const std::uint_least8_t n)
    {
        return n <= midi_channel_upper_bound;
    }

    constexpr
    std::uint_least8_t
    verify_midi_channel(const std::uint_least8_t n)
    {
        verify(is_valid_midi_channel(n), "{0} is not a valid MIDI channel", n);
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
         * Creates a new `midi_aftertouch` instance.
         *
         * @param n
         *   The MIDI aftertouch value.
         *
         * @returns
         *   The new `midi_aftertouch` instance, or a `std::error_code`
         *   instance if the value is not valid.
         */

        static constexpr
        std::expected<midi_channel, std::error_code>
        create(const std::uint_least8_t n) noexcept
        {
            if (! is_valid_midi_channel(n)) {
                return std::unexpected(
                    std::make_error_code(std::errc::invalid_argument));
            }
            return midi_channel(n, validated);
        }

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

    private:

        constexpr
        midi_channel(const std::uint_least8_t n, validated_t):
            uint_least8_proxy<midi_channel>(n)
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_channel_pressure
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    std::uint_least8_t
    verify_midi_channel_pressure(const std::uint_least8_t n)
    {
        verify(
            is_valid_midi_data_byte(n),
            "{0} is not a valid MIDI channel pressure value", n);
        return n;
    }

    /**
     * Contains a valid MIDI channel pressure value.
     */

    export
    class midi_channel_pressure final:
        public uint_least8_proxy<midi_channel_pressure> {

    public:

        /**
         * Creates a new `midi_channel_pressure` instance.
         *
         * @param n
         *   The MIDI channel pressure value.
         *
         * @returns
         *   The new `midi_channel_pressure` instance, or a `std::error_code`
         *   instance if the value is not valid.
         */

        static constexpr
        std::expected<midi_channel_pressure, std::error_code>
        create(const std::uint_least8_t n) noexcept
        {
            if (! is_valid_midi_data_byte(n)) {
                return std::unexpected(
                    std::make_error_code(std::errc::invalid_argument));
            }
            return midi_channel_pressure(n, validated);
        }

        /**
         * Constructs a `midi_channel_pressure` instance.
         *
         * @param n
         *   The MIDI channel pressure value.
         */

        constexpr
        midi_channel_pressure(const std::uint_least8_t n):
            uint_least8_proxy<midi_channel_pressure>(
                verify_midi_channel_pressure(n))
        {
            // empty
        }

    private:

        constexpr
        midi_channel_pressure(const std::uint_least8_t n, validated_t):
            uint_least8_proxy<midi_channel_pressure>(n)
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
        verify(
            is_valid_midi_data_byte(n),
            "{0} is not a valid MIDI control index", n);
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
         * Creates a new `midi_control_index` instance.
         *
         * @param n
         *   The MIDI control index.
         *
         * @returns
         *   The new `midi_control_index` instance, or a `std::error_code`
         *   instance if the value is not valid.
         */

        static constexpr
        std::expected<midi_control_index, std::error_code>
        create(const std::uint_least8_t n) noexcept
        {
            if (! is_valid_midi_data_byte(n)) {
                return std::unexpected(
                    std::make_error_code(std::errc::invalid_argument));
            }
            return midi_control_index(n, validated);
        }

        /**
         * Constructs a `midi_control_index` instance.
         *
         * @param n
         *   The MIDI control index.
         */

        constexpr
        midi_control_index(const std::uint_least8_t n):
            uint_least8_proxy<midi_control_index>(verify_midi_control_index(n))
        {
            // empty
        }

    private:

        constexpr
        midi_control_index(const std::uint_least8_t n, validated_t):
            uint_least8_proxy<midi_control_index>(n)
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
        verify(
            is_valid_midi_data_byte(n),
            "{0} is not a valid MIDI control value", n);
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
         * Creates a new `midi_control_value` instance.
         *
         * @param n
         *   The MIDI control value.
         *
         * @returns
         *   The new `midi_control_value` instance, or a `std::error_code`
         *   instance if the value is not valid.
         */

        static constexpr
        std::expected<midi_control_value, std::error_code>
        create(const std::uint_least8_t n) noexcept
        {
            if (! is_valid_midi_data_byte(n)) {
                return std::unexpected(
                    std::make_error_code(std::errc::invalid_argument));
            }
            return midi_control_value(n, validated);
        }

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

    private:

        constexpr
        midi_control_value(const std::uint_least8_t n, validated_t):
            uint_least8_proxy<midi_control_value>(n)
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_control_map
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Map type that maps control indices to control values.
     */

    export
    using midi_control_map =
        std::flat_map<midi_control_index, midi_control_value>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::midi_note
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    std::uint_least8_t
    verify_midi_note(const std::uint_least8_t n)
    {
        verify(
            is_valid_midi_data_byte(n), "{0} is not a valid MIDI note value",
            n);
        return n;
    }

    /**
     * Contains a valid MIDI note value.
     */

    export
    class midi_note final: public uint_least8_proxy<midi_note> {

    public:

        /**
         * Creates a new `midi_note` instance.
         *
         * @param n
         *   The MIDI note value.
         *
         * @returns
         *   The new `midi_note` instance, or a `std::error_code` instance if
         *   the value is not valid.
         */

        static constexpr
        std::expected<midi_note, std::error_code>
        create(const std::uint_least8_t n) noexcept
        {
            if (! is_valid_midi_data_byte(n)) {
                return std::unexpected(
                    std::make_error_code(std::errc::invalid_argument));
            }
            return midi_note(n, validated);
        }

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

    private:

        constexpr
        midi_note(const std::uint_least8_t n, validated_t):
            uint_least8_proxy<midi_note>(n)
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
        verify(
            is_valid_midi_data_byte(n),
            "{0} is not a valid MIDI velocity value", n);
        return n;
    }

    /**
     * Contains a valid MIDI velocity value.
     */

    export
    class midi_velocity final: public uint_least8_proxy<midi_velocity> {

    public:

        /**
         * Creates a new `midi_velocity` instance.
         *
         * @param n
         *   The MIDI velocity value.
         *
         * @returns
         *   The new `midi_velocity` instance, or a `std::error_code` instance
         *   if the value is not valid.
         */

        static constexpr
        std::expected<midi_velocity, std::error_code>
        create(const std::uint_least8_t n) noexcept
        {
            if (! is_valid_midi_data_byte(n)) {
                return std::unexpected(
                    std::make_error_code(std::errc::invalid_argument));
            }
            return midi_velocity(n, validated);
        }

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

    private:

        constexpr
        midi_velocity(const std::uint_least8_t n, validated_t):
            uint_least8_proxy<midi_velocity>(n)
        {
            // empty
        }

    };

}
