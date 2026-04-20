// The `libarchive` API leaves a lot to be desired.  The `archive_entry` struct
// exposes attributes that some of the supported archive formats don't actually
// use and doesn't contain attributes corresponding to some of the attributes
// that the formats do use, and some of the `libarchive` code makes decisions
// for the application (e.g. parts of the code will abort on behalf of the
// application when memory can't be allocated).  It's a bit of a clusterfuck.

module;

#include <cerrno>

export module synthclone.archive:internal;

import std;

import synthclone.external.archive;
import synthclone.external.boost.interprocess;
import synthclone.external.posix;
import synthclone.util;

import :core;

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_entry_pointer
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<class T>
    concept archive_entry_pointer = pointer_to<T, ::archive_entry>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::assume_archive_entry()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<archive_entry_pointer T>
    constexpr
    pointer_address_t<T>
    assume_archive_entry(T& entry)
    {
        auto ptr = std::to_address(entry);
        assume(ptr != nullptr, "`entry` is set to NULL");
        return ptr;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_entry_ptr
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    struct archive_entry_destroyer final {

        void
        operator()(::archive_entry* entry)
        {
            assume_archive_entry(entry);
            ::archive_entry_free(entry);
        }

    };

    export
    using archive_entry_ptr =
        std::unique_ptr<::archive_entry, archive_entry_destroyer>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::clone_archive_entry()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    archive_entry_ptr
    clone_archive_entry(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);

        archive_entry_ptr result(::archive_entry_clone(ptr));
        if (result == nullptr) [[unlikely]] {
            throw std::bad_alloc();
        }
        return result;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_device_id()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    archive_device_id
    get_archive_entry_device_id(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);
        assume(
            ::archive_entry_rdev_is_set(ptr),
            "entry {0:x} does not have set device id",
            reinterpret_cast<std::uintptr_t>(ptr));

        return archive_device_id(
            ::archive_entry_rdevmajor(ptr), ::archive_entry_rdevminor(ptr));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_file_type()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    unsigned int
    get_archive_entry_file_type(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);
        assume(
            ::archive_entry_filetype_is_set(ptr),
            "entry {0:x} does not have set file type",
            reinterpret_cast<std::uintptr_t>(ptr));
        return ::archive_entry_filetype(ptr);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_gid()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    archive_gid_t
    get_archive_entry_gid(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);
        assume(
            ::archive_entry_gid_is_set(ptr),
            "entry {0:x} does not have set gid",
            reinterpret_cast<std::uintptr_t>(ptr));
        return ::archive_entry_gid(ptr);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_group_name()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    std::string_view
    get_archive_entry_group_name(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);

        errno = 0;
        const auto* s = ::archive_entry_gname(ptr);
        if (s == nullptr) [[unlikely]] {
            assume(
                errno != 0, "::archive_entry_gname({0:x}): group name not set",
                reinterpret_cast<std::uintptr_t>(ptr));
            throw archive_error(
                std::format(
                    "::archive_entry_gname({0:x}): failed to get group name: "
                    "{1}",
                    reinterpret_cast<std::uintptr_t>(ptr),
                    std::generic_category().message(errno)));
        }

        return s;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_hard_link_path()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    constexpr bool is_archive_entry_wchar_path =
        std::same_as<std::filesystem::path::value_type, wchar_t>;

    export
    std::filesystem::path
    get_archive_entry_hard_link_path(const archive_entry_ptr& entry)
    {
        auto* ptr = assume_archive_entry(entry);

        errno = 0;
        if constexpr (is_archive_entry_wchar_path) {
            const auto* s = ::archive_entry_hardlink_w(ptr);
            if (s == nullptr) [[unlikely]] {
                assume(
                    errno != 0,
                    "::archive_entry_hardlink_w({0:x}): hard link not set",
                    reinterpret_cast<std::uintptr_t>(ptr));
                throw archive_error(
                    std::format(
                        "::archive_entry_hardlink_w({0:x}): failed to get "
                        "hard link path: {1}",
                        reinterpret_cast<std::uintptr_t>(ptr),
                        std::generic_category().message(errno)));
            }
            return s;
        } else {
            const auto* s = ::archive_entry_hardlink(ptr);
            if (s == nullptr) [[unlikely]] {
                assume(
                    errno != 0,
                    "::archive_entry_hardlink({0:x}): hard link not set",
                    reinterpret_cast<std::uintptr_t>(ptr));
                throw archive_error(
                    std::format(
                        "::archive_entry_hardlink({0:x}): failed to get "
                        "hard link path: {1}",
                        reinterpret_cast<std::uintptr_t>(ptr),
                        std::generic_category().message(errno)));
            }
            return s;
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_modification_time()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    std::chrono::nanoseconds
    get_archive_entry_modification_time(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);
        assume(
            ::archive_entry_mtime_is_set(ptr),
            "entry {0:x} does not have set modification time",
            reinterpret_cast<std::uintptr_t>(ptr));
        return std::chrono::seconds(::archive_entry_mtime(ptr)) +
            std::chrono::nanoseconds(::archive_entry_mtime_nsec(ptr));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_path()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    std::filesystem::path
    get_archive_entry_path(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);

        errno = 0;
        if constexpr (is_archive_entry_wchar_path) {
            const auto* s = ::archive_entry_pathname_w(ptr);
            if (s == nullptr) [[unlikely]] {
                assume(
                    errno != 0,
                    "::archive_entry_pathname_w({0:x}): path not set",
                    reinterpret_cast<std::uintptr_t>(ptr));
                throw archive_error(
                    std::format(
                        "::archive_entry_pathname_w({0:x}): failed to get "
                        "path: {1}",
                        reinterpret_cast<std::uintptr_t>(ptr),
                        std::generic_category().message(errno)));
            }
            return s;
        } else {
            const auto* s = ::archive_entry_pathname(ptr);
            if (s == nullptr) [[unlikely]] {
                assume(
                    errno != 0,
                    "::archive_entry_pathname({0:x}): path not set",
                    reinterpret_cast<std::uintptr_t>(ptr));
                throw archive_error(
                    std::format(
                        "::archive_entry_pathname({0:x}): failed to get "
                        "path: {1}",
                        reinterpret_cast<std::uintptr_t>(ptr),
                        std::generic_category().message(errno)));
            }
            return s;
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_permissions()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    std::filesystem::perms
    get_archive_entry_permissions(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);
        assume(
            ::archive_entry_perm_is_set(ptr),
            "entry {0:x} does not have set permissions",
            reinterpret_cast<std::uintptr_t>(ptr));
        return std::filesystem::perms(::archive_entry_perm(ptr));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_size()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    archive_size_t
    get_archive_entry_size(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);
        assume(
            ::archive_entry_size_is_set(ptr),
            "entry {0:x} does not have set size",
            reinterpret_cast<std::uintptr_t>(ptr));
        return ::archive_entry_size(ptr);
     }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_symbolic_link_path()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    std::filesystem::path
    get_archive_entry_symbolic_link_path(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);

        errno = 0;
        if constexpr (is_archive_entry_wchar_path) {
            const auto* s = ::archive_entry_symlink_w(ptr);
            if (s == nullptr) [[unlikely]] {
                assume(
                    errno != 0,
                    "::archive_entry_symlink_w({0:x}): symbolic link path not "
                    "set",
                    reinterpret_cast<std::uintptr_t>(ptr));
                throw archive_error(
                    std::format(
                        "::archive_entry_symlink_w({0:x}): failed to get "
                        "symbolic link path: {1}",
                        reinterpret_cast<std::uintptr_t>(ptr),
                        std::generic_category().message(errno)));
            }
            return s;
        } else {
            const auto* s = ::archive_entry_symlink(ptr);
            if (s == nullptr) [[unlikely]] {
                assume(
                    errno != 0,
                    "::archive_entry_symlink({0:x}): symbolic link path not "
                    "set",
                    reinterpret_cast<std::uintptr_t>(ptr));
                throw archive_error(
                    std::format(
                        "::archive_entry_symlink({0:x}): failed to get "
                        "symbolic link path: {1}",
                        reinterpret_cast<std::uintptr_t>(ptr),
                        std::generic_category().message(errno)));
            }
            return s;
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_uid()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    archive_uid_t
    get_archive_entry_uid(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);
        assume(
            ::archive_entry_uid_is_set(ptr),
            "entry {0:x} does not have set uid",
            reinterpret_cast<std::uintptr_t>(ptr));
        return ::archive_entry_uid(ptr);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_archive_entry_user_name()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    std::string_view
    get_archive_entry_user_name(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);

        errno = 0;
        const auto* s = ::archive_entry_uname(ptr);
        if (s == nullptr) [[unlikely]] {
            assume(
                errno != 0, "::archive_entry_uname({0:x}): user name not set",
                reinterpret_cast<std::uintptr_t>(ptr));
            throw archive_error(
                std::format(
                    "::archive_entry_uname({0:x}): failed to get user name: "
                    "{1}",
                    reinterpret_cast<std::uintptr_t>(ptr),
                    std::generic_category().message(errno)));
        }

        return s;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::is_archive_entry_hard_link()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    bool
    is_archive_entry_hard_link(const archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);
        return ::archive_entry_hardlink_is_set(ptr);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::make_archive_entry()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    archive_entry_ptr
    make_archive_entry()
    {
        archive_entry_ptr entry(::archive_entry_new());
        if (entry == nullptr) [[unlikely]] {
            throw std::bad_alloc();
        }
        return entry;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_device_id()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    void
    set_archive_entry_device_id(
        const archive_entry_ptr& entry,
        const archive_device_id& device_id
    )
    {
        auto ptr = assume_archive_entry(entry);
        ::archive_entry_set_rdevmajor(ptr, device_id.major());
        ::archive_entry_set_rdevminor(ptr, device_id.minor());
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_file_type()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<unsigned int Type>
    void
    set_archive_entry_file_type(archive_entry_ptr& entry)
    {
        auto ptr = assume_archive_entry(entry);
        ::archive_entry_set_filetype(ptr, Type);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_gid()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    set_archive_entry_gid(const archive_entry_ptr& entry, archive_gid_t gid)
    {
        auto ptr = assume_archive_entry(entry);
        ::archive_entry_set_gid(ptr, gid);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_group_name()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    set_archive_entry_group_name(
        const archive_entry_ptr& entry,
        const std::string& name
    )
    {
        auto ptr = assume_archive_entry(entry);
        ::archive_entry_copy_gname(ptr, name.c_str());
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_hard_link_path()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    void
    set_archive_entry_hard_link_path(::archive_entry* ptr, const char* s)
    {
        ::archive_entry_copy_hardlink(ptr, s);
    }

    void
    set_archive_entry_hard_link_path(::archive_entry* ptr, const wchar_t* s)
    {
        ::archive_entry_copy_hardlink_w(ptr, s);
    }

    export
    inline
    void
    set_archive_entry_hard_link_path(
        const archive_entry_ptr& entry,
        const std::filesystem::path& path
    )
    {
        // XXX: `if constexpr` doesn't work in this context.
        // `path::value_type` ends up being evaluated in both branches of the
        // `if constexpr`.
        auto* ptr = assume_archive_entry(entry);
        set_archive_entry_hard_link_path(ptr, path.c_str());
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_modification_time()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    set_archive_entry_modification_time(
        const archive_entry_ptr& entry,
        std::chrono::nanoseconds time
    )
    {
        auto ptr = assume_archive_entry(entry);
        ::archive_entry_set_mtime(ptr, 0, time.count());
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_path()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    void
    set_archive_entry_path(::archive_entry* ptr, const char* s)
    {
        ::archive_entry_copy_pathname(ptr, s);
    }

    void
    set_archive_entry_path(::archive_entry* ptr, const wchar_t* s)
    {
        ::archive_entry_copy_pathname_w(ptr, s);
    }

    export
    inline
    void
    set_archive_entry_path(
        const archive_entry_ptr& entry,
        const std::filesystem::path& path
    )
    {
        // XXX: `if constexpr` doesn't work in this context.
        // `path::value_type` ends up being evaluated in both branches of the
        // `if constexpr`.
        auto ptr = assume_archive_entry(entry);
        set_archive_entry_path(ptr, path.c_str());
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_permissions()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    set_archive_entry_permissions(
        const archive_entry_ptr& entry,
        std::filesystem::perms permissions
    )
    {
        auto ptr = assume_archive_entry(entry);
        ::archive_entry_set_perm(ptr, static_cast<::mode_t>(permissions));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_size()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    set_archive_entry_size(const archive_entry_ptr& entry, archive_size_t size)
    {
        auto ptr = assume_archive_entry(entry);
        ::archive_entry_set_size(ptr, size);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_symbolic_link_path()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    void
    set_archive_entry_symbolic_link_path(::archive_entry* ptr, const char* s)
    {
        ::archive_entry_copy_symlink(ptr, s);
    }

    void
    set_archive_entry_symbolic_link_path(
        ::archive_entry* ptr,
        const wchar_t* s
    )
    {
        ::archive_entry_copy_symlink_w(ptr, s);
    }

    export
    inline
    void
    set_archive_entry_symbolic_link_path(
        const archive_entry_ptr& entry,
        const std::filesystem::path& path
    )
    {
        // XXX: `if constexpr` doesn't work in this context.
        // `path::value_type` ends up being evaluated in both branches of the
        // `if constexpr`.
        auto ptr = assume_archive_entry(entry);
        set_archive_entry_symbolic_link_path(ptr, path.c_str());
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_uid()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    set_archive_entry_uid(const archive_entry_ptr& entry, archive_uid_t uid)
    {
        auto ptr = assume_archive_entry(entry);
        ::archive_entry_set_uid(ptr, uid);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_entry_user_name()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    set_archive_entry_user_name(
        const archive_entry_ptr& entry,
        const std::string& name
    )
    {
        auto ptr = assume_archive_entry(entry);
        ::archive_entry_copy_uname(ptr, name.c_str());
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_pointer_type
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<class T>
    concept archive_pointer_type = pointer_to<T, ::archive>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_ptr
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<archive_pointer_type T>
    constexpr
    pointer_address_t<T>
    assume_archive(T& archive)
    {
        auto ptr = std::to_address(archive);
        assume(ptr != nullptr, "`archive` is set to NULL");
        return ptr;
    }

    template<archive_pointer_type T>
    const char*
    extract_archive_error_message(T& archive)
    {
        auto ptr = assume_archive(archive);

        const auto* message = ::archive_error_string(ptr);
        assume(
            message != nullptr,
            "::archive_error_string({0:x}): error message is not set",
            reinterpret_cast<std::uintptr_t>(ptr));
        return message;
    }

    struct archive_destroyer final {

        void
        operator()(::archive* archive)
        {
            assume_archive(archive);

            if (::archive_free(archive) != ::ARCHIVE_OK) [[unlikely]] {
                throw archive_error(
                    std::format(
                        "::archive_free({0:x}): failed to free archive: {1}",
                        reinterpret_cast<std::uintptr_t>(archive),
                        extract_archive_error_message(archive)));
            }
        }

    };

    using archive_ptr = std::unique_ptr<::archive, archive_destroyer>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::add_archive_reader_tar_format_support()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    void
    add_archive_reader_tar_format_support(const archive_ptr& archive)
    {
        auto ptr = assume_archive(archive);

        if (
            ::archive_read_support_format_tar(ptr) != ::ARCHIVE_OK
        ) [[unlikely]] {
            throw archive_error(
                std::format(
                    "::archive_read_support_format_tar({0:x}): failed to "
                    "add support: {1}",
                    reinterpret_cast<std::uintptr_t>(ptr),
                    extract_archive_error_message(ptr)));
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::make_archive_reader()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    archive_ptr
    make_archive_reader()
    {
        archive_ptr archive(::archive_read_new());
        if (archive == nullptr) [[unlikely]] {
            throw std::bad_alloc();
        }
        return archive;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::make_archive_writer()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    inline
    archive_ptr
    make_archive_writer()
    {
        archive_ptr archive(::archive_write_new());
        if (archive == nullptr) [[unlikely]] {
            throw std::bad_alloc();
        }
        return archive;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::read_archive_entry()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<archive_pointer_type T>
    constexpr
    pointer_address_t<T>
    verify_archive(T& archive)
    {
        auto ptr = std::to_address(archive);
        verify(ptr != nullptr, "`archive` is set to NULL");
        return ptr;
    }

    export
    bool
    read_archive_entry(
        const archive_ptr& archive,
        const archive_entry_ptr& entry
    )
    {
        auto archive_ptr = verify_archive(archive);
        auto entry_ptr = assume_archive_entry(entry);

        switch (::archive_read_next_header2(archive_ptr, entry_ptr)) {
        case ::ARCHIVE_EOF:
            return false;
        [[likely]] case ::ARCHIVE_OK:
            return true;
        default:
            ;
        }

        throw archive_error(
            std::format(
                "::archive_read_next_header2({0:x}, {1:x}): failed to read "
                "next header: {2}",
                reinterpret_cast<std::uintptr_t>(archive_ptr),
                reinterpret_cast<std::uintptr_t>(entry_ptr),
                extract_archive_error_message(archive_ptr)));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::read_archive_file_fragment()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    archive_file_fragment
    read_archive_file_fragment(const archive_ptr& archive)
    {
        auto ptr = verify_archive(archive);

        const void* data;
        std::size_t length;
        std::int64_t offset;
        switch (::archive_read_data_block(ptr, &data, &length, &offset)) {
        case ::ARCHIVE_EOF:
            return {};

        case ::ARCHIVE_OK:
            assume(
                (data != nullptr) && (length != 0),
                "::archive_read_data_block({0}, {1}, {2}, {3}): unexpected "
                "result: data={4}, length={5}",
                reinterpret_cast<std::uintptr_t>(ptr),
                reinterpret_cast<std::uintptr_t>(&data),
                reinterpret_cast<std::uintptr_t>(&length),
                reinterpret_cast<std::uintptr_t>(&offset),
                reinterpret_cast<std::uintptr_t>(data), length);

            return archive_file_fragment(
                static_cast<const std::byte*>(data), length);

        [[unlikely]] default:
            ;
        }

        throw archive_error(
            std::format(
                "::archive_read_data_block({0}, {1}, {2}, {3}): failed to "
                "read data block: {4}",
                reinterpret_cast<std::uintptr_t>(ptr),
                reinterpret_cast<std::uintptr_t>(&data),
                reinterpret_cast<std::uintptr_t>(&length),
                reinterpret_cast<std::uintptr_t>(&offset),
                extract_archive_error_message(archive)));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_reader_format
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    void
    set_archive_reader_format(const archive_ptr& archive, int format)
    {
        auto ptr = assume_archive(archive);
        if (
            ::archive_read_set_format(ptr, format) != ::ARCHIVE_OK
        ) [[unlikely]] {
            throw archive_error(
                std::format(
                    "::archive_read_set_format({0}, {1}): failed to set "
                    "archive reader format: {2}",
                    reinterpret_cast<std::uintptr_t>(ptr), format,
                    extract_archive_error_message(archive)));
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::set_archive_writer_format()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    void
    set_archive_writer_format(const archive_ptr& archive, int format)
    {
        auto ptr = assume_archive(archive);

        if (
            ::archive_write_set_format(ptr, format) != ::ARCHIVE_OK
        ) [[unlikely]] {
            throw archive_error(
                std::format(
                    "::archive_write_set_format({0}, {1}): failed to set "
                    "archive writer format: {2}",
                    reinterpret_cast<std::uintptr_t>(ptr), format,
                    extract_archive_error_message(archive)));
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_archive_entry()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    void
    write_archive_entry(
        const archive_ptr& archive,
        const archive_entry_ptr& entry
    )
    {
        auto archive_ptr = verify_archive(archive);
        auto entry_ptr = assume_archive_entry(entry);

        if (
            ::archive_write_header(archive_ptr, entry_ptr) != ::ARCHIVE_OK
        ) [[unlikely]] {
            throw archive_error(
                std::format(
                    "::archive_write_header2({0:x}, {1:x}): failed to write "
                    "header: {2}",
                    reinterpret_cast<std::uintptr_t>(archive_ptr),
                    reinterpret_cast<std::uintptr_t>(entry_ptr),
                    extract_archive_error_message(archive_ptr)));
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::write_archive_file_fragment()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    void
    write_archive_file_fragment(
        const archive_ptr& archive,
        archive_file_fragment fragment
    )
    {
        auto ptr = verify_archive(archive);

        const auto* data = fragment.data();
        assume(data != nullptr, "data is set to `NULL`");

        auto size = fragment.size();
        assume(size != 0, "size is set to 0");

        auto result = ::archive_write_data(
            ptr, static_cast<const void*>(data), size);
        if (result < 0) [[unlikely]] {
            throw archive_error(
                std::format(
                    "::archive_write_data({0}, {1}, {2}): failed to write "
                    "data: {3}",
                    reinterpret_cast<std::uintptr_t>(ptr),
                    reinterpret_cast<std::uintptr_t>(data),
                    reinterpret_cast<std::uintptr_t>(size),
                    extract_archive_error_message(archive)));
        }

        assume(
            result == size,
            "incomplete write: {0} out of {1} bytes were written", result,
            size);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_stream_reader_ops
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    // The internal headers for `libarchive` define `ARCHIVE_ERRNO_MISC` for
    // errors where the reason is unknown, but, for whatever reason, that
    // definition is not available in public headers.
    constexpr int archive_errno_misc = -1;

    void
    set_archive_stream_error(::archive* archive, int code, const char* message)
    {
        ::archive_set_error(archive, code, "%s", message);
    }

    void
    set_archive_stream_error(::archive* archive, const ios_error& e)
    {
        set_archive_stream_error(archive, e.code().value(), e.what());
    }

    void
    set_archive_stream_error(::archive* archive, const std::exception& e)
    {
        ::archive_set_error(
            archive, archive_errno_misc, "unexpected error: %s", e.what());
    }

    void
    set_archive_stream_error(::archive* archive)
    {
        ::archive_set_error(archive, archive_errno_misc, "unknown error");
    }

    export
    template<std::derived_from<std::istream> T>
    class archive_stream_reader_ops final: private nonmovable {

    public:

        archive_stream_reader_ops(
            T& stream,
            const archive_ptr& archive,
            std::size_t buffer_size
        ):
            stream_(stream),
            buffer_(buffer_size)
        {
            auto* ptr = assume_archive(archive);

            if (
                ::archive_read_open2(
                    ptr, static_cast<void*>(this), nullptr, read, skip,
                    nullptr) !=
                ::ARCHIVE_OK
            ) [[unlikely]] {
                throw archive_error(
                    std::format(
                        "::archive_read_open2({0:x}, {1:x}, nullptr, {2:x}, "
                        "{3:x}, nullptr): failed to open archive for reading: "
                        "{4}",
                        reinterpret_cast<std::uintptr_t>(ptr),
                        reinterpret_cast<std::uintptr_t>(this),
                        reinterpret_cast<std::uintptr_t>(read),
                        reinterpret_cast<std::uintptr_t>(skip),
                        extract_archive_error_message(archive)));
            }
        }

        archive_stream_reader_ops(T& stream, const archive_ptr& archive):
            archive_stream_reader_ops(
                stream, archive,
                boost::interprocess::mapped_region::get_page_size())
        {
            // empty
        }

    private:

        static
        ::la_ssize_t
        read(::archive* archive, void* ptr, const void** buffer) noexcept
        {
            auto* reader = static_cast<archive_stream_reader_ops*>(ptr);
            auto& stream_buffer = reader->buffer_;

            ::la_ssize_t count;
            try {
                count = static_cast<::la_ssize_t>(
                    synthclone::read(reader->stream_, stream_buffer));
                *buffer = static_cast<const void*>(stream_buffer.data());
                return count;
            } catch (const ios_error& e) {
                set_archive_stream_error(archive, e);
            } catch (const std::exception& e) {
                set_archive_stream_error(archive, e);
            } catch (...) {
                set_archive_stream_error(archive);
            }

            *buffer = nullptr;
            return 0;
        }

        static
        ::la_int64_t
        skip(::archive* archive, void* ptr, ::la_int64_t n) noexcept
        {
            auto& stream =
                static_cast<archive_stream_reader_ops*>(ptr)->stream_;

            try {

                typename T::pos_type old_position;
                try {
                    old_position = tell(stream);
                    seek(
                        stream, static_cast<T::off_type>(n),
                        std::ios_base::cur);
                } catch (const ios_error& e) {
                    // Given that there's nothing in the standard specifying
                    // how `errno` is set during a `seekg()`/`tellg()`, we do
                    // what we can to detect unsupported and invalid seeks,
                    // defaulting to the pessimized *read one buffer at a time*
                    // fallback strategy used by `libarchive` in those cases.
                    auto value = e.code().value();
                    switch (value) {
                    case static_cast<int>(std::errc::invalid_seek):
                        [[fallthrough]];
                    case static_cast<int>(std::errc::io_error):
                        [[fallthrough]];
                    case static_cast<int>(std::errc::not_supported):
                        [[fallthrough]];
                    default:
                        set_archive_stream_error(archive, value, e.what());
                    }
                    return 0;
                }

                try {
                    auto new_position = tell(stream);
                    return static_cast<::la_int64_t>(
                        new_position - old_position);
                } catch (const ios_error& e) {
                    // This is kind of terrible because we've already changed
                    // the position of the stream, but we don't know the new
                    // position, so we can't return a value specifying how much
                    // data we've skipped.
                    set_archive_stream_error(archive, e);
                }

            } catch (const std::exception& e) {
                set_archive_stream_error(archive, e);
            } catch (...) {
                set_archive_stream_error(archive);
            }

            return 0;
        }

        T& stream_;
        dynamic_buffer<std::byte> buffer_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// archive_stream_reader_ops_ptr
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<std::derived_from<std::istream> T>
    using archive_stream_reader_ops_ptr =
        std::unique_ptr<archive_stream_reader_ops<T>>;

}

///////////////////////////////////////////////////////////////////////////////
// archive_stream_writer_ops
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<std::derived_from<std::ostream> T>
    class archive_stream_writer_ops final: private nonmovable {

    public:

        archive_stream_writer_ops(T& stream, const archive_ptr& archive):
            stream_(stream)
        {
            auto* ptr = assume_archive(archive);

            if (
                ::archive_write_open2(
                    ptr, static_cast<void*>(this), nullptr, write, nullptr,
                    nullptr) !=
                ::ARCHIVE_OK
            ) [[unlikely]] {
                throw archive_error(
                    std::format(
                        "::archive_write_open2({0:x}, {1:x}, nullptr, {2:x}, "
                        "nullptr, nullptr): failed to open archive for "
                        "writing: {3}",
                        reinterpret_cast<std::uintptr_t>(ptr),
                        reinterpret_cast<std::uintptr_t>(this),
                        reinterpret_cast<std::uintptr_t>(write),
                        extract_archive_error_message(archive)));
            }
        }

    private:

        static
        ::la_ssize_t
        write(
            ::archive* archive,
            void* ptr,
            const void* buffer,
            std::size_t size
        ) noexcept
        {
            try {
                synthclone::write(
                    static_cast<archive_stream_writer_ops*>(ptr)->stream_,
                    std::span(static_cast<const std::byte*>(buffer), size));
                return static_cast<::la_ssize_t>(size);
            } catch (const ios_error& e) {
                set_archive_stream_error(archive, e);
            } catch (const std::exception& e) {
                set_archive_stream_error(archive, e);
            } catch (...) {
                set_archive_stream_error(archive);
            }

            return static_cast<::la_ssize_t>(-1);
        }

        T& stream_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// archive_stream_writer_ops_ptr
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<std::derived_from<std::ostream> T>
    using archive_stream_writer_ops_ptr =
        std::unique_ptr<archive_stream_writer_ops<T>>;

}
