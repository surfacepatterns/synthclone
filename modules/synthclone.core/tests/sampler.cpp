#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;
import synthclone.util;

class QQuickItem;

namespace {

    class test_core_ops final: public synthclone::sampler_core_ops {

    public:

        std::unique_ptr<synthclone::sampler_instance>
        create(
            synthclone::app_host& host,
            const synthclone::session_info& info
        ) override final
        {
            return nullptr;
        }

        std::generator<synthclone::sampler_capture_message>
        capture(
            synthclone::sampler_instance& instance,
            const synthclone::sampler_capture_params& capture_params,
            const synthclone::audio_sink& sink,
            std::stop_token stop_token
        ) override final
        {
            co_return;
        }

        std::generator<synthclone::sampler_play_message>
        play(
            synthclone::sampler_instance& instance,
            const synthclone::audio_source& source,
            std::stop_token stop_token
        ) override final
        {
            co_return;
        }

    };

    class test_editor_ops final: public synthclone::sampler_editor_ops {

    public:

        std::generator<synthclone::sampler_edit_message>
        edit(
            synthclone::sampler_instance& instance,
            ::QQuickItem* parent,
            std::stop_token stop_token
        ) override final
        {
            co_return;
        }

    };

    class test_instance final: public synthclone::sampler_instance {};

    class test_state_ops final: public synthclone::sampler_state_ops {

    public:

        synthclone::state_value
        dump(const synthclone::sampler_instance& instance) override final
        {
            return synthclone::state_value();
        }

        std::unique_ptr<synthclone::sampler_instance>
        load(
            synthclone::app_host& host,
            const synthclone::session_info& info,
            const synthclone::metadata_element& version,
            const synthclone::state_value& value
        ) override final
        {
            return nullptr;
        }

    };

    void
    verify_capture_params(
        const synthclone::sampler_capture_params& params,
        synthclone::midi_channel expected_channel,
        synthclone::midi_note expected_note,
        synthclone::midi_velocity expected_velocity,
        std::optional<synthclone::midi_aftertouch> expected_aftertouch,
        std::optional<synthclone::midi_channel_pressure> expected_pressure,
        const synthclone::midi_control_map& expected_controls,
        synthclone::audio_duration expected_sample_duration,
        synthclone::audio_duration expected_release_duration
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_capture_params", params, expected_channel,
                expected_note, expected_velocity, expected_aftertouch,
                expected_pressure, expected_controls, expected_sample_duration,
                expected_release_duration));

        synthclone::verify_eq(expected_channel, params.channel());
        synthclone::verify_eq(expected_note, params.note());
        synthclone::verify_eq(expected_velocity, params.velocity());
        synthclone::verify_eq(expected_aftertouch, params.aftertouch());
        synthclone::verify_eq(expected_pressure, params.channel_pressure());
        synthclone::verify_eq(expected_controls, params.controls());
        synthclone::verify_eq(
            expected_sample_duration, params.sample_duration());
        synthclone::verify_eq(
            expected_release_duration, params.release_duration());
    }

    void
    verify_capture_params(
        const synthclone::sampler_capture_params& expected_params,
        const synthclone::sampler_capture_params& actual_params
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_capture_params", expected_params, actual_params));

        verify_capture_params(
            actual_params, expected_params.channel(), expected_params.note(),
            expected_params.velocity(), expected_params.aftertouch(),
            expected_params.channel_pressure(), expected_params.controls(),
            expected_params.sample_duration(),
            expected_params.release_duration());
    }

    void
    verify_type(
        const synthclone::sampler_type& type,
        const synthclone::sampler_core_ops* expected_core_ops_ptr,
        const synthclone::sampler_editor_ops* expected_editor_ops_ptr,
        const synthclone::sampler_state_ops* expected_state_ops_ptr,
        const synthclone::component_metadata& expected_metadata
    )
    {
        synthclone::verify_eq(expected_core_ops_ptr, type.core_ops().get());
        synthclone::verify_eq(
            expected_editor_ops_ptr, type.editor_ops().get());
        synthclone::verify_eq(expected_state_ops_ptr, type.state_ops().get());
        synthclone::verify_eq(expected_metadata, type.metadata());
    }

}

BOOST_AUTO_TEST_SUITE(sampler)

BOOST_AUTO_TEST_CASE(capture_params)
{
    synthclone::sampler_capture_params params_1(
        {
            .channel = 1,
            .note = 40,
            .velocity = 100,
            .sample_duration = 1,
            .release_duration = 2
        });

    verify_capture_params(
        params_1, 1, 40, 100, std::nullopt, std::nullopt, {}, 1, 2);

    synthclone::midi_control_map controls {
        {1, 2},
        {3, 4}
    };
    synthclone::sampler_capture_params params_2(
        {
            .channel = 10,
            .note = 80,
            .velocity = 127,
            .aftertouch = 72,
            .channel_pressure = 84,
            .controls = controls,
            .sample_duration = 4,
            .release_duration = 8
        });
    verify_capture_params(params_2, 10, 80, 127, 72, 84, controls, 4, 8);

    synthclone::sampler_capture_params params_3(params_1);

    verify_capture_params(params_1, params_3);

    params_1 = params_2;

    verify_capture_params(params_2, params_1);
}

BOOST_AUTO_TEST_CASE(instance)
{
    std::unique_ptr<synthclone::sampler_instance> instance(
        std::make_unique<test_instance>());
    instance.reset();
}

BOOST_AUTO_TEST_CASE(types)
{
    auto core_ops_1 = std::make_unique<test_core_ops>();
    const auto* core_ops_1_ptr = core_ops_1.get();
    synthclone::component_metadata_init_args metadata_1(
        {
            .identifier = "foo",
            .version = "1.2.3"
        });
    synthclone::sampler_type type_1(
        {
            .core_ops = std::move(core_ops_1),
            .metadata = metadata_1
        });
    verify_type(type_1, core_ops_1_ptr, nullptr, nullptr, metadata_1);

    synthclone::session_info info(
        synthclone::audio_traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_f32,
            synthclone::audio_endianness::little, 48000, 2));
    BOOST_REQUIRE(core_ops_1_ptr->is_supported(info));

    std::unique_ptr<synthclone::sampler_instance> instance(
        std::make_unique<test_instance>());
    core_ops_1->activate(*instance);
    core_ops_1->deactivate(*instance);

    auto core_ops_2 = std::make_unique<test_core_ops>();
    const auto* core_ops_2_ptr = core_ops_2.get();
    auto editor_ops_2 = std::make_unique<test_editor_ops>();
    const auto* editor_ops_2_ptr = editor_ops_2.get();
    auto state_ops_2 = std::make_unique<test_state_ops>();
    const auto* state_ops_2_ptr = state_ops_2.get();

    synthclone::component_metadata_init_args metadata_2(
        {
            .identifier = "bar",
            .version = "4.5.6"
        });
    synthclone::sampler_type type_2(
        {
            .core_ops = std::move(core_ops_2),
            .editor_ops = std::move(editor_ops_2),
            .state_ops = std::move(state_ops_2),
            .metadata = metadata_2
        });
    verify_type(
        type_2, core_ops_2_ptr, editor_ops_2_ptr, state_ops_2_ptr, metadata_2);

    synthclone::sampler_type type_3 = std::move(type_1);
    verify_type(type_3, core_ops_1_ptr, nullptr, nullptr, metadata_1);

    type_1 = std::move(type_2);
    verify_type(
        type_1, core_ops_2_ptr, editor_ops_2_ptr, state_ops_2_ptr, metadata_2);

    BOOST_CHECK_THROW(
        synthclone::sampler_type(
            {
                .core_ops = nullptr,
                .metadata = metadata_1
            }),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_SUITE_END()
