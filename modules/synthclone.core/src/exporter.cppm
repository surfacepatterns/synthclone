/**
 * @file
 *
 * Contains functionality that allows plugins to create their own exporter
 * types.
 */

module;

#include <synthclone/config.h>

// The forward declaration needs to be here so the declaration is not attached
// to the module.
class QQuickItem;

export module synthclone.core:exporter;

import std;

import synthclone.util;

import :app;
import :component_core;
import :operation;
import :snapshot;

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
        operation_idle_message,
        operation_state_changed_message,
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
        operation_progress_message,
        operation_state_changed_message,
        operation_status_message,
        operation_warning_message
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
    class exporter_core_ops: public nonmovable {

    public:

        /**
         * Destructs an `exporter_core_ops` instance.
         */

        virtual
        ~exporter_core_ops() = default;

        /**
         * Instantiates a new component instance.
         *
         * @param host
         *   The application host.
         * @param info
         *   Information about the current session.
         *
         * @return
         *   A pointer to the new component instance.
         */

        virtual
        std::unique_ptr<exporter_instance>
        create(app_host& host, const session_info& info) = 0;

        /**
         * Exports contained session data (typically stored during an ongoing
         * `exporter_edit_ops::edit()` operation).
         *
         * @param instance
         *   The exporter instance.
         * @param stop_token
         *   A token to monitor for cancellation.
         *
         * @return
         *   A generator used to emit messages during the process.
         */

        virtual
        std::generator<exporter_run_message>
        run(exporter_instance& instance, std::stop_token stop_token) = 0;

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
    class exporter_editor_ops: private nonmovable {

    public:

        /**
         * Destructor.
         */

        virtual
        ~exporter_editor_ops() = default;

        /**
         * Populates the given window with an editor interface, allowing the
         * user to edit the exporter state, and manages the window throughout
         * the lifetime of the edit operations (e.g. until the window is
         * closed).
         *
         * @param instance
         *   The exporter instance to be edited.
         * @param snapshot
         *   A snapshot of the current session.
         * @param parent
         *   The item to use as the parent of the edit interface.
         * @param stop_token
         *   A stop token that will be set if the operation is cancelled.
         *
         * @return
         *   A generator that is used to send messages back to the host.
         */

        virtual
        std::generator<exporter_edit_message>
        edit(
            exporter_instance& instance,
            session_snapshot& snapshot,
            ::QQuickItem* parent,
            std::stop_token stop_token
        ) = 0;

    protected:

        /**
         * Default constructor.
         */

        explicit
        exporter_editor_ops() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::exporter_state_ops
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains exporter state operations.
     */

    export
    class exporter_state_ops: private nonmovable {

    public:

        /**
         * Destructor.
         */

        virtual
        ~exporter_state_ops() = default;

        /**
         * Gets a snapshot of the given exporter's state.
         *
         * @param instance
         *   The exporter instance to get the state for.
         *
         * @return
         *   The exporter state.
         */

        virtual
        state_value
        dump(const exporter_instance& instance) = 0;

        /**
         * Loads an exporter from the given state.
         *
         * @param host
         *   The application host.
         * @param info
         *   Information about the current session.
         * @param version
         *   The version of the exporter type that was used to save the given
         *   state.
         * @param state
         *   The state to load the exporter from.
         *
         * @return
         *   A pointer to the loaded exporter instance.
         */

        virtual
        std::unique_ptr<exporter_instance>
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
        exporter_state_ops() = default;

    };

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
    class exporter_type final: public component_type<
        exporter_core_ops,
        exporter_editor_ops,
        exporter_state_ops,
        true
    > {

    public:

        /**
         * Constructs an `exporter_type` instance.
         *
         * @param args
         *   The data to use to populate the `exporter_type` instance.
         */

        constexpr
        exporter_type(exporter_type_init_args args):
            component_type(
                std::move(args.core_ops), std::move(args.editor_ops),
                std::move(args.state_ops), std::move(args.metadata))
        {
            // empty
        }

    };

}
