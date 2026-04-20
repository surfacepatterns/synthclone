/**
 * @file
 *
 * Contains functionality associated with long-running operations.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:operation;

import std;

import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::operation_idle_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message type indicating that an operation is idle, and the host should
     * wait for events before resuming the operation.
     */

    export
    struct operation_idle_message final {};

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::operation_progress_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    float
    verify_operation_progress(float n)
    {
        verify(
            (n >= 0.0f) && (n <= 1.0f),
            "{0}: invalid operation progress value", n);
        return n;
    }

    /**
     * Message indicating the current amount of progress performing an
     * operation.
     */

    export
    class operation_progress_message final {

    public:

        /**
         * Constructs an `operation_progress_message` instance.
         *
         * @param progress
         *   The progress amount, which should be in the range [0.0, 1.0].
         */

        constexpr explicit
        operation_progress_message(float progress):
            progress_(verify_operation_progress(progress))
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
        progress() const noexcept
        {
            return progress_;
        }

    private:

        float progress_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::operation_state_changed_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message type indicating an operation changed some state and that the
     * application may want to react to that change in some context-specific
     * fashion.
     */

    export
    struct operation_state_changed_message final {};

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::operation_string
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * String type used to communicate some aspect of an operation.
     */

    export
    using operation_string = utf8_line;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::operation_status_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message indicating the current status of an operation.
     */

    export
    class operation_status_message final {

    public:

        /**
         * Constructs an `operation_status_message` event.
         *
         * @param status
         *   The operation status string.
         */

        constexpr explicit
        operation_status_message(operation_string status) noexcept:
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
        const operation_string&
        status() const noexcept
        {
            return status_;
        }

    private:

        operation_string status_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::operation_warning_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Message indicating a *potential* problem, but not necessarily an
     * outright error.
     */

    export
    class operation_warning_message final {

    public:

        /**
         * Constructs an `operation_warning_message` event.
         *
         * @param warning
         *   The operation warning string.
         */

        constexpr explicit
        operation_warning_message(operation_string warning) noexcept:
            warning_(std::move(warning))
        {
            // empty
        }

        /**
         * Gets the warning string.
         *
         * @return
         *   The warning string.
         */

        constexpr
        const operation_string&
        warning() const noexcept
        {
            return warning_;
        }

    private:

        operation_string warning_;

    };

}
