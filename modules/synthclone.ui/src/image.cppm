module;

#include <QtCore/qobjectdefs.h>

export module synthclone.ui:image;

import std;

import synthclone.external.boost.urls;
import synthclone.external.qt.concurrent;
import synthclone.external.qt.core;
import synthclone.external.qt.gui;
import synthclone.external.qt.quick;
import synthclone.qt;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::theme_icon_provider
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    using theme_icon_load_result = std::expected<::QImage, ::QString>;

    class theme_icon_response final: public ::QQuickImageResponse {

    public:

        explicit
        theme_icon_response(const ::QFuture<theme_icon_load_result>& future):
            finish_connection_guard_(
                &watcher_, &::QFutureWatcher<theme_icon_load_result>::finished,
                [this]() mutable { this->process_result(); })
        {
            // We set the future here so that any signals emitted when the
            // future is set (say, if the operation has already been completed)
            // are emitted *after* the watcher signals are connected.
            watcher_.setFuture(future);
        }

        Q_SLOT
        void
        cancel()
        override final
        {
            watcher_.cancel();
        }

        ::QString
        errorString()
        const
        override final
        {
            const ::QString* message = std::get_if<::QString>(&result_);
            return message != nullptr ? *message : "";
        }

        ::QQuickTextureFactory*
        textureFactory()
        const
        override final
        {
            const ::QImage* image = std::get_if<::QImage>(&result_);
            assume(image != nullptr, "image is set to NULL");
            return ::QQuickTextureFactory::textureFactoryForImage(*image);
        }

    private:

        void
        process_result()
        {
            if (! watcher_.isCanceled()) {
                auto result = watcher_.result();
                if (! result) {
                    result_ = std::move(result.error());
                } else {
                    result_ = std::move(result.value());
                }
                Q_EMIT finished();
            }
        }

        std::variant<std::monostate, ::QImage, ::QString> result_;
        ::QFutureWatcher<theme_icon_load_result> watcher_;
        qobject_connection_guard finish_connection_guard_;

    };

    ::QImage
    load_theme_icon(const std::string& id, ::QSize size)
    {
        auto parse_result = boost::urls::parse_relative_ref(id);
        if (! parse_result) [[unlikely]] {
            throw std::runtime_error(
                std::format(
                    "boost::urls::parse_relative_ref({0:?}): failed to parse "
                    "icon theme URL: {1}",
                    id, parse_result.error().message()));
        }

        const auto& url_view = parse_result.value();
        assume(
            ! url_view.has_scheme(),
            "boost::urls::parse_relative_ref({0:?}) returned url view with "
            "non-empty scheme",
            id);

        ::QIcon::Mode mode = ::QIcon::Mode::Normal;
        ::QIcon::State state = ::QIcon::State::Off;

        std::ranges::for_each(
            url_view.params(),
            [&id, &mode, &state](boost::urls::param param) {
                const auto& key = param.key;
                if (key == "mode") {

                    if (! param.has_value) {
                        throw std::runtime_error(
                            std::format(
                                "{0:?}: mode parameter has empty value", id));
                    }

                    // XXX: We can probably do better than this once we're
                    // using a version of Qt wherein `::QIcon` uses the
                    // `Q_GADGET` macro.
                    const auto &value = param.value;
                    if (value == "Active") {
                        mode = ::QIcon::Mode::Active;
                    } else if (value == "Disabled") {
                        mode = ::QIcon::Mode::Disabled;
                    } else if (value == "Normal") {
                        mode = ::QIcon::Mode::Normal;
                    } else if (value == "Selected") {
                        mode = ::QIcon::Mode::Selected;
                    } else {
                        throw std::runtime_error(
                            std::format(
                                "{0:?}: {1:?}: invalid value for 'mode' "
                                "parameter",
                                id, value));
                    }

                } else if (key == "state") {

                    if (! param.has_value) {
                        throw std::runtime_error(
                            std::format(
                                "{0:?}: state parameter has empty value", id));
                    }

                    // XXX: We can probably do better than this once we're
                    // using a version of Qt wherein `::QIcon` uses the
                    // `Q_GADGET` macro.
                    const auto &value = param.value;
                    if (value == "Off") {
                        state = ::QIcon::State::Off;
                    } else if (value == "On") {
                        state = ::QIcon::State::On;
                    } else {
                        throw std::runtime_error(
                            std::format(
                                "{0:?}: {1:?}: invalid value for 'state' "
                                "parameter",
                                id, value));
                    }
                }
            });

        auto path = url_view.path();

        auto icon = ::QIcon::fromTheme(::QString::fromStdString(path));
        if (icon.isNull()) {
            throw std::runtime_error(
                std::format("{0:?}: theme icon is not available", path));
        }

        if (! size.isValid()) {
            auto available_sizes = icon.availableSizes(mode, state);
            if (available_sizes.empty()) {
                throw std::runtime_error(
                    std::format(
                        "{0:?}: icon has no available sizes for the given "
                        "mode and state",
                        id));
            }

            // XXX: Is the first size the *best* size?
            size = available_sizes.front();
        }

        auto pixmap = icon.pixmap(size, mode, state);
        if (pixmap.isNull()) {
            throw std::runtime_error(
                std::format(
                    "{0:?}: failed to load pixmap for theme icon", id));
        }

        auto image = pixmap.toImage();
        assume(! image.isNull(), "theme icon image is NULL");
        return image;
    }

    std::unique_ptr<::QThreadPool>
    make_theme_icon_thread_pool()
    {
        auto pool = std::make_unique<::QThreadPool>();
        pool->setMaxThreadCount(1);
        return pool;
    }

    export
    class theme_icon_provider final: public ::QQuickAsyncImageProvider {

    public:

        explicit
        theme_icon_provider():
            thread_pool_(make_theme_icon_thread_pool())
        {
            // empty
        }

        ::QQuickImageResponse*
        requestImageResponse(
            const ::QString& id,
            const ::QSize& size
        )
        override final
        {
            auto future = ::QtConcurrent::run(
                thread_pool_.get(),
                [id = id.toStdString(), size]() -> theme_icon_load_result {
                    try {
                        return load_theme_icon(id, size);
                    } catch (const std::exception& e) {
                        return std::unexpected(e.what());
                    } catch (...) {
                        return std::unexpected("unknown error");
                    }
                });

            // This hurts.
            return new theme_icon_response(future);
        }

    private:

        std::unique_ptr<::QThreadPool> thread_pool_;

    };

}
