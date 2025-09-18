/**
 * @file
 *
 * Specifies aspects of `synthclone` sessions that are available to plugins.
 *
 * XXX: The logging functionality here may very well change.  It's a
 * placeholder for something potentially better.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:session;

import std;

import synthclone.util;

import :audio;

///////////////////////////////////////////////////////////////////////////////
// synthclone::session_log_level
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Valid log levels.
     */

    export
    enum class session_log_level: std::uint8_t {

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
// synthclone::session_log
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Application-specific log destination.
     */

    export
    class session_log: private nonmovable {

    public:

        /**
         * Emits a log entry to the session log.
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
            session_log_level level,
            const std::string& message,
            const std::source_location& location
        ) = 0;

    protected:

        /**
         * Default constructor.
         */

        explicit
        session_log() = default;

        /**
         * Destructor.
         */

        ~session_log() = default;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::session_logger
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    session_log_level
    verify_session_log_level(session_log_level level)
    {
        auto level_n = std::to_underlying(level);
        verify(
            std::to_underlying(level) <=
            std::to_underlying(session_log_level::error),
            "{0}: invalid log level value", level_n);
        return level;
    }

    /**
     * Allows plugins to write log entries to an application logger.
     */

    export
    class session_logger final: private nonmovable {

    public:

        /**
         * Constructs a `session_logger` instance.
         *
         * @param level_threshold
         *   The minimum log level for a given log entry to be emitted to the
         *   given session log.
         */

        explicit
        session_logger(session_log& log, session_log_level level_threshold):
            log_(log),
            level_threshold_(verify_session_log_level(level_threshold))
        {
            // empty
        }

        /**
         * Writes a log entry to the session log if the given log level is
         * greater than or equal to the log level threshold.
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
            session_log_level level,
            diagnostic_format_string<std::type_identity_t<Args>...> s,
            Args&&... args
        )
        {
            if (level >= level_threshold_) {
                log_.emit(
                    level,
                    std::format(s.string(), std::forward<Args>(args)...),
                    s.location());
            }
        }

    private:

        session_log& log_;
        session_log_level level_threshold_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::session_host
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains information about the session and mechanisms for limited
     * application functionality exposed to a plugin.
     */

    export
    class session_host final {

    public:

        /**
         * Constructs a `session_host` instance.
         *
         * @param logger
         *   A logger that a plugin can use to write log entries to the session
         *   log.
         * @param sample_rate
         *   The session sample rate.
         * @param channel_count
         *   The session channel count.
         */

        explicit
        session_host(
            session_logger& logger,
            audio_sample_rate sample_rate,
            audio_channel_count channel_count
        )
        noexcept:
            // XXX: Choose endianness based on native endianness.
            audio_traits_(
                audio_format::raw, audio_codec::pcm_f32,
                audio_endianness::little, sample_rate, channel_count),
            logger_(logger)
        {
            // empty
        }

        /**
         * Gets the audio traits used for samples stored in this session.
         *
         * @return
         *   The audio traits.
         */

        constexpr
        const audio_traits&
        audio_traits()
        const noexcept
        {
            return audio_traits_;
        }

        /**
         * Gets the session logger.
         *
         * @return
         *   The logger.
         */

        constexpr
        session_logger&
        logger()
        noexcept
        {
            return logger_;
        }

    private:

        class audio_traits audio_traits_;
        session_logger& logger_;

    };

}
