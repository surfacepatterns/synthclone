#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.core.test;
import synthclone.test;
import synthclone.util;

class QQuickItem;

namespace {

    class test_core_ops final: public synthclone::importer_core_ops {

    public:

        std::unique_ptr<synthclone::importer_instance>
        create()
        override final
        {
            return nullptr;
        }

        std::generator<synthclone::importer_run_message>
        run(
            synthclone::importer_instance& instance,
            std::stop_token stop_token
        )
        override final
        {
            co_return;
        }

    };

    class test_editor_ops final: public synthclone::importer_editor_ops {

    public:

        std::generator<synthclone::importer_edit_message>
        edit(
            synthclone::importer_instance& instance,
            ::QQuickItem* parent,
            std::stop_token stop_token
        )
        override final
        {
            co_return;
        }

    };

    class test_instance final: public synthclone::importer_instance {};

    class test_state_ops final: public synthclone::importer_state_ops {

    public:

        synthclone::state_value
        dump(const synthclone::importer_instance& instance)
        override final
        {
            return synthclone::state_value();
        }

        std::unique_ptr<synthclone::importer_instance>
        load(const synthclone::state_value& value)
        override final
        {
            return nullptr;
        }

    };

    void
    verify_type(
        const synthclone::importer_type& type,
        const synthclone::importer_core_ops* expected_core_ops_ptr,
        const synthclone::importer_editor_ops* expected_editor_ops_ptr,
        const synthclone::importer_state_ops* expected_state_ops_ptr,
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

BOOST_AUTO_TEST_SUITE(importer)

BOOST_AUTO_TEST_CASE(instance)
{
    std::unique_ptr<synthclone::importer_instance> instance(
        std::make_unique<test_instance>());
    instance.reset();
}

BOOST_AUTO_TEST_CASE(types)
{
    auto core_ops_1 = std::make_unique<test_core_ops>();
    const auto* core_ops_1_ptr = core_ops_1.get();
    auto editor_ops_1 = std::make_unique<test_editor_ops>();
    const auto* editor_ops_1_ptr = editor_ops_1.get();
    synthclone::metadata_init_args metadata_1(
        {
            .identifier = "foo",
            .version = "1.2.3"
        });
    synthclone::importer_type type_1(
        {
            .core_ops = std::move(core_ops_1),
            .editor_ops = std::move(editor_ops_1),
            .metadata = metadata_1
        });
    verify_type(type_1, core_ops_1_ptr, editor_ops_1_ptr, nullptr, metadata_1);

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
    synthclone::importer_type type_2(
        {
            .core_ops = std::move(core_ops_2),
            .editor_ops = std::move(editor_ops_2),
            .state_ops = std::move(state_ops_2),
            .metadata = metadata_2
        });
    verify_type(
        type_2, core_ops_2_ptr, editor_ops_2_ptr, state_ops_2_ptr, metadata_2);

    synthclone::importer_type type_3 = std::move(type_1);
    verify_type(type_3, core_ops_1_ptr, editor_ops_1_ptr, nullptr, metadata_1);

    type_1 = std::move(type_2);
    verify_type(
        type_1, core_ops_2_ptr, editor_ops_2_ptr, state_ops_2_ptr, metadata_2);

    BOOST_CHECK_THROW(
        synthclone::importer_type(
            {
                .core_ops = std::make_unique<test_core_ops>(),
                .metadata = metadata_1
            }),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::importer_type(
            {
                .editor_ops = std::make_unique<test_editor_ops>(),
                .metadata = metadata_1
            }),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(zone_messages)
{
    synthclone::midi_control_array unset_controls;

    synthclone::importer_zone_message message_1(
        synthclone::zone_port_params(
            {
                .channel = 2,
                .note = 40,
                .velocity = 70
            }));
    synthclone::verify_zone_port_params(
        message_1.params(), 2, 40, 70, std::nullopt, std::nullopt,
        unset_controls, std::nullopt);

    synthclone::importer_zone_message message_2(
        synthclone::zone_port_params(
            {
                .channel = 4,
                .note = 80,
                .velocity = 35
            }));
    synthclone::verify_zone_port_params(
        message_2.params(), 4, 80, 35, std::nullopt, std::nullopt,
        unset_controls, std::nullopt);

    synthclone::importer_zone_message message_3(std::move(message_1));
    synthclone::verify_zone_port_params(
        message_3.params(), 2, 40, 70, std::nullopt, std::nullopt,
        unset_controls, std::nullopt);

    message_1 = std::move(message_2);
    synthclone::verify_zone_port_params(
        message_1.params(), 4, 80, 35, std::nullopt, std::nullopt,
        unset_controls, std::nullopt);
}

BOOST_AUTO_TEST_SUITE_END()
