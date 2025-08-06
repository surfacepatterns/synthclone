/**
 * @file
 *
 * Contains filesystem functionality not directly supported by the standard
 * library.
 */

module;

#include <cerrno>

#include <unistd.h>

#include <boost/scope/scope_fail.hpp>

#include <synthclone/config.h>

export module synthclone.util:filesystem;

import std;

import :utility;

///////////////////////////////////////////////////////////////////////////////
// synthclone::temporary_directory
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class T>
    concept temporary_filesystem_object_creator =
        std::is_nothrow_default_constructible_v<T> &&
        requires (T t, std::filesystem::path path) {
            { t(path) } -> std::same_as<std::filesystem::path>;
        };

    template<class T>
    concept temporary_filesystem_object_destroyer =
        std::is_nothrow_default_constructible_v<T> &&
        requires (T t, std::filesystem::path path) {
            t(path);
        };

    template<
        temporary_filesystem_object_creator C,
        temporary_filesystem_object_destroyer D
    >
    class temporary_filesystem_object final: private noncopyable {

    public:

        /**
         * Constructs a temporary filesystem object that creates the temporary
         * resource in the system-specified temporary directory.
         */

        temporary_filesystem_object():
            temporary_filesystem_object(std::filesystem::temp_directory_path())
        {
            // empty
        }

        /**
         * Constructs a temporary filesystem object using the contents of the
         * given temporary filesystem object.
         *
         * @param other
         *   The temporary filesystem object to move.
         */

        temporary_filesystem_object(
            temporary_filesystem_object&& other
        ) noexcept:
            path_(std::move(other.path_)),
            released_(other.released_)
        {
            other.released_ = true;
        }

        /**
         * Constructs a temporary filesystem object inside the given directory.
         *
         * @param parent_path
         *   The path to the directory to create the temporary resource in.
         */

        explicit
        temporary_filesystem_object(const std::filesystem::path& parent_path):
            path_(C()(parent_path)),
            released_(false)
        {
            // empty
        }

        /**
         * Destructs a temporary filesystem object, removing the temporary
         * resource if the resource has not been released.
         */

        ~temporary_filesystem_object()
        {
            destroy();
        }

        /**
         * Assigns the contents of the given temporary filesystem object to
         * this temporary filesystem object.
         *
         * @param other
         *   The temporary filesystem object to move.
         *
         * @return
         *   This temporary filesystem object.
         */

        temporary_filesystem_object&
        operator=(temporary_filesystem_object&& other)
        {
            destroy();
            path_ = std::move(other.path_);
            released_ = other.released_;
            other.released_ = true;
            return *this;
        }

        /**
         * Gets the path to the temporary resource.
         *
         * @return
         *   The path to the resource.
         */

        constexpr
        const std::filesystem::path&
        path() const noexcept
        {
            return path_;
        }

        /**
         * Releases the temporary resource, thereby cancelling RAII collection
         * of the underlying resource.
         */

        void
        release() noexcept
        {
            path_ = std::filesystem::path();
            released_ = true;
        }

        /**
         * Destroys the underlying temporary resource.
         */

        void
        reset()
        {
            if (! released_) {
                D()(path_);
                release();
            }
        }

    private:

        void
        destroy()
        {
            if (! released_) {
                D()(path_);
            }
        }

        std::filesystem::path path_;
        bool released_;

    };

    constexpr char temp_template[] = "XXXXXX";

    struct temporary_directory_creator final {

        std::filesystem::path
        operator()(const std::filesystem::path& parent_path)
        {
            auto path_str = (parent_path / temp_template).string();
            if (::mkdtemp(path_str.data()) == nullptr) [[unlikely]] {
                throw std::filesystem::filesystem_error(
                    "failed to make temporary directory", parent_path,
                    std::error_code(errno, std::generic_category()));
            }

            boost::scope::scope_fail remove_guard(
                [&path_str]() {
                    if (::rmdir(path_str.c_str()) != 0) [[unlikely]] {
                        // We can't throw `std::filesystem::filesystem_error`
                        // because that would require creating an instance of
                        // `std::filesystem::path`, which is the operation that
                        // failed and got us here in the first place.
                        std::throw_with_nested(
                            std::system_error(
                                errno, std::generic_category(), path_str));
                    }
                });

            return std::filesystem::path(path_str);
        }

    };

    struct temporary_directory_destroyer final {

        inline void
        operator()(const std::filesystem::path& path)
        {
            // If the directory doesn't exist, `std::filesystem::remove_all`
            // returns 0 instead of raising an exception (at least in
            // `libc++`).
            if (std::filesystem::remove_all(path) == 0) [[unlikely]] {
                throw std::filesystem::filesystem_error(
                    "failed to remove temporary directory", path,
                    std::make_error_code(
                        std::errc::no_such_file_or_directory));
            }
        }

    };

    /**
     * Basic RAII mechanism for temporary directories.
     */

    export
    using temporary_directory = temporary_filesystem_object<
        temporary_directory_creator, temporary_directory_destroyer
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::temporary_file
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    struct temporary_file_creator final {

        std::filesystem::path
        operator()(const std::filesystem::path& parent_path)
        {
            auto path_str = (parent_path / temp_template).string();

            int fd = ::mkstemp(path_str.data());
            if (fd == -1) [[unlikely]] {
                throw std::filesystem::filesystem_error(
                    "failed to make temporary file", parent_path,
                    std::error_code(errno, std::generic_category()));
            }

            boost::scope::scope_fail remove_guard(
                [&path_str]() {
                    if (::unlink(path_str.c_str()) != 0) [[unlikely]] {
                        // We can't throw `std::filesystem::filesystem_error`
                        // because that would require creating an instance of
                        // `std::filesystem::path`, which might be the
                        // operation that failed and got us here in the first
                        // place.
                        std::throw_with_nested(
                            std::system_error(
                                errno, std::generic_category(), path_str));
                    }
                });

            if (::close(fd) == -1) [[unlikely]] {
                throw std::filesystem::filesystem_error(
                    "failed to close temporary file",
                    std::filesystem::path(path_str),
                    std::error_code(errno, std::generic_category()));
            }

            return std::filesystem::path(path_str);
        }

    };

    struct temporary_file_destroyer final {

        inline void
        operator()(const std::filesystem::path& path)
        {
            if (! std::filesystem::remove(path)) [[unlikely]] {
                throw std::filesystem::filesystem_error(
                    "failed to remove temporary file", path,
                    std::make_error_code(
                        std::errc::no_such_file_or_directory));
            }
        }

    };

    /**
     * Basic RAII mechanism for temporary files.
     */

    export
    using temporary_file = temporary_filesystem_object<
        temporary_file_creator, temporary_file_destroyer
    >;

}
