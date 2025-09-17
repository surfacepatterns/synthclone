/**
 * @file
 *
 * Contains functionality that allows plugins to create their own effect types.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:effect;

import std;

import synthclone.util;

import :audio;
import :component;
import :component_core;

///////////////////////////////////////////////////////////////////////////////
// synthclone::effect_edit_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Variant type containing a message generated during an effect `edit()`
     * operation.
     */

    export
    using effect_edit_message = std::variant<
        component_event_wait_message,
        component_state_changed_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::effect_run_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message generated during the process of applying an effect to audio.
     */

    export
    using effect_run_message = std::variant<
        component_progress_message,
        component_status_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::effect_instance
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Effect instance created by a plugin.
     */

    export
    class effect_instance: private nonmovable {

    public:

        /**
         * Destroys an `effect_instance` instance.
         */

        virtual
        ~effect_instance() = default;

    protected:

        /**
         * Constructs an `effect_instance` instance.
         */

        explicit
        effect_instance() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::effect_core_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains core operations for an effect type.
     */

    export
    class effect_core_ops: public component_core_ops<effect_instance> {

    public:

        /**
         * Destructs an `effect_core_ops` instance.
         */

        virtual
        ~effect_core_ops() = default;

        /**
         * Applies some transformation to audio in the given input stream and
         * writes the audio to the given output stream.
         *
         * @param instance
         *   The effect instance.
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
        std::generator<effect_run_message>
        run(
            effect_instance& instance,
            audio_input_stream& input_stream,
            audio_output_stream& output_stream,
            std::stop_token stop_token
        ) = 0;

    protected:

        /**
         * Constructs an `effect_core_ops` instance.
         */

        explicit
        effect_core_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::effect_editor_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains effect editor operations.
     */

    export
    using effect_editor_ops = component_editor_ops<
        effect_instance,
        effect_edit_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::effect_state_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains effect state operations.
     */

    export
    using effect_state_ops = component_state_ops<effect_instance>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::effect_type_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `effect_type` instances using aggregate
     * initialization.
     */

    export
    using effect_type_init_args = component_type_init_args<
        effect_core_ops,
        effect_editor_ops,
        effect_state_ops
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::effect_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains operations and metadata corresponding to an effect type.
     */

    export
    using effect_type = component_type<
        effect_core_ops,
        effect_editor_ops,
        effect_state_ops
    >;

}
