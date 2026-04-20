export module synthclone.archive:tar;

import std;

import synthclone.external.archive;
import synthclone.util;

import :core;
import :internal;

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_directory_header_init_args
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    constexpr archive_gid_t default_tar_gid = 0;

    constexpr std::string default_tar_group_name("");

    constexpr std::chrono::nanoseconds default_tar_modification_time(0);

    constexpr archive_gid_t default_tar_uid = 0;

    constexpr std::string default_tar_user_name("");

    constexpr std::filesystem::perms default_tar_permissions =
        std::filesystem::perms::none;

    struct tar_filesystem_header_init_args final {

        std::filesystem::path path;

        std::chrono::nanoseconds modification_time =
            default_tar_modification_time;

        archive_gid_t gid = default_tar_gid;

        std::string group_name = default_tar_group_name;

        archive_uid_t uid = default_tar_uid;

        std::string user_name = default_tar_user_name;

        std::filesystem::perms permissions = default_tar_permissions;

    };

    export
    using tar_directory_header_init_args = tar_filesystem_header_init_args;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_directory_header
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    template<unsigned int Type, class T>
    archive_entry_ptr
    make_tar_entry(const T& args)
    {
        auto entry = make_archive_entry();
        set_archive_entry_file_type<Type>(entry);

        auto gid = args.gid;
        verify(gid >= 0, "gid {0} is less than 0", gid);
        set_archive_entry_gid(entry, gid);

        auto uid = args.uid;
        verify(uid >= 0, "uid {0} is less than 0", uid);
        set_archive_entry_uid(entry, uid);

        set_archive_entry_path(entry, args.path);
        set_archive_entry_modification_time(entry, args.modification_time);
        set_archive_entry_group_name(entry, args.group_name);
        set_archive_entry_user_name(entry, args.user_name);
        set_archive_entry_permissions(entry, args.permissions);

        return entry;
    }

    class tar_filesystem_header {

    public:

        inline
        archive_gid_t
        gid() const
        {
            return get_archive_entry_gid(entry_);
        }

        inline
        std::string_view
        group_name() const
        {
            return get_archive_entry_group_name(entry_);
        }

        inline
        std::chrono::nanoseconds
        modification_time() const
        {
            return get_archive_entry_modification_time(entry_);
        }

        constexpr
        const std::filesystem::path&
        path() const noexcept
        {
            return path_;
        }

        inline
        std::filesystem::perms
        permissions() const
        {
            return get_archive_entry_permissions(entry_);
        }

        inline
        archive_uid_t
        uid() const
        {
            return get_archive_entry_uid(entry_);
        }

        inline
        std::string_view
        user_name() const
        {
            return get_archive_entry_user_name(entry_);
        }

    protected:

        tar_filesystem_header(tar_filesystem_header&&) = default;

        inline
        tar_filesystem_header(const tar_filesystem_header& entry):
            path_(entry.path_),
            entry_(clone_archive_entry(entry.entry_))
        {
            // empty
        }

        explicit
        tar_filesystem_header(archive_entry_ptr&& entry):
            path_(get_archive_entry_path(entry)),
            entry_(std::move(entry))
        {
            // empty
        }

        inline
        tar_filesystem_header(
            archive_entry_ptr&& entry,
            std::filesystem::path&& path
        ) noexcept:
            path_(std::move(path)),
            entry_(std::move(entry))
        {
            // empty
        }

        tar_filesystem_header&
        operator=(tar_filesystem_header&& entry) = default;

        std::filesystem::path path_;
        archive_entry_ptr entry_;

    private:

        tar_filesystem_header&
        operator=(const tar_filesystem_header&) = delete;

    };

    export
    class tar_directory_header;

    template<std::derived_from<std::ostream> T>
    class tar_ostream_iterator_impl;

    tar_directory_header
    make_tar_directory_header(archive_entry_ptr&& entry);

    archive_entry_ptr
    make_tar_directory_entry(const tar_directory_header_init_args& args)
    {
        return make_tar_entry<::AE_IFDIR>(args);
    }

    export
    class tar_directory_header final: public tar_filesystem_header {

    public:

        tar_directory_header(tar_directory_header&&) = default;

        tar_directory_header(const tar_directory_header&) = default;

        inline
        tar_directory_header(tar_directory_header_init_args args):
            tar_filesystem_header(
                make_tar_directory_entry(args), std::move(args.path))
        {
            // empty
        }

        tar_directory_header&
        operator=(tar_directory_header&&) = default;

        inline
        tar_directory_header&
        operator=(const tar_directory_header& directory)
        {
            *this = tar_directory_header(directory);
            return *this;
        }

    private:

        template<std::derived_from<std::ostream> T>
        friend
        class tar_ostream_iterator_impl;

        friend
        tar_directory_header
        make_tar_directory_header(archive_entry_ptr&& entry);

        explicit
        tar_directory_header(archive_entry_ptr&& entry):
            tar_filesystem_header(std::move(entry))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_fifo_header_init_args
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    using tar_fifo_header_init_args = tar_filesystem_header_init_args;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_fifo_header
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    archive_entry_ptr
    make_tar_fifo_entry(const tar_fifo_header_init_args& args)
    {
        return make_tar_entry<::AE_IFIFO>(args);
    }

    export
    class tar_fifo_header;

    tar_fifo_header
    make_tar_fifo_header(archive_entry_ptr&& entry);

    export
    class tar_fifo_header final: public tar_filesystem_header {

    public:

        tar_fifo_header(tar_fifo_header&&) = default;

        tar_fifo_header(const tar_fifo_header&) = default;

        inline
        tar_fifo_header(tar_fifo_header_init_args args):
            tar_filesystem_header(
                make_tar_fifo_entry(args), std::move(args.path))
        {
            // empty
        }

        tar_fifo_header&
        operator=(tar_fifo_header&&) = default;

        inline
        tar_fifo_header&
        operator=(const tar_fifo_header& fifo)
        {
            *this = tar_fifo_header(fifo);
            return *this;
        }

    private:

        template<std::derived_from<std::ostream> T>
        friend
        class tar_ostream_iterator_impl;

        friend
        tar_fifo_header
        make_tar_fifo_header(archive_entry_ptr&& entry);

        explicit
        tar_fifo_header(archive_entry_ptr&& entry):
            tar_filesystem_header(std::move(entry))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_character_device_header_init_args
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    struct tar_device_header_init_args final {

        std::filesystem::path path;

        archive_device_id device_id;

        std::chrono::nanoseconds modification_time =
            default_tar_modification_time;

        archive_gid_t gid = default_tar_gid;

        std::string group_name = default_tar_group_name;

        archive_uid_t uid = default_tar_uid;

        std::string user_name = default_tar_user_name;

        std::filesystem::perms permissions = default_tar_permissions;

    };

    export
    using tar_character_device_header_init_args = tar_device_header_init_args;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_character_device_header
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    class tar_device_header: public tar_filesystem_header {

    public:

        constexpr
        archive_device_id
        device_id() const noexcept
        {
            return device_id_;
        }

    protected:

        tar_device_header(tar_device_header&&) = default;

        tar_device_header(const tar_device_header&) = default;

        explicit
        tar_device_header(archive_entry_ptr&& entry):
            tar_filesystem_header(std::move(entry)),
            device_id_(get_archive_entry_device_id(entry_))
        {
            // empty
        }

        tar_device_header(
            archive_entry_ptr&& entry,
            std::filesystem::path&& path,
            archive_device_id&& device_id
        ):
            tar_filesystem_header(std::move(entry), std::move(path)),
            device_id_(std::move(device_id))
        {
            // empty
        }

        tar_device_header&
        operator=(tar_device_header&&) = default;

        archive_device_id device_id_;

    };

    template<unsigned int Type, class T>
    archive_entry_ptr
    make_tar_device_entry(const T& args)
    {
        auto entry = make_tar_entry<Type>(args);
        set_archive_entry_device_id(entry, args.device_id);
        return entry;
    }

    archive_entry_ptr
    make_tar_character_device_entry(
        const tar_character_device_header_init_args& args
    )
    {
        return make_tar_device_entry<::AE_IFCHR>(args);
    }

    export
    class tar_character_device_header;

    tar_character_device_header
    make_tar_character_device_header(archive_entry_ptr&& entry);

    export
    class tar_character_device_header final: public tar_device_header {

    public:

        tar_character_device_header(tar_character_device_header&&) = default;

        tar_character_device_header(
            const tar_character_device_header&
        ) = default;

        inline
        tar_character_device_header(
            tar_character_device_header_init_args args
        ):
            tar_device_header(
                make_tar_character_device_entry(args), std::move(args.path),
                std::move(args.device_id))
        {
            // empty
        }

        tar_character_device_header&
        operator=(tar_character_device_header&&) = default;

        inline
        tar_character_device_header&
        operator=(const tar_character_device_header& device)
        {
            *this = tar_character_device_header(device);
            return *this;
        }

    private:

        template<std::derived_from<std::ostream> T>
        friend
        class tar_ostream_iterator_impl;

        friend
        tar_character_device_header
        make_tar_character_device_header(archive_entry_ptr&& entry);

        explicit
        tar_character_device_header(archive_entry_ptr&& entry):
            tar_device_header(std::move(entry))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_block_device_header_init_args
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    using tar_block_device_header_init_args = tar_device_header_init_args;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_block_device_header
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    archive_entry_ptr
    make_tar_block_device_entry(const tar_block_device_header_init_args& args)
    {
        return make_tar_device_entry<::AE_IFBLK>(args);
    }

    export
    class tar_block_device_header;

    tar_block_device_header
    make_tar_block_device_header(archive_entry_ptr&& entry);

    export
    class tar_block_device_header final: public tar_device_header {

    public:

        tar_block_device_header(tar_block_device_header&&) = default;

        tar_block_device_header(const tar_block_device_header&) = default;

        inline
        tar_block_device_header(tar_block_device_header_init_args args):
            tar_device_header(
                make_tar_block_device_entry(args), std::move(args.path),
                std::move(args.device_id))
        {
            // empty
        }

        tar_block_device_header&
        operator=(tar_block_device_header&&) = default;

        inline
        tar_block_device_header&
        operator=(const tar_block_device_header& device)
        {
            *this = tar_block_device_header(device);
            return *this;
        }

    private:

        template<std::derived_from<std::ostream> T>
        friend
        class tar_ostream_iterator_impl;

        friend
        tar_block_device_header
        make_tar_block_device_header(archive_entry_ptr&& entry);

        explicit
        tar_block_device_header(archive_entry_ptr&& entry):
            tar_device_header(std::move(entry))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_symbolic_link_header_init_args
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    struct tar_link_header_init_args final {

        std::filesystem::path path;

        std::filesystem::path link_path;

        std::chrono::nanoseconds modification_time =
            default_tar_modification_time;

        archive_gid_t gid = default_tar_gid;

        std::string group_name = default_tar_group_name;

        archive_uid_t uid = default_tar_uid;

        std::string user_name = default_tar_user_name;

        std::filesystem::perms permissions = default_tar_permissions;

    };

    export
    using tar_symbolic_link_header_init_args = tar_link_header_init_args;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_symbolic_link_header
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    class tar_link_header: public tar_filesystem_header {

    public:

        constexpr
        const std::filesystem::path&
        link_path() const noexcept
        {
            return link_path_;
        }

    protected:

        tar_link_header(tar_link_header&&) = default;

        tar_link_header(const tar_link_header&) = default;

        tar_link_header(
            archive_entry_ptr&& entry,
            std::filesystem::path&& link_path
        ):
            tar_filesystem_header(std::move(entry)),
            link_path_(std::move(link_path))
        {
            // empty
        }

        tar_link_header(
            archive_entry_ptr&& entry,
            std::filesystem::path&& path,
            std::filesystem::path&& link_path
        ):
            tar_filesystem_header(std::move(entry), std::move(path)),
            link_path_(std::move(link_path))
        {
            // empty
        }

        tar_link_header&
        operator=(tar_link_header&&) = default;

    private:

        std::filesystem::path link_path_;

    };

    archive_entry_ptr
    make_tar_symbolic_link_entry(
        const tar_symbolic_link_header_init_args& args
    )
    {
        auto entry = make_tar_entry<::AE_IFLNK>(args);
        set_archive_entry_symbolic_link_path(entry, args.link_path);
        return entry;
    }

    export
    class tar_symbolic_link_header;

    tar_symbolic_link_header
    make_tar_symbolic_link_header(archive_entry_ptr&& entry);

    export
    class tar_symbolic_link_header final: public tar_link_header {

    public:

        tar_symbolic_link_header(tar_symbolic_link_header&&) = default;

        tar_symbolic_link_header(const tar_symbolic_link_header&) = default;

        inline
        tar_symbolic_link_header(tar_symbolic_link_header_init_args args):
            tar_link_header(
                make_tar_symbolic_link_entry(args), std::move(args.path),
                std::move(args.link_path))
        {
            // empty
        }

        tar_symbolic_link_header&
        operator=(tar_symbolic_link_header&&) = default;

        inline
        tar_symbolic_link_header&
        operator=(const tar_symbolic_link_header& link)
        {
            *this = tar_symbolic_link_header(link);
            return *this;
        }

     private:

        template<std::derived_from<std::ostream> T>
        friend
        class tar_ostream_iterator_impl;

        friend
        tar_symbolic_link_header
        make_tar_symbolic_link_header(archive_entry_ptr&& entry);

        explicit
        tar_symbolic_link_header(archive_entry_ptr&& entry):
            tar_link_header(
                std::move(entry), get_archive_entry_symbolic_link_path(entry))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_hard_link_header_init_args
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    using tar_hard_link_header_init_args = tar_link_header_init_args;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_hard_link_header
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    archive_entry_ptr
    make_tar_hard_link_entry(const tar_hard_link_header_init_args& args)
    {
        auto entry = make_tar_entry<0>(args);
        set_archive_entry_hard_link_path(entry, args.link_path);
        return entry;
    }

    export
    class tar_hard_link_header;

    tar_hard_link_header
    make_tar_hard_link_header(archive_entry_ptr&& entry);

    export
    class tar_hard_link_header final: public tar_link_header {

    public:

        tar_hard_link_header(tar_hard_link_header&&) = default;

        tar_hard_link_header(const tar_hard_link_header&) = default;

        inline
        tar_hard_link_header(tar_hard_link_header_init_args args):
            tar_link_header(
                make_tar_hard_link_entry(args), std::move(args.path),
                std::move(args.link_path))
        {
            // empty
        }

        tar_hard_link_header&
        operator=(tar_hard_link_header&&) = default;

        inline
        tar_hard_link_header&
        operator=(const tar_hard_link_header& link)
        {
            *this = tar_hard_link_header(link);
            return *this;
        }

     private:

        template<std::derived_from<std::ostream> T>
        friend
        class tar_ostream_iterator_impl;

        friend
        tar_hard_link_header
        make_tar_hard_link_header(archive_entry_ptr&& entry);

        explicit
        tar_hard_link_header(archive_entry_ptr&& entry):
            tar_link_header(
                std::move(entry), get_archive_entry_hard_link_path(entry))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_file_header_init_args
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    struct tar_file_header_init_args final {

        std::filesystem::path path;

        archive_size_t size;

        std::chrono::nanoseconds modification_time =
            default_tar_modification_time;

        archive_gid_t gid = default_tar_gid;

        std::string group_name = default_tar_group_name;

        archive_uid_t uid = default_tar_uid;

        std::string user_name = default_tar_user_name;

        std::filesystem::perms permissions = default_tar_permissions;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_file_header
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    archive_entry_ptr
    make_tar_file_entry(const tar_file_header_init_args& args)
    {
        auto entry = make_tar_entry<::AE_IFREG>(args);
        set_archive_entry_size(entry, args.size);
        return entry;
    }

    export
    class tar_file_header;

    tar_file_header
    make_tar_file_header(archive_entry_ptr&& entry);

    export
    class tar_file_header final: public tar_filesystem_header {

    public:

        tar_file_header(tar_file_header&&) = default;

        tar_file_header(const tar_file_header&) = default;

        inline
        tar_file_header(tar_file_header_init_args args):
            tar_filesystem_header(
                make_tar_file_entry(args), std::move(args.path)),
            size_(args.size)
        {
            // empty
        }

        tar_file_header&
        operator=(tar_file_header&&) = default;

        inline
        tar_file_header&
        operator=(const tar_file_header& file)
        {
            *this = tar_file_header(file);
            return *this;
        }

        constexpr
        archive_size_t
        size() const noexcept
        {
            return size_;
        }

    private:

        template<std::derived_from<std::ostream> T>
        friend
        class tar_ostream_iterator_impl;

        friend
        tar_file_header
        make_tar_file_header(archive_entry_ptr&& entry);

        explicit
        tar_file_header(archive_entry_ptr&& entry):
            tar_filesystem_header(std::move(entry)),
            size_(get_archive_entry_size(entry_))
        {
            // empty
        }

        archive_size_t size_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_element
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    using tar_element = std::variant<
        tar_directory_header,
        tar_fifo_header,
        tar_character_device_header,
        tar_block_device_header,
        tar_symbolic_link_header,
        tar_hard_link_header,
        tar_file_header,
        archive_file_fragment
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_istream_view
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    tar_block_device_header
    make_tar_block_device_header(archive_entry_ptr&& entry)
    {
        return tar_block_device_header(std::move(entry));
    }

    tar_character_device_header
    make_tar_character_device_header(archive_entry_ptr&& entry)
    {
        return tar_character_device_header(std::move(entry));
    }

    tar_directory_header
    make_tar_directory_header(archive_entry_ptr&& entry)
    {
        return tar_directory_header(std::move(entry));
    }

    tar_fifo_header
    make_tar_fifo_header(archive_entry_ptr&& entry)
    {
        return tar_fifo_header(std::move(entry));
    }

    tar_file_header
    make_tar_file_header(archive_entry_ptr&& entry)
    {
        return tar_file_header(std::move(entry));
    }

    tar_hard_link_header
    make_tar_hard_link_header(archive_entry_ptr&& entry)
    {
        return tar_hard_link_header(std::move(entry));
    }

    tar_symbolic_link_header
    make_tar_symbolic_link_header(archive_entry_ptr&& entry)
    {
        return tar_symbolic_link_header(std::move(entry));
    }

    std::pair<std::optional<tar_element>, archive_size_t>
    read_tar_header(const archive_ptr& archive)
    {
        auto entry = make_archive_entry();
        if (! read_archive_entry(archive, entry)) [[unlikely]] {
            return {std::nullopt, 0};
        }

        // `libarchive` sets the file type for hard links by looking at the
        // upper bits of the `mode`.  AFAICT, this isn't correct, as `tar`
        // doesn't make use of the upper bits of the `mode` (they're always set
        // to `000`); so, the file type is *normally* going to be set to 0,
        // *but* it could be set to whatever the upper bits of the `mode` are
        // set to in the hard link entry.
        if (is_archive_entry_hard_link(entry)) {
            return {make_tar_hard_link_header(std::move(entry)), 0};
        }

        auto entry_type = get_archive_entry_file_type(entry);
        switch (entry_type) {
        case ::AE_IFLNK:
            return {make_tar_symbolic_link_header(std::move(entry)), 0};
        case ::AE_IFCHR:
            return {make_tar_character_device_header(std::move(entry)), 0};
        case ::AE_IFBLK:
            return {make_tar_block_device_header(std::move(entry)), 0};
        case ::AE_IFDIR:
            return {make_tar_directory_header(std::move(entry)), 0};
        case ::AE_IFIFO:
            return {make_tar_fifo_header(std::move(entry)), 0};
        case ::AE_IFREG:
            [[fallthrough]];
        [[unlikely]] default:
            // AFAICT, `libarchive` converts all unknown types and types it
            // doesn't explicitly deal with to `AE_IFREG` (as recommended by
            // POSIX) *except* type 'GNU "Multi-volume"'.  If we get here, it's
            // likely because of that format, though future changes to
            // `libarchive` could obviously change things.
            //
            // For now, we're just going to convert any unknown types to
            // regular files too.
            ;
        }

        auto file = make_tar_file_header(std::move(entry));
        return {std::move(file), file.size()};
    }

    export
    template<std::derived_from<std::istream> T>
    class tar_istream_view;

    template<std::derived_from<std::istream> T>
    class tar_istream_iterator final: private noncopyable {

    public:

        using difference_type = std::ptrdiff_t;

        using value_type = tar_element;

        tar_element
        operator*() const
        {
            verify(
                element_ != std::nullopt,
                "iterator does not reference an element");
            return *element_;
        }

        tar_istream_iterator&
        operator++()
        {
            if (pending_byte_count_ != 0) {
                auto content = read_archive_file_fragment(archive_);
                auto size = content.size();

                assume(
                    size > 0, "size is 0, but there are {0} pending bytes",
                    pending_byte_count_);
                assume(
                    size <= pending_byte_count_,
                    "size ({0}) is greater than pending byte count ({1})",
                    size, pending_byte_count_);

                element_ = content;
                pending_byte_count_ -= size;
            } else {
                advance_header();
            }
            return *this;
        }

        void
        operator++(int)
        {
            ++(*this);
        }

        friend
        bool
        operator==(
            const tar_istream_iterator& iter,
            std::default_sentinel_t
        ) noexcept
        {
            return iter.element_ == std::nullopt;
        }

        void
        advance_header()
        {
            auto pair = read_tar_header(archive_);
            element_ = std::move(pair.first);
            pending_byte_count_ = pair.second;
        }

    private:

        friend
        class tar_istream_view<T>;

        tar_istream_iterator(
            archive_ptr&& archive,
            archive_stream_reader_ops_ptr<T>&& reader_ops,
            std::pair<std::optional<tar_element>, archive_size_t>&& p
        ) noexcept:
            element_(std::move(p.first)),
            reader_ops_(std::move(reader_ops)),
            archive_(std::move(archive)),
            pending_byte_count_(p.second)
        {
            // empty
        }

        explicit
        tar_istream_iterator(
            archive_ptr&& archive,
            archive_stream_reader_ops_ptr<T>&& reader_ops
        ):
            tar_istream_iterator(
                std::move(archive), std::move(reader_ops),
                read_tar_header(archive))
        {
            // empty
        }

        std::optional<tar_element> element_;
        archive_stream_reader_ops_ptr<T> reader_ops_;
        archive_ptr archive_;
        archive_size_t pending_byte_count_;

    };

    archive_ptr
    make_tar_istream_archive()
    {
        auto archive = make_archive_reader();
        set_archive_reader_format(archive, ::ARCHIVE_FORMAT_TAR);
        return archive;
    }

    export
    template<std::derived_from<std::istream> T>
    class tar_istream_view final:
        public std::ranges::view_interface<tar_istream_view<T>> {

    public:

        using iterator = tar_istream_iterator<T>;

        using sentinel = std::default_sentinel_t;

        inline
        tar_istream_view(T& stream, std::size_t buffer_size):
            tar_istream_view(stream, make_tar_istream_archive(), buffer_size)
        {
            // empty
        }

        inline explicit
        tar_istream_view(T& stream):
            tar_istream_view(stream, make_tar_istream_archive())
        {
            // empty
        }

        tar_istream_iterator<T>
        begin()
        {
            verify_archive(archive_);
            return tar_istream_iterator<T>(
                std::move(archive_), std::move(reader_ops_));
        }

        std::default_sentinel_t
        end() const noexcept
        {
            return {};
        }

    private:

        tar_istream_view(T& stream, archive_ptr&& archive):
            reader_ops_(
                std::make_unique<archive_stream_reader_ops<T>>(
                    stream, archive)),
            archive_(std::move(archive))
        {
            // empty
        }

        tar_istream_view(
            T& stream,
            archive_ptr&& archive,
            std::size_t buffer_size
        ):
            reader_ops_(
                std::make_unique<archive_stream_reader_ops<T>>(
                    stream, archive, buffer_size)),
            archive_(std::move(archive))
        {
            // empty
        }

        // It's important for the archive to be destroyed before the reader
        // because the archive close operation may invoke I/O operations.
        archive_stream_reader_ops_ptr<T> reader_ops_;
        archive_ptr archive_;

    };

    export
    template<std::derived_from<std::istream> T>
    tar_istream_view(T&) -> tar_istream_view<T>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_ostream_iterator
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    archive_ptr
    make_tar_ostream_archive()
    {
        auto archive = make_archive_writer();
        set_archive_writer_format(
            archive, ::ARCHIVE_FORMAT_TAR_PAX_RESTRICTED);
        return archive;
    }

    template<std::derived_from<std::ostream> T>
    class tar_ostream_iterator_impl final: private noncopyable {

    public:

        inline explicit
        tar_ostream_iterator_impl(T& stream):
            tar_ostream_iterator_impl(stream, make_tar_ostream_archive())
        {
            // empty
        }

        template<std::derived_from<tar_filesystem_header> H>
        void
        operator()(const H& header)
        {
            verify(
                pending_byte_count_ == 0,
                "can't write header because there are {0} bytes pending",
                pending_byte_count_);

            write_archive_entry(archive_, header.entry_);
        }

        void
        operator()(const tar_file_header& header)
        {
            verify(
                pending_byte_count_ == 0,
                "can't write header because there are {0} bytes pending",
                pending_byte_count_);

            write_archive_entry(archive_, header.entry_);
            pending_byte_count_ = header.size();
        }

        void
        operator()(const archive_file_fragment& fragment)
        {
            auto size = fragment.size();
            verify(size != 0, "the given archive file fragment is empty");
            verify(
                size <= pending_byte_count_,
                "archive file fragment is {0} bytes in length, but there are "
                "{1} bytes pending",
                size, pending_byte_count_);

            write_archive_file_fragment(archive_, fragment);
            pending_byte_count_ -= size;
        }

    private:

        tar_ostream_iterator_impl(T& stream, archive_ptr&& archive):
            writer_ops_(
                std::make_unique<archive_stream_writer_ops<T>>(
                    stream, archive)),
            archive_(std::move(archive)),
            pending_byte_count_(0)
        {
            // empty
        }

        // It's important for the archive to be destroyed before the writer
        // because the archive close operation may invoke I/O operations.
        archive_stream_writer_ops_ptr<T> writer_ops_;
        archive_ptr archive_;
        archive_size_t pending_byte_count_;

    };

    // The `tar_ostream_iterator` API mimics the `ostream_iterator` API and the
    // APIs of other output iterators in the standard library.  I can't say I
    // *like* the API - it has a bunch of silly no-ops to satisfy the
    // constraints of `std::output_iterator` - but it works and it's a known,
    // accepted pattern.

    export
    template<std::derived_from<std::ostream> T>
    class tar_ostream_iterator final: private noncopyable {

    public:

        using difference_type = std::ptrdiff_t;

        using value_type = tar_element;

        inline explicit
        tar_ostream_iterator(T& stream):
            impl_(stream)
        {
            // empty
        }

        tar_ostream_iterator&
        operator=(const tar_element& element)
        {
            std::visit(impl_, element);
            return *this;
        }

        tar_ostream_iterator&
        operator=(archive_file_fragment fragment)
        {
            impl_(fragment);
            return *this;
        }

        tar_ostream_iterator&
        operator=(const tar_block_device_header& header)
        {
            impl_(header);
            return *this;
        }

        tar_ostream_iterator&
        operator=(const tar_character_device_header& header)
        {
            impl_(header);
            return *this;
        }

        tar_ostream_iterator&
        operator=(const tar_directory_header& header)
        {
            impl_(header);
            return *this;
        }

        tar_ostream_iterator&
        operator=(const tar_fifo_header& header)
        {
            impl_(header);
            return *this;
        }

        tar_ostream_iterator&
        operator=(const tar_file_header& header)
        {
            impl_(header);
            return *this;
        }

        tar_ostream_iterator&
        operator=(const tar_hard_link_header& header)
        {
            impl_(header);
            return *this;
        }

        tar_ostream_iterator&
        operator=(const tar_symbolic_link_header& header)
        {
            impl_(header);
            return *this;
        }

        tar_ostream_iterator&
        operator*() noexcept
        {
            return *this;
        }

        tar_ostream_iterator&
        operator++()
        {
            return *this;
        }

        void
        operator++(int)
        {
            // empty
        }

    private:

        tar_ostream_iterator_impl<T> impl_;

    };

    export
    template<std::derived_from<std::ostream> T>
    tar_ostream_iterator(T&) -> tar_ostream_iterator<T>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::tar_minimal_header_t
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    struct tar_minimal_header_t {

        explicit
        tar_minimal_header_t() = default;

    };

    export
    constexpr inline
    tar_minimal_header_t tar_minimal_header{};

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::add_file()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<std::derived_from<std::ostream> T>
    void
    add_file(
        tar_minimal_header_t,
        tar_ostream_iterator<T>& iter,
        const std::filesystem::path& source_path,
        const std::filesystem::path& destination_path,
        std::span<std::byte> buffer
    )
    {
        auto file_size = std::filesystem::file_size(source_path);

        *iter = tar_file_header(
            {
                .path = destination_path,
                .size = static_cast<archive_size_t>(file_size)
            });
        ++iter;

        if (file_size != 0) {
            auto stream = open_ifstream(
                source_path, std::ios::binary | std::ios::in);
            auto buffer_size = buffer.size();

            for (
                ;
                file_size >= buffer_size;
                ++iter, file_size -= buffer_size
            ) {
                auto bytes_read = read(stream, buffer);
                if (bytes_read != buffer_size) [[unlikely]] {
                    throw archive_error(
                        std::format(
                            "{0:?}: file size changed on disk",
                            source_path.string()));
                }
                *iter = archive_file_fragment(buffer);
            }

            auto bytes_read = read(stream, buffer);
            if (bytes_read != file_size) [[unlikely]] {
                throw archive_error(
                    std::format(
                        "{0:?}: file size changed on disk",
                        source_path.string()));
            }
            if (bytes_read != 0) {
                *iter = archive_file_fragment(buffer.first(bytes_read));
                ++iter;
            }
        }
    }

}
