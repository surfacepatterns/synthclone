module;

#include <boost/test/unit_test.hpp>

export module synthclone.test.plugin:capture_effect;

import std;

import synthclone.core;
import synthclone.test;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_capture_effect_run()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    class capture_effect_run_message_tracker {

    public:

        explicit
        capture_effect_run_message_tracker(
            std::size_t expected_state_change_count = 0
        ):
            progress_(0.0),
            state_changes_left_(expected_state_change_count),
            status_("")
        {
            // empty
        }

        void
        operator()(const component_progress_message& message)
        {
            BOOST_TEST_INFO_SCOPE(
                make_test_info(
                    "synthclone::capture_effect_run_message_tracker::"
                    "operator()",
                    message));

            auto updated_progress = message.progress();
            verify_le(progress_, updated_progress);
            progress_ = updated_progress;
        }

        void
        operator()(const component_state_changed_message& message)
        {
            BOOST_TEST_INFO_SCOPE(
                make_test_info(
                    "synthclone::capture_effect_run_message_tracker::"
                    "operator()",
                    message));

            verify_gt(state_changes_left_, 0);
            --state_changes_left_;
        }

        void
        operator()(const component_status_message& message)
        {
            status_ = message.status();
        }

        constexpr
        float
        progress()
        const noexcept
        {
            return progress_;
        }

        constexpr
        std::size_t
        state_changes_left()
        const noexcept
        {
            return state_changes_left_;
        }

        constexpr
        const utf8_line&
        status()
        const noexcept
        {
            return status_;
        }

    private:

        float progress_;
        std::size_t state_changes_left_;
        utf8_line status_;

    };

    export
    template<std::ranges::contiguous_range R1, std::ranges::sized_range R2>
    requires (
        std::same_as<std::ranges::range_value_t<R1>, audio_sample> &&
        std::same_as<std::ranges::range_value_t<R2>, audio_sample>
    )
    void
    verify_basic_capture_effect_run(
        synthclone::session_host& host,
        synthclone::capture_effect_type& type,
        synthclone::capture_effect_instance& instance,
        R1&& input_samples,
        R2&& expected_output_samples,
        std::size_t expected_state_change_count = 0
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_basic_capture_effect_run", host, type,
                instance, input_samples, expected_output_samples));

        const auto& audio_traits = host.audio_traits();

        synthclone::temporary_file input_file;
        const auto& input_file_path = input_file.path();

        {
            synthclone::audio_output_stream stream(
                input_file_path, audio_traits);
            if (! input_samples.empty()) {
                stream.write(input_samples);
            }
        }

        synthclone::temporary_file output_file;
        const auto& output_file_path = output_file.path();

        {
            synthclone::audio_input_stream input_stream(
                input_file_path, audio_traits);
            synthclone::audio_output_stream output_stream(
                output_file_path, audio_traits);
            std::stop_source stop_source;

            capture_effect_run_message_tracker tracker(
                expected_state_change_count);

            auto gen = type.core_ops()->run(
                instance, input_stream, output_stream,
                stop_source.get_token());
            for (auto message: gen) {
                std::visit(tracker, message);
            }

            verify_eq(1.0, tracker.progress());
            verify_eq(0, tracker.state_changes_left());
        }

        dynamic_buffer<audio_sample> output_samples(
            expected_output_samples.size());
        synthclone::audio_input_stream stream(
            output_file_path, audio_traits);
        if (! std::empty(expected_output_samples)) {
            auto frame_count = stream.read(output_samples);
            verify_eq(
                output_samples.size() / audio_traits.channel_count().value(),
                frame_count);
        }

        auto position = stream.tell();
        stream.seek(0, audio_seek_origin::end);
        verify_eq(position, stream.tell());

        BOOST_CHECK_EQUAL_COLLECTIONS(
            expected_output_samples.begin(), expected_output_samples.end(),
            output_samples.begin(), output_samples.end());
    }

}
