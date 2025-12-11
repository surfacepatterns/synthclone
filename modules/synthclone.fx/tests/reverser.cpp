#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.external.boost.interprocess;
import synthclone.fx;
import synthclone.qt;
import synthclone.test;
import synthclone.test.plugin;
import synthclone.util;

namespace {

    std::size_t
    get_default_buffer_size()
    {
        return boost::interprocess::mapped_region::get_page_size();
    }

    std::unique_ptr<synthclone::capture_effect_instance>
    load_reverser_instance(
        synthclone::capture_effect_type& type,
        std::size_t buffer_size
    )
    {
        return type.state_ops()->load(
            synthclone::state_map { { "buffer-size", buffer_size } });
    }

    synthclone::capture_effect_type
    load_reverser_type(synthclone::session_host& host)
    {
        auto instance = synthclone::make_fx_plugin()->instantiate(host);
        return synthclone::load_capture_effect_type(
            instance, "synthclone.fx.reverser");
    }

    void
    verify_buffer_size(
        synthclone::capture_effect_type& type,
        synthclone::capture_effect_instance& instance,
        std::size_t expected_buffer_size
    )
    {
        auto state_root = type.state_ops()->dump(instance);
        const auto& state_map = synthclone::extract_map(state_root);
        synthclone::verify_eq(1, state_map.size());

        auto iter = state_map.find("buffer-size");
        synthclone::verify_ne(iter, state_map.end());

        synthclone::verify_eq(
            expected_buffer_size,
            synthclone::extract_number<std::size_t>(iter->second));
    }

    void
    verify_default_buffer_size(
        synthclone::capture_effect_type& type,
        synthclone::capture_effect_instance& instance
    )
    {
        verify_buffer_size(type, instance, get_default_buffer_size());
    }

}

BOOST_AUTO_TEST_SUITE(reverser)

BOOST_AUTO_TEST_CASE(core_ops)
{
    {
        synthclone::test_session_log log;
        synthclone::session_logger logger(
            log, synthclone::session_log_level::debug);
        synthclone::session_host host(logger, 48000, 1);

        std::array<synthclone::audio_sample, 96000> in_samples;
        for (std::size_t i = 0; i < 96000; ++i) {
            in_samples[i] = static_cast<synthclone::audio_sample>(i) / 96000.1;
        }

        auto type = load_reverser_type(host);

        auto instance_ptr = type.core_ops()->create();
        synthclone::verify_basic_capture_effect_run(
            host, type, *instance_ptr, in_samples,
            std::ranges::reverse_view(in_samples));

        instance_ptr = load_reverser_instance(type, 0);
        synthclone::verify_basic_capture_effect_run(
            host, type, *instance_ptr, in_samples,
            std::ranges::reverse_view(in_samples), 1);
    }

    {
        synthclone::test_session_log log;
        synthclone::session_logger logger(
            log, synthclone::session_log_level::debug);
        synthclone::session_host host(logger, 48000, 2);

        std::array<synthclone::audio_sample, 96000> in_samples;
        std::array<synthclone::audio_sample, 96000> expected_out_samples;
        for (std::size_t i = 0; i < 48000; ++i) {
            auto n1 = static_cast<synthclone::audio_sample>(i) / 48000.1;
            auto n2 = n1 + 0.00001;

            auto in_index = i * 2;
            in_samples[in_index] = n1;
            in_samples[in_index + 1] = n2;

            auto out_index = 95998 - in_index;
            expected_out_samples[out_index] = n1;
            expected_out_samples[out_index + 1] = n2;
        }

        auto type = load_reverser_type(host);

        auto instance_ptr = type.core_ops()->create();
        synthclone::verify_basic_capture_effect_run(
            host, type, *instance_ptr, in_samples, expected_out_samples);

        instance_ptr = load_reverser_instance(type, 1);
        synthclone::verify_basic_capture_effect_run(
            host, type, *instance_ptr, in_samples, expected_out_samples, 1);
    }

    {
        synthclone::test_session_log log;
        synthclone::session_logger logger(
            log, synthclone::session_log_level::debug);
        synthclone::session_host host(logger, 48000, 1);

        auto type = load_reverser_type(host);

        auto instance_ptr = type.core_ops()->create();
        synthclone::verify_basic_capture_effect_run(
            host, type, *instance_ptr,
            std::ranges::empty_view<synthclone::audio_sample>(),
            std::ranges::empty_view<synthclone::audio_sample>());
    }
}

BOOST_AUTO_TEST_CASE(state_ops)
{
    synthclone::test_session_log log;
    synthclone::session_logger logger(
        log, synthclone::session_log_level::debug);
    synthclone::session_host host(logger, 48000, 1);

    auto type = load_reverser_type(host);
    auto instance_ptr = type.core_ops()->create();
    verify_default_buffer_size(type, *instance_ptr);

    instance_ptr = load_reverser_instance(type, 16384);
    verify_buffer_size(type, *instance_ptr, 16384);

    auto& state_ops_ptr = type.state_ops();

    BOOST_CHECK_THROW(state_ops_ptr->load("foo"), synthclone::state_error);
    BOOST_CHECK_THROW(
        state_ops_ptr->load(synthclone::state_map {}),
        synthclone::state_error);
    BOOST_CHECK_THROW(
        state_ops_ptr->load(synthclone::state_map { { "buffer-size", -4096 }}),
        synthclone::state_error);
}

BOOST_AUTO_TEST_SUITE_END()
