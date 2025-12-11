export module synthclone.qt:qml;

import std;

import synthclone.external.qt.core;
import synthclone.external.qt.qml;
import synthclone.util;

import :object;

///////////////////////////////////////////////////////////////////////////////
// synthclone::qml_error
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    class qml_error: public std::runtime_error {

    public:

        template<std::ranges::input_range T>
        requires (
            std::convertible_to<std::ranges::range_value_t<T>, std::string>
        )
        qml_error(const std::string& what_arg, T&& diagnostics):
            std::runtime_error(what_arg),
            diagnostics_(std::forward<T>(diagnostics))
        {
            // empty
        }

        template<std::ranges::input_range T>
        requires (
            std::convertible_to<std::ranges::range_value_t<T>, std::string>
        )
        qml_error(const char* what_arg, T&& diagnostics):
            std::runtime_error(what_arg),
            diagnostics_(std::forward<T>(diagnostics))
        {
            // empty
        }

        constexpr
        auto
        diagnostics()
        const
        {
            return std::ranges::subrange(diagnostics_);
        }

    private:

        std::vector<std::string> diagnostics_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::load_qml_component()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    std::vector<std::string>
    generate_qml_error_diagnostics(const ::QList<::QQmlError>& errors)
    {
        std::vector<std::string> diagnostics;
        diagnostics.reserve(errors.size());

        for (const auto& error: errors) {
            diagnostics.push_back(error.toString().toStdString());
        }

        return diagnostics;
    }

    export
    std::unique_ptr<::QQmlComponent>
    load_qml_component(
        qobject_ptr<::QQmlEngine> engine,
        std::string_view module_uri,
        std::string_view type_name,
        ::QQmlComponent::CompilationMode mode =
            ::QQmlComponent::CompilationMode::PreferSynchronous
    )
    {
        auto component = std::make_unique<::QQmlComponent>(
            engine, module_uri, type_name, mode);

        for (;;) {
            auto status = component->status();
            switch (status) {
            case ::QQmlComponent::Loading:
                ::QCoreApplication::processEvents(
                    ::QEventLoop::WaitForMoreEvents);
                continue;
            case ::QQmlComponent::Ready:
                goto ready;
            [[unlikely]] case ::QQmlComponent::Error:
                throw qml_error(
                    std::format(
                        "failed to load QML component {0:?} from module {1:?}",
                        type_name, module_uri),
                    generate_qml_error_diagnostics(component->errors()));
            [[unlikely]] default:
                assume_unreachable(
                    "unexpected status: {0}", std::to_underlying(status));
            }
        }

    ready:
        return component;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::create()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    std::unique_ptr<::QObject>
    create(
        qobject_ptr<::QQmlComponent> component,
        ::QQmlContext* context = nullptr
    )
    {
        std::unique_ptr<::QObject> result(component->create(context));
        if (result == nullptr) [[unlikely]] {
            throw qml_error(
                std::format(
                    "failed to create QML object from component {0:?}",
                    component->url().url().toStdString()),
                generate_qml_error_diagnostics(component->errors()));
        }
        return result;
    }

}
