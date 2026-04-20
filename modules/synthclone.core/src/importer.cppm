/**
 * @file
 *
 * Contains functionality that allows plugins to create their own importer
 * types.
 */

module;

#include <synthclone/config.h>

// The forward declaration needs to be here so the declaration is not attached
// to the module.
class QQuickItem;

export module synthclone.core:importer;

import std;

import synthclone.util;

import :app;
import :component_core;
import :operation;
import :snapshot;

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
        operation_idle_message,
        operation_state_changed_message,
        importer_request_message
    >;

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
        operation_progress_message,
        operation_state_changed_message,
        operation_status_message,
        operation_warning_message
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
    class importer_core_ops: public nonmovable {

    public:

        /**
         * Destructs an `importer_core_ops` instance.
         */

        virtual
        ~importer_core_ops() = default;

        /**
         * Instantiates a new component instance.
         *
         * @param host
         *   The application host.
         *
         * @return
         *   A pointer to the new component instance.
         */

        virtual
        std::unique_ptr<importer_instance>
        create(app_host& host) = 0;

        /**
         * Imports session data.
         *
         * @param instance
         *   The importer instance.
         * @param result
         *   A parameter to use to store the imported session snapshot.
         * @param stop_token
         *   A token to monitor for cancellation.
         *
         * @return
         *   A generator used to emit messages during the process.
         */

        virtual
        std::generator<importer_run_message>
        run(
            importer_instance& instance,
            out_param<session_snapshot>& result,
            std::stop_token stop_token
        ) = 0;

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
    class importer_editor_ops: private nonmovable {

    public:

        /**
         * Destructor.
         */

        virtual
        ~importer_editor_ops() = default;

        /**
         * Populates the given window with an editor interface, allowing the
         * user to edit the importer state, and manages the window throughout
         * the lifetime of the edit operations (e.g. until the window is
         * closed).
         *
         * @param instance
         *   The importer instance to be edited.
         * @param parent
         *   The item to use as the parent of the edit interface.
         * @param stop_token
         *   A stop token that will be set if the operation is cancelled.
         *
         * @return
         *   A generator that is used to send messages back to the host.
         */

        virtual
        std::generator<importer_edit_message>
        edit(
            importer_instance& instance,
            ::QQuickItem* parent,
            std::stop_token stop_token
        ) = 0;

    protected:

        /**
         * Default constructor.
         */

        explicit
        importer_editor_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::importer_state_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains importer state operations.
     */

    export
    class importer_state_ops: private nonmovable {

    public:

        /**
         * Destructor.
         */

        virtual
        ~importer_state_ops() = default;

        /**
         * Gets a snapshot of the given importer's state.
         *
         * @param instance
         *   The importer instance to get the state for.
         *
         * @return
         *   The importer state.
         */

        virtual
        state_value
        dump(const importer_instance& instance) = 0;

        /**
         * Loads an importer from the given state.
         *
         * @param host
         *   The application host.
         * @param version
         *   The version of the importer type that was used to save the given
         *   state.
         * @param state
         *   The state to load the importer from.
         *
         * @return
         *   A pointer to the loaded importer instance.
         */

        virtual
        std::unique_ptr<importer_instance>
        load(
            app_host& host,
            const metadata_element& version,
            const state_value& state
        ) = 0;

    protected:

        /**
         * Default constructor.
         */

        explicit
        importer_state_ops() = default;

    };

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
                std::move(args.core_ops), std::move(args.editor_ops),
                std::move(args.state_ops), std::move(args.metadata))
        {
            // empty
        }

    };

}
