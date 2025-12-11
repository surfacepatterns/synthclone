#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;
import synthclone.util;

class QQuickItem;

namespace {

    class test_core_ops final: public synthclone::capture_effect_core_ops {

    public:

        std::unique_ptr<synthclone::capture_effect_instance>
        create()
        override final
        {
            return nullptr;
        }

        std::generator<synthclone::capture_effect_run_message>
        run(
            synthclone::capture_effect_instance& instance,
            synthclone::audio_input_stream& input_stream,
            synthclone::audio_output_stream& output_stream,
            std::stop_token stop_token
        )
        override final
        {
            co_return;
        }

    };

    class test_editor_ops final: public synthclone::capture_effect_editor_ops {

    public:

        std::generator<synthclone::capture_effect_edit_message>
        edit(
            synthclone::capture_effect_instance& instance,
            ::QQuickItem* parent,
            std::stop_token stop_token
        )
        override final
        {
            co_return;
        }

    };

    class test_instance final: public synthclone::capture_effect_instance {};

    class test_state_ops final: public synthclone::capture_effect_state_ops {

    public:

        synthclone::state_value
        dump(const synthclone::capture_effect_instance& instance)
        override final
        {
            return synthclone::state_value();
        }

        std::unique_ptr<synthclone::capture_effect_instance>
        load(const synthclone::state_value& value)
        override final
        {
            return nullptr;
        }

    };

    void
    verify_type(
        const synthclone::capture_effect_type& type,
        const synthclone::capture_effect_core_ops* core_ops_ptr,
        const synthclone::capture_effect_editor_ops* editor_ops_ptr,
        const synthclone::capture_effect_state_ops* state_ops_ptr,
        const synthclone::metadata& metadata
    )
    {
        synthclone::verify_eq(core_ops_ptr, type.core_ops().get());
        synthclone::verify_eq(editor_ops_ptr, type.editor_ops().get());
        synthclone::verify_eq(state_ops_ptr, type.state_ops().get());
        synthclone::verify_eq(metadata, type.metadata());
    }

}

BOOST_AUTO_TEST_SUITE(effect)

BOOST_AUTO_TEST_CASE(instance)
{
    std::unique_ptr<synthclone::capture_effect_instance> instance(
        std::make_unique<test_instance>());
    instance.reset();
}

BOOST_AUTO_TEST_CASE(types)
{
    auto core_ops_1 = std::make_unique<test_core_ops>();
    const auto* core_ops_1_ptr = core_ops_1.get();
    synthclone::metadata_init_args metadata_1(
        {
            .identifier = "foo",
            .version = "1.2.3"
        });
    synthclone::capture_effect_type type_1(
        {
            .core_ops = std::move(core_ops_1),
            .metadata = metadata_1
        });
    verify_type(type_1, core_ops_1_ptr, nullptr, nullptr, metadata_1);

    auto core_ops_2 = std::make_unique<test_core_ops>();
    const auto* core_ops_2_ptr = core_ops_2.get();
    auto editor_ops_2 = std::make_unique<test_editor_ops>();
    const auto* editor_ops_2_ptr = editor_ops_2.get();
    auto state_ops_2 = std::make_unique<test_state_ops>();
    const auto* state_ops_2_ptr = state_ops_2.get();

    synthclone::metadata_init_args metadata_2(
        {
            .identifier = "bar",
            .version = "4.5.6"
        });
    synthclone::capture_effect_type type_2(
        {
            .core_ops = std::move(core_ops_2),
            .editor_ops = std::move(editor_ops_2),
            .state_ops = std::move(state_ops_2),
            .metadata = metadata_2
        });
    verify_type(
        type_2, core_ops_2_ptr, editor_ops_2_ptr, state_ops_2_ptr, metadata_2);

    synthclone::capture_effect_type type_3 = std::move(type_1);
    verify_type(type_3, core_ops_1_ptr, nullptr, nullptr, metadata_1);

    type_1 = std::move(type_2);
    verify_type(
        type_1, core_ops_2_ptr, editor_ops_2_ptr, state_ops_2_ptr, metadata_2);

    BOOST_CHECK_THROW(
        synthclone::capture_effect_type(
            {
                .core_ops = nullptr,
                .metadata = metadata_1
            }),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_SUITE_END()
