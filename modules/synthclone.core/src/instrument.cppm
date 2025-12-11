/**
 * @file
 *
 * Contains functionality that allows plugins to create their own instrument
 * types.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:instrument;

import std;

import synthclone.util;

import :audio;
import :component;
import :component_core;
import :midi;
import :zone;

///////////////////////////////////////////////////////////////////////////////
// synthclone::instrument_edit_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Variant type containing a message generated during an instrument
     * `edit()` operation.
     */

    export
    using instrument_edit_message = std::variant<
        component_event_wait_message,
        component_state_changed_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::instrument_run_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message generated during the process of capturing audio from an
     * instrument.
     */

    export
    using instrument_run_message = std::variant<
        component_progress_message,
        component_state_changed_message,
        component_status_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::instrument_instance
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Instrument instance created by a plugin.
     */

    export
    class instrument_instance: private nonmovable {

    public:

        /**
         * Destroys an `instrument_instance` instance.
         */

        virtual
        ~instrument_instance() = default;

    protected:

        /**
         * Constructs an `instrument_instance` instance.
         */

        explicit
        instrument_instance() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::instrument_core_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains core operations for an instrument type.
     */

    export
    class instrument_core_ops: public component_core_ops<instrument_instance> {

    public:

        /**
         * Destructs an `instrument_core_ops` instance.
         */

        virtual
        ~instrument_core_ops() = default;

        /**
         * Captures audio corresponding to the given capture parameters.
         *
         * @param instance
         *   The instrument instance.
         * @param capture_params
         *   The capture parameters.
         * @param output_stream
         *   The output stream to which the captured audio should be written.
         * @param stop_token
         *   A token to monitor for cancellation.
         *
         * @return
         *   A generator used to emit messages during the process.
         */

        virtual std::generator<instrument_run_message>
        run(
            instrument_instance& instance,
            const zone_capture_params& capture_params,
            audio_output_stream& output_stream,
            std::stop_token stop_token
        ) = 0;

    protected:

        /**
         * Constructs a `instrument_core_ops` instance.
         */

        explicit
        instrument_core_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::instrument_editor_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains instrument editor operations.
     */

    export
    using instrument_editor_ops = component_editor_ops<
        instrument_instance,
        instrument_edit_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::instrument_state_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains instrument state operations.
     */

    export
    using instrument_state_ops = component_state_ops<instrument_instance>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::instrument_type_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `instrument_type` instances using aggregate
     * initialization.
     */

    export
    using instrument_type_init_args = component_type_init_args<
        instrument_core_ops,
        instrument_editor_ops,
        instrument_state_ops
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::instrument_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains operations and metadata corresponding to an instrument type.
     */

    export
    class instrument_type final: public component_type<
        instrument_core_ops,
        instrument_editor_ops,
        instrument_state_ops,
        false
    > {

    public:

        /**
         * Constructs an `instrument_type` instance.
         *
         * @param args
         *   The data to use to populate the `instrument_type` instance.
         */

        constexpr
        instrument_type(instrument_type_init_args args):
            component_type(
                std::move(args.core_ops), std::move(args.editor_ops),
                std::move(args.state_ops), std::move(args.metadata))
        {
            // empty
        }

    };

}
