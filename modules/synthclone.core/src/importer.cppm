/**
 * @file
 *
 * Contains functionality that allows plugins to create their own importer
 * types.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:importer;

import std;

import synthclone.util;

import :component;
import :component_core;
import :zone;

///////////////////////////////////////////////////////////////////////////////
// synthclone::importer_request_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message indicating the user requested import of zones.
     */

    export
    struct importer_request_message final {};

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::importer_edit_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Variant type containing a message generated during an importer `edit()`
     * operation.
     */

    export
    using importer_edit_message = std::variant<
        component_event_wait_message,
        component_state_changed_message,
        importer_request_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::importer_zone_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message indicating that a zone should be generated from the contained
     * parameters.
     */

    export
    class importer_zone_message final: private noncopyable {

    public:

        /**
         * Constructs an `importer_zone_message` instance.
         *
         * @param params
         *   The zone parameters to use to construct the zone.
         */

        constexpr explicit
        importer_zone_message(zone_port_params&& params):
            params_(std::move(params))
        {
            // empty
        }

        /**
         * Gets the zone parameters.
         *
         * @return
         *   The zone parameters.
         */

        constexpr
        zone_port_params&
        params()
        noexcept
        {
            return params_;
        }

    private:

        zone_port_params params_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::importer_run_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message generated during the process of importing zones.
     */

    export
    using importer_run_message = std::variant<
        component_progress_message,
        component_state_changed_message,
        component_status_message,
        importer_zone_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::importer_instance
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Importer instance created by a plugin.
     */

    export
    class importer_instance: private nonmovable {

    public:

        /**
         * Destroys an `importer_instance` instance.
         */

        virtual
        ~importer_instance() = default;

    protected:

        /**
         * Constructs an `importer_instance` instance.
         */

        explicit
        importer_instance() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::importer_core_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains core operations for an importer type.
     */

    export
    class importer_core_ops: public component_core_ops<importer_instance> {

    public:

        /**
         * Destructs an `importer_core_ops` instance.
         */

        virtual
        ~importer_core_ops() = default;

        /**
         * Imports zone data.
         *
         * @param instance
         *   The importer instance.
         * @param stop_token
         *   A token to monitor for cancellation.
         *
         * @return
         *   A generator used to emit messages during the process.
         */

        virtual
        std::generator<importer_run_message>
        run(importer_instance& instance, std::stop_token stop_token) = 0;

    protected:

        /**
         * Constructs an `importer_core_ops` instance.
         */

        explicit
        importer_core_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::importer_editor_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains importer editor operations.
     */

    export
    using importer_editor_ops = component_editor_ops<
        importer_instance,
        importer_edit_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::importer_state_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains importer state operations.
     */

    export
    using importer_state_ops = component_state_ops<importer_instance>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::importer_type_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `importer_type` instances using aggregate
     * initialization.
     */

    export
    using importer_type_init_args = component_type_init_args<
        importer_core_ops,
        importer_editor_ops,
        importer_state_ops
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::importer_type
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains operations and metadata corresponding to an importer type.
     */

    export
    class importer_type final: public component_type<
        importer_core_ops,
        importer_editor_ops,
        importer_state_ops,
        true
    > {

    public:

        /**
         * Constructs an `importer_type` instance.
         *
         * @param args
         *   The data to use to populate the `importer_type` instance.
         */

        constexpr
        importer_type(importer_type_init_args args):
            component_type(
                std::move(args.core_ops), std::move(args.external_editor_ops),
                std::move(args.state_ops), std::move(args.metadata))
        {
            // empty
        }

    };

}
