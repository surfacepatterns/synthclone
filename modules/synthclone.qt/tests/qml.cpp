#include <boost/test/unit_test.hpp>

import std;

import synthclone.external.qt.core;
import synthclone.external.qt.qml;
import synthclone.external.qt.quick;
import synthclone.qt;
import synthclone.test;

namespace {

    void
    verify_loaded_component(const std::unique_ptr<::QQmlComponent>& component)
    {
        BOOST_CHECK(! component->isError());
        BOOST_CHECK(! component->isLoading());
        BOOST_CHECK(! component->isNull());
        BOOST_CHECK(component->isReady());
    }

}

BOOST_AUTO_TEST_SUITE(qml)

BOOST_AUTO_TEST_CASE(create)
{
    ::QQmlEngine engine;
    auto component = synthclone::load_qml_component(engine, "QtQuick", "Item");
    auto instance = synthclone::create(*component);
    auto item = synthclone::qobject_cast<::QQuickItem>(instance.get());

    ::QQmlComponent bad_component;
    BOOST_CHECK_EXCEPTION(
        synthclone::create(bad_component), synthclone::qml_error,
        [&bad_component](const synthclone::qml_error& e) {
            const auto expected_diagnostics = bad_component.errors();
            const auto diagnostics = e.diagnostics();
            if (expected_diagnostics.size() != diagnostics.size()) {
                return false;
            }

            auto expected_iter = expected_diagnostics.begin();
            for (const auto s: diagnostics) {
                if (s != expected_iter->toString().toStdString()) {
                    return false;
                }
                ++expected_iter;
            }

            return true;
        });
}

BOOST_AUTO_TEST_CASE(load_qml_component)
{
    ::QQmlEngine engine;
    auto component = synthclone::load_qml_component(engine, "QtQuick", "Item");
    verify_loaded_component(component);

    component = synthclone::load_qml_component(
        engine, "QtQuick", "Item",
        ::QQmlComponent::CompilationMode::Asynchronous);
    verify_loaded_component(component);

    BOOST_CHECK_THROW(
        synthclone::load_qml_component(engine, "", "Item"),
        synthclone::qml_error);
    BOOST_CHECK_THROW(
        synthclone::load_qml_component(engine, "QtQuick", "NoSuchComponent"),
        synthclone::qml_error);
}

BOOST_AUTO_TEST_SUITE_END()
