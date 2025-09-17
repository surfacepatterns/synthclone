/**
 * @file
 *
 * Contains functionality that allows plugins to create their own exporter
 * types.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:exporter;

import std;

import synthclone.util;

import :component;
import :component_core;
import :zone;

///////////////////////////////////////////////////////////////////////////////
// synthclone::exporter_request_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message indicating the user requested export of the zones in the current
     * session.
     */

    export
    struct exporter_request_message final {};

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::exporter_edit_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Variant type containing a message generated during an exporter `edit()`
     * operation.
     */

    export
    using exporter_edit_message = std::variant<
        component_event_wait_message,
        component_state_changed_message,
        exporter_request_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::exporter_run_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message generated during the process of applying an exporter to audio.
     */

    export
    using exporter_run_message = std::variant<
        component_progress_message,
        component_status_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::exporter_instance
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Exporter instance created by a plugin.
     */

    export
    class exporter_instance: private nonmovable {

    public:

        /**
         * Destroys an `exporter_instance` instance.
         */

        virtual
        ~exporter_instance() = default;

    protected:

        /**
         * Constructs an `exporter_instance` instance.
         */

        explicit
        exporter_instance() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::exporter_core_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains core operations for an exporter type.
     */

    export
    class exporter_core_ops: public component_core_ops<exporter_instance> {

    public:

        /**
         * Destructs an `exporter_core_ops` instance.
         */

        virtual
        ~exporter_core_ops() = default;

        /**
         * Exports the given zone data.
         *
         * @param instance
         *   The exporter instance.
         * @param zones
         *   A generated range of zones to export.
         * @param stop_token
         *   A token to monitor for cancellation.
         *
         * @return
         *   A generator used to emit messages during the process.
         */

        virtual
        std::generator<exporter_run_message>
        run(
            exporter_instance& instance,
            std::generator<zone_port_params>& zones,
            std::stop_token stop_token
        ) = 0;

    protected:

        /**
         * Constructs an `exporter_core_ops` instance.
         */

        explicit
        exporter_core_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::exporter_editor_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains exporter editor operations.
     */

    export
    using exporter_editor_ops = component_editor_ops<
        exporter_instance,
        exporter_edit_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::exporter_state_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains exporter state operations.
     */

    export
    using exporter_state_ops = component_state_ops<exporter_instance>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::exporter_type_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `exporter_type` instances using aggregate
     * initialization.
     */

    export
    using exporter_type_init_args = component_type_init_args<
        exporter_core_ops,
        exporter_editor_ops,
        exporter_state_ops
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::exporter_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains operations and metadata corresponding to an exporter type.
     */

    export
    using exporter_type = component_type<
        exporter_core_ops,
        exporter_editor_ops,
        exporter_state_ops,
        true
    >;

}
