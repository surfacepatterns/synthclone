/**
 * @file
 *
 * Contains functionality that allows plugins to create their own sampler
 * types.
 */

module;

#include <synthclone/config.h>

// The forward declaration needs to be here so the declaration is not attached
// to the module.
class QQuickItem;

export module synthclone.core:sampler;

import std;

import synthclone.util;

import :app;
import :audio;
import :component_core;
import :midi;
import :operation;
import :session;

///////////////////////////////////////////////////////////////////////////////
// synthclone::sampler_edit_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Variant type containing a message generated during a sampler `edit()`
     * operation.
     */

    export
    using sampler_edit_message = std::variant<
        operation_idle_message,
        operation_state_changed_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sampler_capture_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message generated during the process of capturing audio.
     */

    export
    using sampler_capture_message = std::variant<
        operation_progress_message,
        operation_status_message,
        operation_warning_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sampler_play_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message generated during the process of playing audio.
     */

    export
    using sampler_play_message = std::variant<
        operation_progress_message,
        operation_status_message,
        operation_warning_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sampler_instance
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Sampler instance created by a plugin.
     */

    export
    class sampler_instance: private nonmovable {

    public:

        /**
         * Destroys a `sampler_instance` instance.
         */

        virtual
        ~sampler_instance() = default;

    protected:

        /**
         * Constructs a `sampler_instance` instance.
         */

        explicit
        sampler_instance() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sampler_capture_params_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `sampler_capture_params` instances using aggregate
     * initialization.
     */

    export
    struct sampler_capture_params_init_args final {

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
         * The MIDI control values.
         */

        midi_control_map controls;

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
// synthclone::sampler_capture_params
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains data used when capturing audio from an instrument.
     */

    export
    class sampler_capture_params final {

    public:

        /**
         * Constructs a `sampler_capture_params` instance using the given
         * arguments.
         *
         * @params args
         *   The arguments to use.
         */

        constexpr
        sampler_capture_params(
            const sampler_capture_params_init_args& args
        ) noexcept:
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
        aftertouch() const noexcept
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
        channel() const noexcept
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
        channel_pressure() const noexcept
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
        const midi_control_map&
        controls() const noexcept
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
        note() const noexcept
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
        release_duration() const noexcept
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
        sample_duration() const noexcept
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
        velocity() const noexcept
        {
            return velocity_;
        }

    private:

        midi_control_map controls_;

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
// synthclone::sampler_core_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains core operations for a sampler type.
     */

    export
    class sampler_core_ops: public nonmovable {

    public:

        /**
         * Destructs a `sampler_core_ops` instance.
         */

        virtual
        ~sampler_core_ops() = default;

        /**
         * Activates a sampler.
         *
         * The default implementation does nothing.
         *
         * @param instance
         *   The sampler instance to activate.
         */

        virtual void
        activate(sampler_instance& instance)
        {
            // empty
        }

        /**
         * Captures audio corresponding to the given capture parameters.
         *
         * @param instance
         *   The sampler instance.
         * @param capture_params
         *   The capture parameters.
         * @param sink
         *   The sink to which the captured audio should be written.
         * @param stop_token
         *   A token to monitor for cancellation.
         *
         * @return
         *   A generator used to emit messages during the process.
         */

        virtual std::generator<sampler_capture_message>
        capture(
            sampler_instance& instance,
            const sampler_capture_params& capture_params,
            const audio_sink& sink,
            std::stop_token stop_token
        ) = 0;

        /**
         * Instantiates a new sampler instance.
         *
         * @param host
         *   The application host.
         * @param info
         *   Information on the current session.
         *
         * @return
         *   A pointer to the new sampler instance.
         */

        virtual
        std::unique_ptr<sampler_instance>
        create(app_host& host, const session_info& info) = 0;

        /**
         * Deactivates a sampler.
         *
         * The default implementation does nothing.
         *
         * @param instance
         *   The sampler instance to deactivate.
         */

        virtual void
        deactivate(sampler_instance& instance)
        {
            // empty
        }

        /**
         * Checks if the sampler type supports a session with the given info.
         *
         * The default implementation always returns `true`.
         *
         * @param info
         *   The session info to check.
         *
         * @return
         *   A boolean indicating whether or not the session would be
         *   supported.
         */

        virtual
        bool
        is_supported(const session_info& info) const
        {
            return true;
        }

        /**
         * Plays audio from the given audio source.
         *
         * @param instance
         *   The sampler instance.
         * @param source
         *   The audio source containing the audio to play.
         * @param stop_token
         *   A token to monitor for cancellation.
         *
         * @return
         *   A generator used to emit messages during the process.
         */

        virtual std::generator<sampler_play_message>
        play(
            sampler_instance& instance,
            const audio_source& source,
            std::stop_token stop_token
        ) = 0;

    protected:

        /**
         * Constructs a `sampler_core_ops` instance.
         */

        explicit
        sampler_core_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sampler_editor_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains sampler editor operations.
     */

    export
    class sampler_editor_ops: private nonmovable {

    public:

        /**
         * Destructor.
         */

        virtual
        ~sampler_editor_ops() = default;

        /**
         * Populates the given window with an editor interface, allowing the
         * user to edit the sampler state, and manages the window throughout
         * the lifetime of the edit operations (e.g. until the window is
         * closed).
         *
         * @param instance
         *   The sampler instance to be edited.
         * @param parent
         *   The item to use as the parent of the edit interface.
         * @param stop_token
         *   A stop token that will be set if the operation is cancelled.
         *
         * @return
         *   A generator that is used to send messages back to the host.
         */

        virtual
        std::generator<sampler_edit_message>
        edit(
            sampler_instance& instance,
            ::QQuickItem* parent,
            std::stop_token stop_token
        ) = 0;

    protected:

        /**
         * Default constructor.
         */

        explicit
        sampler_editor_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sampler_state_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains sampler state operations.
     */

    export
    class sampler_state_ops: private nonmovable {

    public:

        /**
         * Destructor.
         */

        virtual
        ~sampler_state_ops() = default;

        /**
         * Gets a snapshot of the given sampler's state.
         *
         * @param instance
         *   The sampler instance to get the state for.
         *
         * @return
         *   The sampler state.
         */

        virtual
        state_value
        dump(const sampler_instance& instance) = 0;

        /**
         * Loads a sampler from the given state.
         *
         * @param host
         *   The application host.
         * @param info
         *   Information about the current session.
         * @param version
         *   The version of the sampler type that was used to save the given
         *   state.
         * @param state
         *   The state to load the sampler from.
         *
         * @return
         *   A pointer to the loaded sampler instance.
         */

        virtual
        std::unique_ptr<sampler_instance>
        load(
            app_host& host,
            const session_info& info,
            const metadata_element& version,
            const state_value& state
        ) = 0;

    protected:

        /**
         * Default constructor.
         */

        explicit
        sampler_state_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sampler_type_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `sampler_type` instances using aggregate
     * initialization.
     */

    export
    using sampler_type_init_args = component_type_init_args<
        sampler_core_ops,
        sampler_editor_ops,
        sampler_state_ops
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sampler_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains operations and metadata corresponding to an sampler type.
     */

    export
    class sampler_type final: public component_type<
        sampler_core_ops,
        sampler_editor_ops,
        sampler_state_ops,
        false
    > {

    public:

        /**
         * Constructs an `sampler_type` instance.
         *
         * @param args
         *   The data to use to populate the `sampler_type` instance.
         */

        constexpr
        sampler_type(sampler_type_init_args args):
            component_type(
                std::move(args.core_ops), std::move(args.editor_ops),
                std::move(args.state_ops), std::move(args.metadata))
        {
            // empty
        }

    };

}
