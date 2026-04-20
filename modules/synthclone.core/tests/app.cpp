#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.core.test;
import synthclone.test;
import synthclone.util;

namespace {

    class minimal_log final: public synthclone::app_log {

    public:

        explicit
        minimal_log() = default;

        void
        emit(
            synthclone::app_log_level log_level,
            const std::string& s,
            const std::source_location& location
        ) override final
        {
            // empty
        }

        synthclone::app_log_level
        log_level_threshold() const noexcept override final
        {
            return synthclone::app_log_level::info;
        }

    };

    class test_log final: public synthclone::app_log {

    public:

        explicit
        test_log(
            synthclone::app_log_level expected_level,
            std::string expected_string,
            std::source_location expected_location,
            std::uint_least32_t expected_line_offset,
            bool& message_read
        ):
            synthclone::app_log(),
            expected_string_(std::move(expected_string)),
            expected_location_(std::move(expected_location)),
            expected_line_offset_(expected_line_offset),
            expected_level_(expected_level),
            message_read_(message_read)
        {
            // empty
        }

        void
        emit(
            synthclone::app_log_level log_level,
            const std::string& s,
            const std::source_location& location
        ) override final
        {
            BOOST_TEST_INFO_SCOPE(
                synthclone::make_test_info("test_log::emit", s, location));

            BOOST_CHECK(! message_read_);

            message_read_ = true;

            synthclone::verify_eq(expected_level_, log_level);
            synthclone::verify_eq(expected_string_, s);

            synthclone::verify_eq(
                expected_location_.line(),
                location.line() - expected_line_offset_);
            synthclone::verify_eq(
                expected_location_.function_name(), location.function_name());
            synthclone::verify_eq(
                expected_location_.file_name(), location.file_name());
        }

        synthclone::app_log_level
        log_level_threshold() const noexcept override final
        {
            return expected_level_;
        }

        std::string expected_string_;
        std::source_location expected_location_;
        std::uint_least32_t expected_line_offset_;
        synthclone::app_log_level expected_level_;
        bool& message_read_;

    };

}

BOOST_AUTO_TEST_SUITE(app)

BOOST_AUTO_TEST_CASE(basic_versions)
{
    synthclone::app_version current_version;
    synthclone::verify_eq(
        synthclone::project_version_major, current_version.major());
    synthclone::verify_eq(
        synthclone::project_version_minor, current_version.minor());
    synthclone::verify_eq(
        synthclone::project_version_patch, current_version.patch());

    synthclone::app_version future_version(10, 11, 12);
    synthclone::verify_eq(10, future_version.major());
    synthclone::verify_eq(11, future_version.minor());
    synthclone::verify_eq(12, future_version.patch());

    synthclone::app_version copied_version(current_version);
    synthclone::verify_eq(
        synthclone::project_version_major, copied_version.major());
    synthclone::verify_eq(
        synthclone::project_version_minor, copied_version.minor());
    synthclone::verify_eq(
        synthclone::project_version_patch, copied_version.patch());

    copied_version = future_version;
    synthclone::verify_eq(10, copied_version.major());
    synthclone::verify_eq(11, copied_version.minor());
    synthclone::verify_eq(12, copied_version.patch());
}

BOOST_AUTO_TEST_CASE(host)
{
    minimal_log log;
    synthclone::app_logger logger(log);
    synthclone::app_host host(logger);

    synthclone::verify_eq(
        std::addressof(logger), std::addressof(host.logger()));

    auto version = host.version();
    synthclone::verify_eq(synthclone::project_version_major, version.major());
    synthclone::verify_eq(synthclone::project_version_minor, version.minor());
    synthclone::verify_eq(synthclone::project_version_patch, version.patch());
}

BOOST_AUTO_TEST_CASE(logger)
{
    // The placement of this code *actually* matters because we're measuring
    // `std::source_location` offsets.

    bool message_read = false;
    const auto location_1 = std::source_location::current();
    test_log log_1(
        synthclone::app_log_level::debug, "foo bar baz", location_1, 6,
        message_read);
    synthclone::app_logger logger_1(log_1);
    BOOST_CHECK(! message_read);
    logger_1.log(synthclone::app_log_level::debug, "foo {0} baz", "bar");
    BOOST_CHECK(message_read);

    message_read = false;
    const auto location_2 = std::source_location::current();
    test_log log_2(
        synthclone::app_log_level::error, "1 2 3 4", location_2, 10,
        message_read);
    synthclone::app_logger logger_2(log_2);

    logger_2.log(synthclone::app_log_level::debug, "foo {0} baz", "bar");
    logger_2.log(synthclone::app_log_level::info, "foo {0} baz", "bar");
    logger_2.log(synthclone::app_log_level::warning, "foo {0} baz", "bar");
    BOOST_CHECK(! message_read);
    logger_2.log(synthclone::app_log_level::error, "1 {0} 3 {1}", 2, 4);
    BOOST_CHECK(message_read);
}

BOOST_AUTO_TEST_CASE(version_comparisons)
{
    synthclone::app_version current_version;

    synthclone::verify_eq(
        current_version,
        synthclone::app_version(
            synthclone::project_version_major,
            synthclone::project_version_minor,
            synthclone::project_version_patch));

    synthclone::verify_lt(
        current_version,
        synthclone::app_version(
            synthclone::project_version_major + 1,
            synthclone::project_version_minor,
            synthclone::project_version_patch));
    synthclone::verify_lt(
        current_version,
        synthclone::app_version(
            synthclone::project_version_major,
            synthclone::project_version_minor + 1,
            synthclone::project_version_patch));
    synthclone::verify_lt(
        current_version,
        synthclone::app_version(
            synthclone::project_version_major,
            synthclone::project_version_minor,
            synthclone::project_version_patch + 1));

    synthclone::app_version future_version(
        synthclone::project_version_major + 1,
        synthclone::project_version_minor + 1,
        synthclone::project_version_patch + 1);

    synthclone::verify_gt(
        future_version,
        synthclone::app_version(
            synthclone::project_version_major,
            synthclone::project_version_minor + 1,
            synthclone::project_version_patch + 1));
    synthclone::verify_gt(
        future_version,
        synthclone::app_version(
            synthclone::project_version_major + 1,
            synthclone::project_version_minor,
            synthclone::project_version_patch + 1));
    synthclone::verify_gt(
        future_version,
        synthclone::app_version(
            synthclone::project_version_major + 1,
            synthclone::project_version_minor + 1,
            synthclone::project_version_patch));
}

BOOST_AUTO_TEST_SUITE_END()
