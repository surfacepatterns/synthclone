/**
 * @file
 *
 * Contains messages used by all components.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:component;

import std;

import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_state_changed_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message type indicating the editor changed a component's state.
     */

    export
    struct component_state_changed_message final {};

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_progress_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    float
    verify_component_progress(float n)
    {
        verify(
            (n >= 0.0) && (n <= 1.0), "{0}: invalid component progress value",
            n);
        return n;
    }

    /**
     * Message indicating the current amount of progress performing an
     * operation.
     */

    export
    class component_progress_message final {

    public:

        /**
         * Constructs a `component_progress_message` event.
         *
         * @param progress
         *   The progress amount, which should be in the range [0.0, 1.0].
         */

        constexpr explicit
        component_progress_message(float progress):
            progress_(verify_component_progress(progress))
        {
            // empty
        }

        /**
         * Gets the progress level.
         *
         * @return
         *   The progress level.
         */

        constexpr
        float
        progress()
        const noexcept
        {
            return progress_;
        }

    private:

        float progress_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_status_string
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * String type that holds the current status of a component operation.
     */

    export
    using component_status_string = utf8_line;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_status_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message indicating the current status of an operation.
     */

    export
    class component_status_message final {

    public:

        /**
         * Constructs a `component_status_message` event.
         *
         * @param status
         *   The operation status.
         */

        constexpr explicit
        component_status_message(component_status_string status)
        noexcept:
            status_(std::move(status))
        {
            // empty
        }

        /**
         * Gets the status string.
         *
         * @return
         *   The status string.
         */

        constexpr
        const component_status_string&
        status()
        const noexcept
        {
            return status_;
        }

    private:

        component_status_string status_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_event_wait_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message type indicating the host should wait for events before resuming
     * the `edit()` coroutine.
     */

    export
    struct component_event_wait_message final {};

}
