/**
 * @file
 *
 * Contains functionality that allows plugins to create their own capture
 * effect types.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:capture_effect;

import std;

import synthclone.util;

import :audio;
import :component;
import :component_core;

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
        component_event_wait_message,
        component_state_changed_message
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
        component_progress_message,
        component_state_changed_message,
        component_status_message
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
    class capture_effect_core_ops:
        public component_core_ops<capture_effect_instance> {

    public:

        /**
         * Destructs a `capture_effect_core_ops` instance.
         */

        virtual
        ~capture_effect_core_ops() = default;

        /**
         * Applies some transformation to audio in the given input stream and
         * writes the audio to the given output stream.
         *
         * @param instance
         *   The capture effect instance.
         * @param input_stream
         *   The input stream containing the input audio.
         * @param output_stream
         *   The output stream to write audio to.
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
            audio_input_stream& input_stream,
            audio_output_stream& output_stream,
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
    using capture_effect_editor_ops = component_editor_ops<
        capture_effect_instance,
        capture_effect_edit_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::capture_effect_state_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains capture effect state operations.
     */

    export
    using capture_effect_state_ops =
        component_state_ops<capture_effect_instance>;

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
    struct capture_effect_type_init_args final {

        /**
         * The capture effect core operations.
         */

        std::unique_ptr<capture_effect_core_ops> core_ops;

        /**
         * The capture effect external editor operations.
         */

        std::unique_ptr<capture_effect_editor_ops> external_editor_ops;

        /**
         * The capture effect internal editor operations.
         */

        std::unique_ptr<capture_effect_editor_ops> internal_editor_ops;

        /**
         * The capture effect state operations.
         */

        std::unique_ptr<capture_effect_state_ops> state_ops;

        /**
         * Metadata describing effects of the given capture effect type.
         */

        metadata_init_args metadata;

    };

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
                std::move(args.core_ops), std::move(args.external_editor_ops),
                std::move(args.state_ops), std::move(args.metadata)),
            internal_editor_ops_(std::move(args.internal_editor_ops))
        {
            // empty
        }

        /**
         * Gets the (possibly optional) internal editor operations for the
         * capture effect type.
         *
         * @return
         *   A pointer to the internal editor operations.
         */

        constexpr
        const std::unique_ptr<capture_effect_editor_ops>&
        internal_editor_ops()
        const noexcept
        {
            return internal_editor_ops_;
        }

    private:

        std::unique_ptr<capture_effect_editor_ops> internal_editor_ops_;

    };

}
