export module synthclone.test.plugin:session;

import std;

import synthclone.core;

///////////////////////////////////////////////////////////////////////////////
// synthclone::test_session_log
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    struct test_session_log final: public session_log {

        void
        emit(
            session_log_level level,
            const std::string& message,
            const std::source_location& location
        )
        override final
        {
            // XXX: We ignore the log level.
            std::println(
                std::cerr, "{0}:{1}:{2} - {3}", location.file_name(),
                location.line(), location.column(), message);
            std::cerr.flush();
        }

    };

}
