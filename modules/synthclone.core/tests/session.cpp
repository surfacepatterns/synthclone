#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.core.test;
import synthclone.test;
import synthclone.util;

namespace {

    class host_log final: public synthclone::session_log {

    public:

        explicit
        host_log() = default;

        void
        emit(
            synthclone::session_log_level log_level,
            const std::string& s,
            const std::source_location& location
        )
        override final
        {
            // empty
        }

    };

    class test_log final: public synthclone::session_log {

    public:

        explicit
        test_log(
            synthclone::session_log_level expected_level,
            std::string expected_string,
            std::source_location expected_location,
            std::uint_least32_t expected_line_offset,
            bool& message_read
        ):
            synthclone::session_log(),
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
            synthclone::session_log_level log_level,
            const std::string& s,
            const std::source_location& location
        )
        override final
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

        std::string expected_string_;
        std::source_location expected_location_;
        std::uint_least32_t expected_line_offset_;
        synthclone::session_log_level expected_level_;
        bool& message_read_;

    };

}

BOOST_AUTO_TEST_SUITE(session)

BOOST_AUTO_TEST_CASE(host)
{
    host_log log;
    synthclone::session_logger logger(
        log, synthclone::session_log_level::debug);
    synthclone::session_host host(logger, 48000, 2);

    synthclone::verify_eq(
        std::addressof(logger), std::addressof(host.logger()));
    synthclone::verify_audio_traits(
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_f32,
            synthclone::audio_endianness::little, 48000, 2),
        host.audio_traits());
}

BOOST_AUTO_TEST_CASE(logger)
{
    // The placement of this code *actually* matters because we're measuring
    // `std::source_location` offsets.

    bool message_read = false;
    const auto location_1 = std::source_location::current();
    test_log log_1(
        synthclone::session_log_level::debug, "foo bar baz", location_1, 7,
        message_read);
    synthclone::session_logger logger_1(
        log_1, synthclone::session_log_level::debug);
    BOOST_CHECK(! message_read);
    logger_1.log(synthclone::session_log_level::debug, "foo {0} baz", "bar");
    BOOST_CHECK(message_read);

    message_read = false;
    const auto location_2 = std::source_location::current();
    test_log log_2(
        synthclone::session_log_level::error, "1 2 3 4", location_2, 10,
        message_read);
    synthclone::session_logger logger_2(
        log_2, synthclone::session_log_level::error);
    logger_2.log(synthclone::session_log_level::debug, "foo {0} baz", "bar");
    logger_2.log(synthclone::session_log_level::info, "foo {0} baz", "bar");
    logger_2.log(synthclone::session_log_level::warning, "foo {0} baz", "bar");
    BOOST_CHECK(! message_read);
    logger_2.log(synthclone::session_log_level::error, "1 {0} 3 {1}", 2, 4);
    BOOST_CHECK(message_read);
}

BOOST_AUTO_TEST_SUITE_END()
