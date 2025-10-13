/**
 * @file
 *
 * Contains portions of zone data needed by components during component
 * operations.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:zone;

import std;

import synthclone.util;

import :audio;
import :midi;

///////////////////////////////////////////////////////////////////////////////
// synthclone::zone_capture_params_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `zone_capture_params` instances using aggregate
     * initialization.
     */

    export
    struct zone_capture_params_init_args final {

        /**
         * The MIDI channel.
         */

        midi_channel channel;

        /**
         * The MIDI note.
         */

        midi_note note;

        /**
         * The MIDI velocity.
         */

        midi_velocity velocity;

        /**
         * The optional MIDI aftertouch value.
         */

        std::optional<midi_aftertouch> aftertouch;

        /**
         * The optional MIDI channel pressure value.
         */

        std::optional<midi_channel_pressure> channel_pressure;

        /**
         * The optional MIDI control values.
         */

        midi_control_array controls;

        /**
         * The time that should be spent capturing instrument audio after the
         * MIDI parameters are sent.
         */

        audio_duration sample_duration;

        /**
         * The additional time that should be spent capturing instrument audio
         * after the MIDI note off event is sent.
         */

        audio_duration release_duration;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::zone_capture_params
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains data used when capturing audio from an instrument.
     */

    export
    class zone_capture_params final {

    public:

        /**
         * Constructs a `zone_capture_params` instance using the given
         * arguments.
         *
         * @params args
         *   The arguments to use.
         */

        constexpr
        zone_capture_params(const zone_capture_params_init_args& args)
        noexcept:
            controls_(args.controls),
            sample_duration_(args.sample_duration),
            release_duration_(args.release_duration),
            aftertouch_(args.aftertouch),
            channel_pressure_(args.channel_pressure),
            channel_(args.channel),
            note_(args.note),
            velocity_(args.velocity)
        {
            // empty
        }

        /**
         * Gets the optional MIDI aftertouch value.
         *
         * @returns
         *   The aftertouch value.
         */

        constexpr
        std::optional<midi_aftertouch>
        aftertouch()
        const noexcept
        {
            return aftertouch_;
        }

        /**
         * Gets the MIDI channel.
         *
         * @returns
         *   The MIDI channel.
         */

        constexpr
        midi_channel
        channel()
        const noexcept
        {
            return channel_;
        }

        /**
         * Gets the optional MIDI channel pressure value.
         *
         * @returns
         *   The pressure value.
         */

        constexpr
        std::optional<midi_channel_pressure>
        channel_pressure()
        const noexcept
        {
            return channel_pressure_;
        }

        /**
         * Gets the MIDI control values.
         *
         * @returns
         *   The control values.
         */

        constexpr
        const midi_control_array&
        controls()
        const noexcept
        {
            return controls_;
        }

        /**
         * Gets the MIDI note value.
         *
         * @returns
         *   The note value.
         */

        constexpr
        midi_note
        note()
        const noexcept
        {
            return note_;
        }

        /**
         * Gets the release duration.
         *
         * @returns
         *   The release duration.
         */

        constexpr
        audio_duration
        release_duration()
        const noexcept
        {
            return release_duration_;
        }

        /**
         * Gets the sample duration.
         *
         * @returns
         *   The sample duration.
         */

        constexpr
        audio_duration
        sample_duration()
        const noexcept
        {
            return sample_duration_;
        }

        /**
         * Gets the MIDI velocity.
         *
         * @returns
         *   The MIDI velocity.
         */

        constexpr
        midi_velocity
        velocity()
        const noexcept
        {
            return velocity_;
        }

    private:

        midi_control_array controls_;

        audio_duration sample_duration_;
        audio_duration release_duration_;

        std::optional<midi_aftertouch> aftertouch_;
        std::optional<midi_channel_pressure> channel_pressure_;

        midi_channel channel_;
        midi_note note_;
        midi_velocity velocity_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::zone_port_params_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `zone_port_params` instances using aggregate
     * initialization.
     */

    export
    struct zone_port_params_init_args final {

        /**
         * The MIDI channel.
         */

        midi_channel channel;

        /**
         * The MIDI note.
         */

        midi_note note;

        /**
         * The MIDI velocity.
         */

        midi_velocity velocity;

        /**
         * The optional MIDI aftertouch value.
         */

        std::optional<midi_aftertouch> aftertouch;

        /**
         * The optional MIDI channel pressure value.
         */

        std::optional<midi_channel_pressure> channel_pressure;

        /**
         * The optional MIDI control values.
         */

        midi_control_array controls;

        /**
         * An optional audio input stream referencing the sample associated
         * with the zone.
         */

        std::optional<audio_input_stream> input_stream;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::zone_port_params
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    std::optional<audio_input_stream>&&
    verify_zone_port_input_stream(std::optional<audio_input_stream>&& stream)
    {
        verify(
            ! (stream && stream->closed()),
            "the given audio input stream is closed");
        return std::move(stream);
    }

    /**
     * Contains data used by importers and exporters to import/export a zone.
     */

    export
    class zone_port_params final: private noncopyable {

    public:

        /**
         * Constructs a `zone_port_params` instance using the given arguments.
         *
         * @params args
         *   The arguments to use.
         */

        inline
        zone_port_params(zone_port_params_init_args args):
            controls_(args.controls),
            input_stream_(
                verify_zone_port_input_stream(std::move(args.input_stream))),
            aftertouch_(args.aftertouch),
            channel_pressure_(args.channel_pressure),
            channel_(args.channel),
            note_(args.note),
            velocity_(args.velocity)
        {
            // empty
        }

        /**
         * Gets the optional MIDI aftertouch value.
         *
         * @returns
         *   The aftertouch value.
         */

        constexpr
        std::optional<midi_aftertouch>
        aftertouch()
        const noexcept
        {
            return aftertouch_;
        }

        /**
         * Gets the MIDI channel.
         *
         * @returns
         *   The MIDI channel.
         */

        constexpr
        midi_channel
        channel()
        const noexcept
        {
            return channel_;
        }

        /**
         * Gets the optional MIDI channel pressure value.
         *
         * @returns
         *   The pressure value.
         */

        constexpr
        std::optional<midi_channel_pressure>
        channel_pressure()
        const noexcept
        {
            return channel_pressure_;
        }

        /**
         * Gets the MIDI control values.
         *
         * @returns
         *   The control values.
         */

        constexpr
        const midi_control_array&
        controls()
        const noexcept
        {
            return controls_;
        }

        /**
         * Gets the optional input stream associated with the zone.
         *
         * @return
         *   The input stream.
         */

        constexpr
        std::optional<audio_input_stream>&
        input_stream()
        noexcept
        {
            return input_stream_;
        }

        /**
         * Gets the MIDI note value.
         *
         * @returns
         *   The note value.
         */

        constexpr
        midi_note
        note()
        const noexcept
        {
            return note_;
        }

        /**
         * Gets the MIDI velocity.
         *
         * @returns
         *   The MIDI velocity.
         */

        constexpr
        midi_velocity
        velocity()
        const noexcept
        {
            return velocity_;
        }

    private:

        midi_control_array controls_;

        std::optional<audio_input_stream> input_stream_;

        std::optional<midi_aftertouch> aftertouch_;
        std::optional<midi_channel_pressure> channel_pressure_;

        midi_channel channel_;
        midi_note note_;
        midi_velocity velocity_;

    };

}
