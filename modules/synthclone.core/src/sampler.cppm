/**
 * @file
 *
 * Contains functionality that allows plugins to create their own sampler
 * types.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:sampler;

import std;

import synthclone.util;

import :audio;
import :component;
import :component_core;
import :midi;
import :zone;

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
        component_event_wait_message,
        component_state_changed_message
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
        component_progress_message,
        component_status_message
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
        component_progress_message,
        component_status_message
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
// synthclone::sampler_core_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains core operations for a sampler type.
     */

    export
    class sampler_core_ops: public component_core_ops<sampler_instance> {

    public:

        /**
         * Destructs a `sampler_core_ops` instance.
         */

        virtual
        ~sampler_core_ops() = default;

        /**
         * Captures audio corresponding to the given capture parameters.
         *
         * @param instance
         *   The sampler instance.
         * @param capture_params
         *   The sample capture parameters.
         * @param output_stream
         *   The output stream to which the captured audio should be written.
         * @param stop_token
         *   A token to monitor for cancellation.
         *
         * @return
         *   A generator used to emit messages during the process.
         */

        virtual std::generator<sampler_capture_message>
        capture(
            sampler_instance& instance,
            const zone_capture_params& capture_params,
            audio_output_stream& output_stream,
            std::stop_token stop_token
        ) = 0;

        /**
         * Plays audio from the given audio input stream.
         *
         * @param instance
         *   The sampler instance.
         * @param input_stream
         *   The input stream containing the audio to play.
         * @param stop_token
         *   A token to monitor for cancellation.
         *
         * @return
         *   A generator used to emit messages during the process.
         */

        virtual std::generator<sampler_play_message>
        play(
            sampler_instance& instance,
            audio_input_stream& input_stream,
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
    using sampler_editor_ops = component_editor_ops<
        sampler_instance,
        sampler_edit_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sampler_state_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains sampler state operations.
     */

    export
    using sampler_state_ops = component_state_ops<sampler_instance>;

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
     * Contains operations and metadata corresponding to a sampler type.
     */

    export
    using sampler_type = component_type<
        sampler_core_ops,
        sampler_editor_ops,
        sampler_state_ops
    >;

}
