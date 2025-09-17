#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;
import synthclone.util;

class QWindow;

namespace {

    class test_core_ops final: public synthclone::effect_core_ops {

    public:

        std::unique_ptr<synthclone::effect_instance>
        create()
        override final
        {
            return nullptr;
        }

        std::generator<synthclone::effect_run_message>
        run(
            synthclone::effect_instance& instance,
            synthclone::audio_input_stream& input_stream,
            synthclone::audio_output_stream& output_stream,
            std::stop_token stop_token
        )
        override final
        {
            co_return;
        }

    };

    class test_editor_ops final: public synthclone::effect_editor_ops {

    public:

        std::generator<synthclone::effect_edit_message>
        edit(
            synthclone::effect_instance& instance,
            ::QWindow& window,
            std::stop_token stop_token
        )
        override final
        {
            co_return;
        }

    };

    class test_instance final: public synthclone::effect_instance {};

    class test_state_ops final: public synthclone::effect_state_ops {

    public:

        synthclone::state_value
        dump(const synthclone::effect_instance& instance)
        override final
        {
            return synthclone::state_value();
        }

        std::unique_ptr<synthclone::effect_instance>
        load(const synthclone::state_value& value)
        override final
        {
            return nullptr;
        }

    };

    void
    verify_type(
        const synthclone::effect_type& type,
        const synthclone::effect_core_ops* expected_core_ops_ptr,
        const synthclone::effect_editor_ops* expected_editor_ops_ptr,
        const synthclone::effect_state_ops* expected_state_ops_ptr,
        const synthclone::metadata& expected_metadata
    )
    {
        synthclone::verify_eq(expected_core_ops_ptr, type.core_ops().get());
        synthclone::verify_eq(
            expected_editor_ops_ptr, type.editor_ops().get());
        synthclone::verify_eq(expected_state_ops_ptr, type.state_ops().get());
        synthclone::verify_eq(expected_metadata, type.metadata());
    }

}

BOOST_AUTO_TEST_SUITE(effect)

BOOST_AUTO_TEST_CASE(instance)
{
    std::unique_ptr<synthclone::effect_instance> instance(
        std::make_unique<test_instance>());
    instance.reset();
}

BOOST_AUTO_TEST_CASE(types)
{
    auto core_ops_1 = std::make_unique<test_core_ops>();
    const auto* core_ops_1_ptr = core_ops_1.get();
    synthclone::metadata metadata_1(
        {
            .identifier = "foo",
            .version = "1.2.3"
        });
    synthclone::effect_type type_1(
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

    synthclone::metadata metadata_2(
        {
            .identifier = "bar",
            .version = "4.5.6"
        });
    synthclone::effect_type type_2(
        {
            .core_ops = std::move(core_ops_2),
            .editor_ops = std::move(editor_ops_2),
            .state_ops = std::move(state_ops_2),
            .metadata = metadata_2
        });
    verify_type(
        type_2, core_ops_2_ptr, editor_ops_2_ptr, state_ops_2_ptr, metadata_2);

    synthclone::effect_type type_3 = std::move(type_1);
    verify_type(type_3, core_ops_1_ptr, nullptr, nullptr, metadata_1);

    type_1 = std::move(type_2);
    verify_type(
        type_1, core_ops_2_ptr, editor_ops_2_ptr, state_ops_2_ptr, metadata_2);

    BOOST_CHECK_THROW(
        synthclone::effect_type(
            {
                .core_ops = nullptr,
                .metadata = metadata_1
            }),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_SUITE_END()
