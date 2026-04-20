module;

#include <archive.h>
#include <archive_entry.h>

constexpr inline int AE_IFBLK_ = AE_IFBLK;
#undef AE_IFBLK

constexpr inline int AE_IFCHR_ = AE_IFCHR;
#undef AE_IFCHR

constexpr inline int AE_IFDIR_ = AE_IFDIR;
#undef AE_IFDIR

constexpr inline int AE_IFIFO_ = AE_IFIFO;
#undef AE_IFIFO

constexpr inline int AE_IFLNK_ = AE_IFLNK;
#undef AE_IFLNK

constexpr inline int AE_IFREG_ = AE_IFREG;
#undef AE_IFREG

constexpr inline int ARCHIVE_EOF_ = ARCHIVE_EOF;
#undef ARCHIVE_EOF

constexpr inline int ARCHIVE_OK_ = ARCHIVE_OK;
#undef ARCHIVE_OK

constexpr inline int ARCHIVE_FORMAT_TAR_PAX_RESTRICTED_ =
    ARCHIVE_FORMAT_TAR_PAX_RESTRICTED;
#undef ARCHIVE_FORMAT_TAR_PAX_RESTRICTED

constexpr inline int ARCHIVE_FORMAT_TAR_ = ARCHIVE_FORMAT_TAR;
#undef ARCHIVE_FORMAT_TAR

export module synthclone.external.archive;

export {

    constexpr inline int AE_IFBLK = AE_IFBLK_;
    constexpr inline int AE_IFCHR = AE_IFCHR_;
    constexpr inline int AE_IFDIR = AE_IFDIR_;
    constexpr inline int AE_IFIFO = AE_IFIFO_;
    constexpr inline int AE_IFLNK = AE_IFLNK_;
    constexpr inline int AE_IFREG = AE_IFREG_;

    constexpr inline int ARCHIVE_EOF = ARCHIVE_EOF_;
    constexpr inline int ARCHIVE_OK = ARCHIVE_OK_;

    constexpr inline int ARCHIVE_FORMAT_TAR = ARCHIVE_FORMAT_TAR_;
    constexpr inline int ARCHIVE_FORMAT_TAR_PAX_RESTRICTED =
        ARCHIVE_FORMAT_TAR_PAX_RESTRICTED_;

    using ::archive;
    using ::archive_entry;

    using ::la_int64_t;
    using ::la_ssize_t;

    using ::archive_error_string;
    using ::archive_free;
    using ::archive_read_data_block;
    using ::archive_read_data_skip;
    using ::archive_read_new;
    using ::archive_read_next_header2;
    using ::archive_read_open2;
    using ::archive_read_set_format;
    using ::archive_read_support_format_tar;
    using ::archive_set_error;
    using ::archive_write_data;
    using ::archive_write_header;
    using ::archive_write_new;
    using ::archive_write_open2;
    using ::archive_write_set_format;

    using ::archive_entry_clone;
    using ::archive_entry_copy_gname;
    using ::archive_entry_copy_hardlink;
    using ::archive_entry_copy_hardlink_w;
    using ::archive_entry_copy_pathname;
    using ::archive_entry_copy_pathname_w;
    using ::archive_entry_copy_symlink;
    using ::archive_entry_copy_symlink_w;
    using ::archive_entry_copy_uname;
    using ::archive_entry_filetype;
    using ::archive_entry_filetype_is_set;
    using ::archive_entry_free;
    using ::archive_entry_gid;
    using ::archive_entry_gid_is_set;
    using ::archive_entry_gname;
    using ::archive_entry_hardlink;
    using ::archive_entry_hardlink_is_set;
    using ::archive_entry_hardlink_w;
    using ::archive_entry_mtime;
    using ::archive_entry_mtime_is_set;
    using ::archive_entry_mtime_nsec;
    using ::archive_entry_new;
    using ::archive_entry_pathname;
    using ::archive_entry_pathname_w;
    using ::archive_entry_perm;
    using ::archive_entry_perm_is_set;
    using ::archive_entry_rdev_is_set;
    using ::archive_entry_rdevmajor;
    using ::archive_entry_rdevminor;
    using ::archive_entry_set_filetype;
    using ::archive_entry_set_gid;
    using ::archive_entry_set_mtime;
    using ::archive_entry_set_perm;
    using ::archive_entry_set_rdevmajor;
    using ::archive_entry_set_rdevminor;
    using ::archive_entry_set_size;
    using ::archive_entry_set_uid;
    using ::archive_entry_size;
    using ::archive_entry_size_is_set;
    using ::archive_entry_symlink;
    using ::archive_entry_symlink_w;
    using ::archive_entry_uid;
    using ::archive_entry_uid_is_set;
    using ::archive_entry_uname;

}
