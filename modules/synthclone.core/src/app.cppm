/**
 * @file
 *
 * Specifies aspects of the `synthclone` application that are available to
 * plugins.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:app;

import std;

import synthclone.util;

import :project;

///////////////////////////////////////////////////////////////////////////////
// synthclone::app_log_level
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Valid log levels.
     */

    export
    enum class app_log_level {

        /**
         * Log level for a log entry that may be helpful for debugging.
         */

        debug = 0,

        /**
         * Log level for a log entry that holds a point of interest.
         */

        info = 1,

        /**
         * Log level for a log entry that indicates an issue, but one that is
         * non-critical/recoverable.
         */

        warning = 2,

        /**
         * Log level for a log entry that indicates an error.
         */

        error = 3

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::app_log
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Application-specific log destination.
     */

    export
    class app_log: private nonmovable {

    public:

        /**
         * Emits a log entry to the application log.
         *
         * @param level
         *   The log level.
         * @param message
         *   The log message.
         * @param location
         *   The source location where the log message was crafted.
         */

        virtual
        void
        emit(
            app_log_level level,
            const std::string& message,
            const std::source_location& location
        ) = 0;

        /**
         * Gets the log level threshold.
         *
         * @return
         *   The log level threshold.
         */

        virtual
        app_log_level
        log_level_threshold() const noexcept = 0;

    protected:

        /**
         * Default constructor.
         */

        explicit
        app_log() = default;

        /**
         * Destructor.
         */

        ~app_log() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::app_logger
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Allows plugins to write log entries to an application logger.
     */

    export
    class app_logger final: private nonmovable {

    public:

        /**
         * Constructs a `app_logger` instance.
         *
         * @param log
         *   The application log.
         */

        explicit
        app_logger(app_log& log):
            log_(log)
        {
            // empty
        }

        /**
         * Writes a log entry to the application log if the given log level is
         * greater than or equal to the application log level threshold.
         *
         * @param level
         *   The log level.
         * @param s
         *   A string that will be passed as the `std::format_string` to
         *   `std::format` to get the final log message.
         * @param args
         *   The arguments to use to populate the format string.
         */

        template<class... Args>
        void
        log(
            app_log_level level,
            diagnostic_format_string<std::type_identity_t<Args>...> s,
            Args&&... args
        )
        {
            if (level >= log_.log_level_threshold()) {
                log_.emit(
                    level,
                    std::format(s.string(), std::forward<Args>(args)...),
                    s.location());
            }
        }

    private:

        app_log& log_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::app_version
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains application version information.
     */

    export
    class app_version final {

    public:

        /**
         * Constructs an `app_version` instance with the current major, minor,
         * and patch values.
         */

        constexpr
        app_version() noexcept:
            app_version(
                project_version_major, project_version_minor,
                project_version_patch)
        {
            // empty
        }

        /**
         * Constructs an `app_version` instance with the given major, minor,
         * and patch values.
         *
         * @param major
         *   The major portion of the version number.
         * @param minor
         *   The minor portion of the version number.
         * @param patch
         *   The patch portion of the version number.
         */

        constexpr
        app_version(
            std::uint_least32_t major,
            std::uint_least32_t minor,
            std::uint_least32_t patch
        ) noexcept:
            major_(major),
            minor_(minor),
            patch_(patch)
        {
            // empty
        }

        /**
         * Gets the major portion of the version number.
         *
         * @return
         *   The major portion.
         */

        constexpr
        std::uint_least32_t
        major() const noexcept
        {
            return major_;
        }

        /**
         * Gets the minor portion of the version number.
         *
         * @return
         *   The minor portion.
         */

        constexpr
        std::uint_least32_t
        minor() const noexcept
        {
            return minor_;
        }

        /**
         * Gets the patch portion of the version number.
         *
         * @return
         *   The patch portion.
         */

        constexpr
        std::uint_least32_t
        patch() const noexcept
        {
            return patch_;
        }

    private:

        std::uint_least32_t major_;
        std::uint_least32_t minor_;
        std::uint_least32_t patch_;

    };

    /**
     * Gets a boolean indicating if the two given versions are equal.
     *
     * @param lhs
     *   The first version.
     * @param rhs
     *   The second version.
     *
     * @return
     *   The boolean indicator.
     */

    export
    constexpr
    bool
    operator==(const app_version& lhs, const app_version& rhs) noexcept
    {
        return (lhs.major() == rhs.major()) && (lhs.minor() == rhs.minor()) &&
            (lhs.patch() == rhs.patch());
    }

    /**
     * Does a rich comparison of two versions.
     *
     * @param lhs
     *   The first version.
     * @param rhs
     *   The second version.
     *
     * @return
     *   The compare result.
     */

    export
    constexpr
    auto
    operator<=>(const app_version& lhs, const app_version& rhs) noexcept
    {
        auto result = lhs.major() <=> rhs.major();
        if (std::is_eq(result)) {
            result = lhs.minor() <=> rhs.minor();
            if (std::is_eq(result)) {
                result = lhs.patch() <=> rhs.patch();
            }
        }
        return result;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::app_host
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains information about the application and mechanisms for limited
     * application functionality exposed to a plugin.
     */

    export
    class app_host final: private nonmovable {

    public:

        /**
         * Constructs an `app_host` instance.
         *
         * @param logger
         *   A logger that a plugin can use to write log entries to the
         *   application log.
         */

        explicit
        app_host(app_logger& logger) noexcept:
            logger_(logger)
        {
            // empty
        }

        /**
         * Gets the application logger.
         *
         * @return
         *   The logger.
         */

        constexpr
        app_logger&
        logger() noexcept
        {
            return logger_;
        }

        /**
         * Gets the application version.
         *
         * @return
         *   The application version.
         */

        constexpr
        const app_version&
        version() const noexcept
        {
            return version_;
        }

    private:

        app_logger& logger_;
        app_version version_;

    };

}
