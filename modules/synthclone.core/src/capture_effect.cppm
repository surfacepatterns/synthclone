/**
 * @file
 *
 * Contains functionality that allows plugins to create their own capture
 * effect types.
 */

module;

#include <synthclone/config.h>

// The forward declaration needs to be here so the declaration is not attached
// to the module.
class QQuickItem;

export module synthclone.core:capture_effect;

import std;

import synthclone.util;

import :app;
import :audio;
import :component_core;
import :operation;
import :session;

///////////////////////////////////////////////////////////////////////////////
// synthclone::capture_effect_edit_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Variant type containing a message generated during a capture effect
     * `edit()` operation.
     */

    export
    using capture_effect_edit_message = std::variant<
        operation_idle_message,
        operation_state_changed_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::capture_effect_run_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message generated during the process of applying a capture effect to
     * audio.
     */

    export
    using capture_effect_run_message = std::variant<
        operation_progress_message,
        operation_state_changed_message,
        operation_status_message,
        operation_warning_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::capture_effect_instance
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Capture effect instance created by a plugin.
     */

    export
    class capture_effect_instance: private nonmovable {

    public:

        /**
         * Destroys a `capture_effect_instance` instance.
         */

        virtual
        ~capture_effect_instance() = default;

    protected:

        /**
         * Constructs a `capture_effect_instance` instance.
         */

        explicit
        capture_effect_instance() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::capture_effect_core_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains core operations for a effect type.
     */

    export
    class capture_effect_core_ops: private nonmovable {

    public:

        /**
         * Destructs a `capture_effect_core_ops` instance.
         */

        virtual
        ~capture_effect_core_ops() = default;

        /**
         * Instantiates a new capture effect instance.
         *
         * @param host
         *   The application host.
         * @param info
         *   Information on the current session.
         *
         * @return
         *   A pointer to the new capture effect instance.
         */

        virtual
        std::unique_ptr<capture_effect_instance>
        create(app_host& host, const session_info& info) = 0;

        /**
         * Checks if the capture effect type supports a session with the given
         * info.
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
         * Applies some transformation to audio from the given source and
         * writes the audio to the given sink.
         *
         * @param instance
         *   The capture effect instance.
         * @param source
         *   The audio source.
         * @param sink
         *   The audio sink.
         * @param stop_token
         *   A token to monitor for cancellation.
         *
         * @return
         *   A generator used to emit messages during the process.
         */

        virtual
        std::generator<capture_effect_run_message>
        run(
            capture_effect_instance& instance,
            const audio_source& source,
            const audio_sink& sink,
            std::stop_token stop_token
        ) = 0;

    protected:

        /**
         * Constructs a `capture_effect_core_ops` instance.
         */

        explicit
        capture_effect_core_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::capture_effect_editor_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains capture effect editor operations.
     */

    export
    class capture_effect_editor_ops: private nonmovable {

    public:

        /**
         * Destructor.
         */

        virtual
        ~capture_effect_editor_ops() = default;

        /**
         * Populates the given window with an editor interface, allowing the
         * user to edit the effect state, and manages the window throughout the
         * lifetime of the edit operations (e.g. until the window is closed).
         *
         * @param instance
         *   The effect instance to be edited.
         * @param parent
         *   The item to use as the parent of the edit interface.
         * @param stop_token
         *   A stop token that will be set if the operation is cancelled.
         *
         * @return
         *   A generator that is used to send messages back to the host.
         */

        virtual
        std::generator<capture_effect_edit_message>
        edit(
            capture_effect_instance& instance,
            ::QQuickItem* parent,
            std::stop_token stop_token
        ) = 0;

    protected:

        /**
         * Default constructor.
         */

        explicit
        capture_effect_editor_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::capture_effect_state_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains capture effect state operations.
     */

    export
    class capture_effect_state_ops: private nonmovable {

    public:

        /**
         * Destructor.
         */

        virtual
        ~capture_effect_state_ops() = default;

        /**
         * Gets a snapshot of the given capture effect's state.
         *
         * @param instance
         *   The capture effect instance to get the state for.
         *
         * @return
         *   The capture effect state.
         */

        virtual
        state_value
        dump(const capture_effect_instance& instance) = 0;

        /**
         * Loads a capture effect from the given state.
         *
         * @param host
         *   The application host.
         * @param info
         *   Information about the current session.
         * @param version
         *   The version of the capture effect type that was used to save the
         *   given state.
         * @param state
         *   The state to load the capture effect from.
         *
         * @return
         *   A pointer to the loaded capture effect instance.
         */

        virtual
        std::unique_ptr<capture_effect_instance>
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
        capture_effect_state_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::capture_effect_type_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `capture_effect_type` instances using aggregate
     * initialization.
     */

    export
    using capture_effect_type_init_args = component_type_init_args<
        capture_effect_core_ops,
        capture_effect_editor_ops,
        capture_effect_state_ops
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::capture_effect_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains operations and metadata corresponding to a capture effect type.
     */

    export
    class capture_effect_type final: public component_type<
        capture_effect_core_ops,
        capture_effect_editor_ops,
        capture_effect_state_ops,
        false
    > {

    public:

        /**
         * Constructs a `capture_effect_type` instance.
         *
         * @param args
         *   The data to use to populate the `capture_effect_type` instance.
         */

        constexpr
        capture_effect_type(capture_effect_type_init_args args):
            component_type(
                std::move(args.core_ops), std::move(args.editor_ops),
                std::move(args.state_ops), std::move(args.metadata))
        {
            // empty
        }

    };

}
