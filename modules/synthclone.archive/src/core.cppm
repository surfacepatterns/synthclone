module;

export module synthclone.archive:core;

import std;

import synthclone.external.archive;

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_error
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    class archive_error: public std::runtime_error {

        using std::runtime_error::runtime_error;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_device_major_t
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    using archive_device_major_t =
        decltype(::archive_entry_rdevmajor(std::declval<::archive_entry*>()));

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_device_minor_t
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    using archive_device_minor_t =
        decltype(::archive_entry_rdevminor(std::declval<::archive_entry*>()));

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_device_id
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    class archive_device_id final {

    public:

        archive_device_id(const archive_device_id&) = default;

        constexpr
        archive_device_id(
            archive_device_major_t major,
            archive_device_minor_t minor
        ) noexcept:
            major_(major),
            minor_(minor)
        {
            // empty
        }

        archive_device_id&
        operator=(const archive_device_id&) = default;

        constexpr
        archive_device_major_t
        major() const noexcept
        {
            return major_;
        }

        constexpr
        archive_device_minor_t
        minor() const noexcept
        {
            return minor_;
        }

    private:

        archive_device_major_t major_;
        archive_device_minor_t minor_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_file_fragment
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    using archive_file_fragment = std::span<const std::byte>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_gid_t
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    using archive_gid_t = decltype(
        ::archive_entry_gid(std::declval<::archive_entry*>()));

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_size_t
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    using archive_size_t = decltype(
        ::archive_entry_size(std::declval<::archive_entry*>()));

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::archive_uid_t
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    using archive_uid_t = decltype(
        ::archive_entry_uid(std::declval<::archive_entry*>()));

}
