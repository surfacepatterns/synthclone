export module synthclone.test.plugin:app;

import std;

import synthclone.core;

///////////////////////////////////////////////////////////////////////////////
// synthclone::test_app_log
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    class test_app_log final: public app_log {

    public:

        explicit
        test_app_log():
            app_log(),
            threshold_(app_log_level::debug)
        {
            // empty
        }

        void
        emit(
            app_log_level level,
            const std::string& message,
            const std::source_location& location
        ) override final
        {
            // XXX: We ignore the log level.
            std::println(
                std::cerr, "{0}:{1}:{2} - {3}", location.file_name(),
                location.line(), location.column(), message);
            std::cerr.flush();
        }

        app_log_level
        log_level_threshold() const noexcept override final
        {
            return threshold_;
        }

        void
        log_level_threshold(app_log_level threshold)
        {
            threshold_ = threshold;
        }

    private:

        app_log_level threshold_;

    };

}
