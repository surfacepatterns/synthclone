#include <boost/test/unit_test.hpp>

import std;

import synthclone.archive;
import synthclone.test;
import synthclone.util;

namespace {

    template<class I, class S, std::ranges::contiguous_range T>
    requires (
        std::same_as<
        //synthclone::explicitly_convertible_to<
            std::ranges::range_value_t<T>,
            std::byte
        >
    )
    void
    verify_archive_file_fragments(I& iter, S end, const T& expected_bytes)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_archive_file_fragments", iter, end, expected_bytes));

        std::span<const std::byte> expected_span(expected_bytes);

        for (; expected_span.size() != 0; ++iter) {
            synthclone::verify_ne(iter, end);

            const auto& element = *iter;
            const auto* fragment =
                std::get_if<synthclone::archive_file_fragment>(&element);
            synthclone::verify_ne(nullptr, fragment);

            auto fragment_size = fragment->size();
            synthclone::verify_le(fragment_size, expected_span.size());

            BOOST_REQUIRE(
                std::ranges::equal(
                    *fragment, expected_span.first(fragment_size)));

            expected_span = expected_span.last(
                expected_span.size() - fragment_size);
        }
    }

    std::ifstream
    verify_input_file_stream(const std::filesystem::path& path)
    {
        std::ifstream stream;
        stream.exceptions(std::ios_base::goodbit);
        stream.open(path);
        BOOST_REQUIRE(stream.is_open());
        return stream;
    }

    std::ofstream
    verify_output_file_stream(const std::filesystem::path& path)
    {
        std::ofstream stream;
        stream.exceptions(std::ios_base::goodbit);
        stream.open(path);
        BOOST_REQUIRE(stream.is_open());
        return stream;
    }

    template<class T>
    void
    verify_tar_header(
        const T& header,
        const std::filesystem::path& expected_path,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_header", header, expected_path,
                expected_modification_time, expected_gid, expected_group_name,
                expected_uid, expected_user_name, expected_permissions));

        synthclone::verify_eq(expected_path, header.path());
        synthclone::verify_eq(
            expected_modification_time, header.modification_time());
        synthclone::verify_eq(expected_gid, header.gid());
        synthclone::verify_eq(expected_group_name, header.group_name());
        synthclone::verify_eq(expected_uid, header.uid());
        synthclone::verify_eq(expected_user_name, header.user_name());
        synthclone::verify_eq(expected_permissions, header.permissions());
    }

    template<class T>
    void
    verify_tar_device_header(
        const T& header,
        const std::filesystem::path& expected_path,
        synthclone::archive_device_id expected_device_id,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_device_header", header, expected_path,
                expected_device_id, expected_modification_time, expected_gid,
                expected_group_name, expected_uid, expected_user_name,
                expected_permissions));

        verify_tar_header(
            header, expected_path, expected_modification_time, expected_gid,
            std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);

        const auto& device_id = header.device_id();
        synthclone::verify_eq(expected_device_id.major(), device_id.major());
        synthclone::verify_eq(expected_device_id.minor(), device_id.minor());
    }

    template<class T>
    void
    verify_tar_link_header(
        const T& header,
        const std::filesystem::path& expected_path,
        const std::filesystem::path& expected_link_path,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_link_header", header, expected_path,
                expected_link_path, expected_modification_time, expected_gid,
                expected_group_name, expected_uid, expected_user_name,
                expected_permissions));

        verify_tar_header(
            header, expected_path, expected_modification_time, expected_gid,
            std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);

        synthclone::verify_eq(expected_link_path, header.link_path());
    }

    void
    verify_tar_block_device_header(
        const synthclone::tar_block_device_header& header,
        const std::filesystem::path& expected_path,
        synthclone::archive_device_id expected_device_id,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_block_device_header", header, expected_path,
                expected_device_id, expected_modification_time, expected_gid,
                expected_group_name, expected_uid, expected_user_name,
                expected_permissions));

        verify_tar_device_header(
            header, expected_path, expected_device_id,
            expected_modification_time, expected_gid,
            std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);
    }

    void
    verify_tar_block_device_header(
        const synthclone::tar_element& element,
        const std::filesystem::path& expected_path,
        synthclone::archive_device_id expected_device_id,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_block_device_header", element, expected_path,
                expected_device_id, expected_modification_time, expected_gid,
                expected_group_name, expected_uid, expected_user_name,
                expected_permissions));

        auto* header =
            std::get_if<synthclone::tar_block_device_header>(&element);
        synthclone::verify_ne(nullptr, header);

        verify_tar_block_device_header(
            *header, expected_path, expected_device_id,
            expected_modification_time, expected_gid, expected_group_name,
            expected_uid, expected_user_name, expected_permissions);
    }

    void
    verify_tar_character_device_header(
        const synthclone::tar_character_device_header& header,
        const std::filesystem::path& expected_path,
        synthclone::archive_device_id expected_device_id,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_character_device_header", header, expected_path,
                expected_device_id, expected_modification_time, expected_gid,
                expected_group_name, expected_uid, expected_user_name,
                expected_permissions));

        verify_tar_device_header(
            header, expected_path, expected_device_id,
            expected_modification_time, expected_gid,
            std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);
    }

    void
    verify_tar_character_device_header(
        const synthclone::tar_element& element,
        const std::filesystem::path& expected_path,
        synthclone::archive_device_id expected_device_id,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_character_device_header", element, expected_path,
                expected_device_id, expected_modification_time, expected_gid,
                expected_group_name, expected_uid, expected_user_name,
                expected_permissions));

        auto* header =
            std::get_if<synthclone::tar_character_device_header>(&element);
        synthclone::verify_ne(nullptr, header);

        verify_tar_character_device_header(
            *header, expected_path, expected_device_id,
            expected_modification_time, expected_gid, expected_group_name,
            expected_uid, expected_user_name, expected_permissions);
    }

    void
    verify_tar_directory_header(
        const synthclone::tar_directory_header& header,
        const std::filesystem::path& expected_path,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_directory_header", header, expected_path,
                expected_modification_time, expected_gid, expected_group_name,
                expected_uid, expected_user_name, expected_permissions));

        verify_tar_header(
            header, expected_path, expected_modification_time, expected_gid,
            std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);
    }

    void
    verify_tar_directory_header(
        const synthclone::tar_element& element,
        const std::filesystem::path& expected_path,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_directory_header", element, expected_path,
                expected_modification_time, expected_gid, expected_group_name,
                expected_uid, expected_user_name, expected_permissions));

        auto* header = std::get_if<synthclone::tar_directory_header>(&element);
        synthclone::verify_ne(nullptr, header);

        verify_tar_directory_header(
            *header, expected_path, expected_modification_time, expected_gid,
            std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);
    }

    void
    verify_tar_fifo_header(
        const synthclone::tar_fifo_header& header,
        const std::filesystem::path& expected_path,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_fifo_header", header, expected_path,
                expected_modification_time, expected_gid, expected_group_name,
                expected_uid, expected_user_name, expected_permissions));

        verify_tar_header(
            header, expected_path, expected_modification_time, expected_gid,
            std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);
    }

    void
    verify_tar_fifo_header(
        const synthclone::tar_element& element,
        const std::filesystem::path& expected_path,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_fifo_header", element, expected_path,
                expected_modification_time, expected_gid, expected_group_name,
                expected_uid, expected_user_name, expected_permissions));

        auto* header = std::get_if<synthclone::tar_fifo_header>(&element);
        synthclone::verify_ne(nullptr, header);

        verify_tar_fifo_header(
            *header, expected_path, expected_modification_time, expected_gid,
            std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);
    }

    void
    verify_tar_file_header(
        const synthclone::tar_file_header& header,
        const std::filesystem::path& expected_path,
        synthclone::archive_size_t expected_size,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_file_header", header, expected_path, expected_size,
                expected_modification_time, expected_gid, expected_group_name,
                expected_uid, expected_user_name, expected_permissions));

        verify_tar_header(
            header, expected_path, expected_modification_time, expected_gid,
            std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);

        synthclone::verify_eq(expected_size, header.size());
    }

    void
    verify_tar_file_header(
        const synthclone::tar_element& element,
        const std::filesystem::path& expected_path,
        synthclone::archive_size_t expected_size,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_file_header", element, expected_path,
                expected_size, expected_modification_time, expected_gid,
                expected_group_name, expected_uid, expected_user_name,
                expected_permissions));

        auto* header = std::get_if<synthclone::tar_file_header>(&element);
        synthclone::verify_ne(nullptr, header);

        verify_tar_file_header(
            *header, expected_path, expected_size, expected_modification_time,
            expected_gid, std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);
    }

    void
    verify_tar_hard_link_header(
        const synthclone::tar_hard_link_header& header,
        const std::filesystem::path& expected_path,
        const std::filesystem::path& expected_link_path,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_hard_link_header", header, expected_path,
                expected_link_path, expected_modification_time, expected_gid,
                expected_group_name, expected_uid, expected_user_name,
                expected_permissions));

        verify_tar_link_header(
            header, expected_path, expected_link_path,
            expected_modification_time, expected_gid,
            std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);
    }

    void
    verify_tar_hard_link_header(
        const synthclone::tar_element& element,
        const std::filesystem::path& expected_path,
        const std::filesystem::path& expected_link_path,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_hard_link_header", element, expected_path,
                expected_link_path, expected_modification_time, expected_gid,
                expected_group_name, expected_uid, expected_user_name,
                expected_permissions));

        auto* header = std::get_if<synthclone::tar_hard_link_header>(&element);
        synthclone::verify_ne(nullptr, header);

        verify_tar_hard_link_header(
            *header, expected_path, expected_link_path,
            expected_modification_time, expected_gid, expected_group_name,
            expected_uid, expected_user_name, expected_permissions);
    }

    void
    verify_tar_symbolic_link_header(
        const synthclone::tar_symbolic_link_header& header,
        const std::filesystem::path& expected_path,
        const std::filesystem::path& expected_link_path,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_symbolic_link_header", header, expected_path,
                expected_link_path, expected_modification_time, expected_gid,
                expected_group_name, expected_uid, expected_user_name,
                expected_permissions));

        verify_tar_link_header(
            header, expected_path, expected_link_path,
            expected_modification_time, expected_gid,
            std::move(expected_group_name), expected_uid,
            std::move(expected_user_name), expected_permissions);
    }

    void
    verify_tar_symbolic_link_header(
        const synthclone::tar_element& element,
        const std::filesystem::path& expected_path,
        const std::filesystem::path& expected_link_path,
        std::chrono::nanoseconds expected_modification_time,
        synthclone::archive_gid_t expected_gid,
        std::string_view expected_group_name,
        synthclone::archive_uid_t expected_uid,
        std::string_view expected_user_name,
        std::filesystem::perms expected_permissions
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_tar_symbolic_link_header", element, expected_path,
                expected_link_path, expected_modification_time, expected_gid,
                expected_group_name, expected_uid, expected_user_name,
                expected_permissions));

        auto* header =
            std::get_if<synthclone::tar_symbolic_link_header>(&element);
        synthclone::verify_ne(nullptr, header);

        verify_tar_symbolic_link_header(
            *header, expected_path, expected_link_path,
            expected_modification_time, expected_gid, expected_group_name,
            expected_uid, expected_user_name, expected_permissions);
    }

    void
    test_basic_tar_istream_elements(const std::filesystem::path& path)
    {
        auto stream = verify_input_file_stream(path);
        synthclone::tar_istream_view elements(stream);

        auto end = elements.end();
        auto iter = elements.begin();

        synthclone::verify_ne(iter, end);
        verify_tar_block_device_header(
            *iter, "block-device", synthclone::archive_device_id(1, 2),
            std::chrono::nanoseconds(4000000000), 3, "block-group", 5,
            "block-user",
            std::filesystem::perms::group_read |
            std::filesystem::perms::others_read |
            std::filesystem::perms::owner_read);

        ++iter;
        synthclone::verify_ne(iter, end);
        verify_tar_character_device_header(
            *iter, "character-device", synthclone::archive_device_id(6, 7),
            std::chrono::nanoseconds(9000000000), 8, "character-group", 10,
            "character-user",
            std::filesystem::perms::group_write |
            std::filesystem::perms::others_write |
            std::filesystem::perms::owner_write);

        ++iter;
        synthclone::verify_ne(iter, end);
        verify_tar_directory_header(
            *iter, "directory/", std::chrono::nanoseconds(12000000000), 11,
            "directory-group", 13, "directory-user",
            std::filesystem::perms::group_exec |
            std::filesystem::perms::others_exec |
            std::filesystem::perms::owner_exec);

        ++iter;
        synthclone::verify_ne(iter, end);
        verify_tar_fifo_header(
            *iter, "fifo", std::chrono::nanoseconds(15000000000), 14,
            "fifo-group", 16, "fifo-user",
            std::filesystem::perms::set_gid | std::filesystem::perms::set_uid |
            std::filesystem::perms::sticky_bit);

        ++iter;
        synthclone::verify_ne(iter, end);
        verify_tar_file_header(
            *iter, "file", 19, std::chrono::nanoseconds(18000000000), 17,
            "file-group", 20, "file-user", std::filesystem::perms::none);

        ++iter;
        synthclone::verify_ne(iter, end);
        verify_archive_file_fragments(
            iter, end,
            std::ranges::to<std::vector<std::byte>>(
                std::views::transform(
                    std::views::iota(0, 19),
                    [](int n) { return static_cast<std::byte>(n); })));

        synthclone::verify_ne(iter, end);
        verify_tar_hard_link_header(
            *iter, "hard-link", "hard-link-path",
            std::chrono::nanoseconds(22000000000), 21, "hard-link-group", 23,
            "hard-link-user", std::filesystem::perms::none);

        ++iter;
        synthclone::verify_ne(iter, end);
        verify_tar_symbolic_link_header(
            *iter, "symbolic-link", "symbolic-link-path",
            std::chrono::nanoseconds(25000000000), 24, "symbolic-link-group",
            26, "symbolic-link-user", std::filesystem::perms::none);

        ++iter;
        synthclone::verify_eq(iter, end);
    }

}

BOOST_AUTO_TEST_SUITE(core)

BOOST_AUTO_TEST_CASE(basic_tar_block_device_headers)
{
    synthclone::archive_device_id device_id_1(4, 5);
    synthclone::tar_block_device_header header_1(
        {
            .path = "foo",
            .device_id = device_id_1
        });
    verify_tar_block_device_header(
        header_1, "foo", device_id_1, std::chrono::nanoseconds(0), 0, "", 0,
        "", std::filesystem::perms::none);

    synthclone::archive_device_id device_id_2(6, 7);
    synthclone::tar_block_device_header header_2(
        {
            .path = "foo",
            .device_id = device_id_2,
            .modification_time = std::chrono::nanoseconds(35),
            .gid = 36,
            .group_name = "some-group",
            .uid = 37,
            .user_name = "some-user",
            .permissions =
                std::filesystem::perms::owner_read |
                std::filesystem::perms::owner_write
        });
    verify_tar_block_device_header(
        header_2, "foo", device_id_2, std::chrono::nanoseconds(35), 36,
        "some-group", 37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    synthclone::tar_block_device_header header_3(header_1);
    verify_tar_block_device_header(
        header_3, "foo", device_id_1, std::chrono::nanoseconds(0), 0, "", 0,
        "", std::filesystem::perms::none);

    synthclone::tar_block_device_header header_4(std::move(header_2));
    verify_tar_block_device_header(
        header_4, "foo", device_id_2, std::chrono::nanoseconds(35), 36,
        "some-group", 37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_3 = header_4;
    verify_tar_block_device_header(
        header_3, "foo", device_id_2, std::chrono::nanoseconds(35), 36,
        "some-group", 37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_4 = std::move(header_1);
    verify_tar_block_device_header(
        header_4, "foo", device_id_1, std::chrono::nanoseconds(0), 0, "", 0,
        "", std::filesystem::perms::none);
}

BOOST_AUTO_TEST_CASE(basic_tar_character_device_headers)
{
    synthclone::archive_device_id device_id_1(4, 5);
    synthclone::tar_character_device_header header_1(
        {
            .path = "foo",
            .device_id = device_id_1
        });
    verify_tar_character_device_header(
        header_1, "foo", device_id_1, std::chrono::nanoseconds(0), 0, "", 0,
        "", std::filesystem::perms::none);

    synthclone::archive_device_id device_id_2(6, 7);
    synthclone::tar_character_device_header header_2(
        {
            .path = "foo",
            .device_id = device_id_2,
            .modification_time = std::chrono::nanoseconds(35),
            .gid = 36,
            .group_name = "some-group",
            .uid = 37,
            .user_name = "some-user",
            .permissions =
                std::filesystem::perms::owner_read |
                std::filesystem::perms::owner_write
        });
    verify_tar_character_device_header(
        header_2, "foo", device_id_2, std::chrono::nanoseconds(35), 36,
        "some-group", 37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    synthclone::tar_character_device_header header_3(header_1);
    verify_tar_character_device_header(
        header_3, "foo", device_id_1, std::chrono::nanoseconds(0), 0, "", 0,
        "", std::filesystem::perms::none);

    synthclone::tar_character_device_header header_4(std::move(header_2));
    verify_tar_character_device_header(
        header_4, "foo", device_id_2, std::chrono::nanoseconds(35), 36,
        "some-group", 37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_3 = header_4;
    verify_tar_character_device_header(
        header_3, "foo", device_id_2, std::chrono::nanoseconds(35), 36,
        "some-group", 37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_4 = std::move(header_1);
    verify_tar_character_device_header(
        header_4, "foo", device_id_1, std::chrono::nanoseconds(0), 0, "", 0,
        "", std::filesystem::perms::none);
}

BOOST_AUTO_TEST_CASE(basic_tar_directories)
{
    synthclone::tar_directory_header header_1({.path = "foo"});
    verify_tar_directory_header(
        header_1, "foo", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    synthclone::tar_directory_header header_2(
        {
            .path = "foo",
            .modification_time = std::chrono::nanoseconds(35),
            .gid = 36,
            .group_name = "some-group",
            .uid = 37,
            .user_name = "some-user",
            .permissions =
                std::filesystem::perms::owner_read |
                std::filesystem::perms::owner_write
        });
    verify_tar_directory_header(
        header_2, "foo", std::chrono::nanoseconds(35), 36, "some-group", 37,
        "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    synthclone::tar_directory_header header_3(header_1);
    verify_tar_directory_header(
        header_3, "foo", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    synthclone::tar_directory_header header_4(std::move(header_2));
    verify_tar_directory_header(
        header_4, "foo", std::chrono::nanoseconds(35), 36, "some-group", 37,
        "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_3 = header_4;
    verify_tar_directory_header(
        header_3, "foo", std::chrono::nanoseconds(35), 36, "some-group", 37,
        "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_4 = std::move(header_1);
    verify_tar_directory_header(
        header_4, "foo", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);
}

BOOST_AUTO_TEST_CASE(basic_tar_fifo_headers)
{
    synthclone::tar_fifo_header header_1({.path = "foo"});
    verify_tar_fifo_header(
        header_1, "foo", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    synthclone::tar_fifo_header header_2(
        {
            .path = "foo",
            .modification_time = std::chrono::nanoseconds(35),
            .gid = 36,
            .group_name = "some-group",
            .uid = 37,
            .user_name = "some-user",
            .permissions =
                std::filesystem::perms::owner_read |
                std::filesystem::perms::owner_write
        });
    verify_tar_fifo_header(
        header_2, "foo", std::chrono::nanoseconds(35), 36, "some-group", 37,
        "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    synthclone::tar_fifo_header header_3(header_1);
    verify_tar_fifo_header(
        header_3, "foo", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    synthclone::tar_fifo_header header_4(std::move(header_2));
    verify_tar_fifo_header(
        header_4, "foo", std::chrono::nanoseconds(35), 36, "some-group", 37,
        "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_3 = header_4;
    verify_tar_fifo_header(
        header_3, "foo", std::chrono::nanoseconds(35), 36, "some-group", 37,
        "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_4 = std::move(header_1);
    verify_tar_fifo_header(
        header_4, "foo", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);
}

BOOST_AUTO_TEST_CASE(basic_tar_file_headers)
{
    synthclone::tar_file_header header_1(
        {
            .path = "foo",
            .size = 23
        });
    verify_tar_file_header(
        header_1, "foo", 23, std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    synthclone::tar_file_header header_2(
        {
            .path = "foo",
            .size = 46,
            .modification_time = std::chrono::nanoseconds(35),
            .gid = 36,
            .group_name = "some-group",
            .uid = 37,
            .user_name = "some-user",
            .permissions =
                std::filesystem::perms::owner_read |
                std::filesystem::perms::owner_write
        });
    verify_tar_file_header(
        header_2, "foo", 46, std::chrono::nanoseconds(35), 36, "some-group",
        37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    synthclone::tar_file_header header_3(header_1);
    verify_tar_file_header(
        header_3, "foo", 23, std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    synthclone::tar_file_header header_4(std::move(header_2));
    verify_tar_file_header(
        header_4, "foo", 46, std::chrono::nanoseconds(35), 36, "some-group",
        37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_3 = header_4;
    verify_tar_file_header(
        header_3, "foo", 46, std::chrono::nanoseconds(35), 36, "some-group",
        37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_4 = std::move(header_1);
    verify_tar_file_header(
        header_4, "foo", 23, std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);
}

BOOST_AUTO_TEST_CASE(basic_tar_hard_link_headers)
{
    synthclone::tar_hard_link_header header_1(
        {
            .path = "foo",
            .link_path = "bar"
        });
    verify_tar_hard_link_header(
        header_1, "foo", "bar", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    synthclone::tar_hard_link_header header_2(
        {
            .path = "foo",
            .link_path = "baz",
            .modification_time = std::chrono::nanoseconds(35),
            .gid = 36,
            .group_name = "some-group",
            .uid = 37,
            .user_name = "some-user",
            .permissions =
                std::filesystem::perms::owner_read |
                std::filesystem::perms::owner_write
        });
    verify_tar_hard_link_header(
        header_2, "foo", "baz", std::chrono::nanoseconds(35), 36, "some-group",
        37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    synthclone::tar_hard_link_header header_3(header_1);
    verify_tar_hard_link_header(
        header_3, "foo", "bar", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    synthclone::tar_hard_link_header header_4(std::move(header_2));
    verify_tar_hard_link_header(
        header_4, "foo", "baz", std::chrono::nanoseconds(35), 36, "some-group",
        37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_3 = header_4;
    verify_tar_hard_link_header(
        header_3, "foo", "baz", std::chrono::nanoseconds(35), 36, "some-group",
        37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_4 = std::move(header_1);
    verify_tar_hard_link_header(
        header_4, "foo", "bar", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);
}

BOOST_AUTO_TEST_CASE(basic_tar_symbolic_link_headers)
{
    synthclone::tar_symbolic_link_header header_1(
        {
            .path = "foo",
            .link_path = "bar"
        });
    verify_tar_symbolic_link_header(
        header_1, "foo", "bar", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    synthclone::tar_symbolic_link_header header_2(
        {
            .path = "foo",
            .link_path = "baz",
            .modification_time = std::chrono::nanoseconds(35),
            .gid = 36,
            .group_name = "some-group",
            .uid = 37,
            .user_name = "some-user",
            .permissions =
                std::filesystem::perms::owner_read |
                std::filesystem::perms::owner_write
        });
    verify_tar_symbolic_link_header(
        header_2, "foo", "baz", std::chrono::nanoseconds(35), 36, "some-group",
        37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    synthclone::tar_symbolic_link_header header_3(header_1);
    verify_tar_symbolic_link_header(
        header_3, "foo", "bar", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    synthclone::tar_symbolic_link_header header_4(std::move(header_2));
    verify_tar_symbolic_link_header(
        header_4, "foo", "baz", std::chrono::nanoseconds(35), 36, "some-group",
        37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_3 = header_4;
    verify_tar_symbolic_link_header(
        header_3, "foo", "baz", std::chrono::nanoseconds(35), 36, "some-group",
        37, "some-user",
        std::filesystem::perms::owner_read |
        std::filesystem::perms::owner_write);

    header_4 = std::move(header_1);
    verify_tar_symbolic_link_header(
        header_4, "foo", "bar", std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);
}

BOOST_AUTO_TEST_CASE(tar_block_device_header_errors)
{
    synthclone::archive_device_id device_id(4, 5);

    synthclone::tar_block_device_header_init_args args {
        .path = "foo",
        .device_id = device_id,
        .uid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_block_device_header(std::move(args)),
        synthclone::verification_error);

    args = {
        .path = "foo",
        .device_id = device_id,
        .gid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_block_device_header(std::move(args)),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(tar_character_device_header_errors)
{
    synthclone::archive_device_id device_id(4, 5);

    synthclone::tar_character_device_header_init_args args {
        .path = "foo",
        .device_id = device_id,
        .uid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_character_device_header(std::move(args)),
        synthclone::verification_error);

    args = {
        .path = "foo",
        .device_id = device_id,
        .gid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_character_device_header(std::move(args)),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(tar_directory_header_errors)
{
    synthclone::tar_directory_header_init_args args {
        .path = "foo",
        .uid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_directory_header(std::move(args)),
        synthclone::verification_error);

    args = {
        .path = "foo",
        .gid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_directory_header(std::move(args)),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(tar_fifo_header_errors)
{
    synthclone::tar_fifo_header_init_args args {
        .path = "foo",
        .uid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_fifo_header(std::move(args)),
        synthclone::verification_error);

    args = {
        .path = "foo",
        .gid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_fifo_header(std::move(args)),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(tar_file_header_errors)
{
    synthclone::tar_file_header_init_args args {
        .path = "foo",
        .size = 0,
        .uid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_file_header(std::move(args)),
        synthclone::verification_error);

    args = {
        .path = "foo",
        .size = 0,
        .gid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_file_header(std::move(args)),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(tar_hard_link_header_errors)
{
    synthclone::tar_hard_link_header_init_args args {
        .path = "foo",
        .link_path = "bar",
        .uid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_hard_link_header(std::move(args)),
        synthclone::verification_error);

    args = {
        .path = "foo",
        .link_path = "bar",
        .gid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_hard_link_header(std::move(args)),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(tar_symbolic_link_header_errors)
{
    synthclone::tar_symbolic_link_header_init_args args {
        .path = "foo",
        .link_path = "bar",
        .uid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_symbolic_link_header(std::move(args)),
        synthclone::verification_error);

    args = {
        .path = "foo",
        .link_path = "bar",
        .gid = -1
    };
    BOOST_CHECK_THROW(
        synthclone::tar_symbolic_link_header(std::move(args)),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(basic_tar_istream_view_elements)
{
    test_basic_tar_istream_elements(
        "data/archives/tar/basic-header-types-gnu.tar");
    test_basic_tar_istream_elements(
        "data/archives/tar/basic-header-types-pax.tar");
    test_basic_tar_istream_elements(
        "data/archives/tar/basic-header-types-ustar.tar");
}

BOOST_AUTO_TEST_CASE(basic_tar_istream_view_files)
{
    auto stream = verify_input_file_stream(
        "data/archives/tar/basic-files.tar");
    synthclone::tar_istream_view elements(stream, 256);

    auto end = elements.end();

    auto iter = elements.begin();
    synthclone::verify_ne(iter, end);
    verify_tar_file_header(
        *iter, "file1", 8192, std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    ++iter;
    synthclone::verify_ne(iter, end);
    verify_archive_file_fragments(
        iter, end,
        std::ranges::to<std::vector<std::byte>>(
            std::views::transform(
                std::views::join(
                    std::views::repeat(std::views::iota(0, 256), 32)),
                [](int n) { return static_cast<std::byte>(n); })));

    synthclone::verify_ne(iter, end);
    verify_tar_file_header(
        *iter, "file2", 1024, std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    iter.advance_header();
    synthclone::verify_eq(iter, end);
}

BOOST_AUTO_TEST_CASE(tar_istream_view_errors)
{

    {
        auto stream = verify_input_file_stream(
            "data/archives/tar/error-truncated-first-directory.tar");
        synthclone::tar_istream_view elements(stream);

        BOOST_CHECK_THROW(elements.begin(), synthclone::archive_error);
    }

    {
        auto stream = verify_input_file_stream(
            "data/archives/tar/error-truncated-second-directory.tar");
        synthclone::tar_istream_view elements(stream);

        auto iter = elements.begin();
        synthclone::verify_ne(iter, elements.end());
        verify_tar_directory_header(
            *iter, "foo/", std::chrono::nanoseconds(0), 0, "", 0, "",
            std::filesystem::perms::none);

        BOOST_CHECK_THROW(++iter, synthclone::archive_error);
    }

    {
        auto stream = verify_input_file_stream(
            "data/archives/tar/error-truncated-second-directory.tar");
        synthclone::tar_istream_view elements(stream);

        auto iter = elements.begin();
        synthclone::verify_ne(iter, elements.end());
        verify_tar_directory_header(
            *iter, "foo/", std::chrono::nanoseconds(0), 0, "", 0, "",
            std::filesystem::perms::none);

        BOOST_CHECK_THROW(iter++, synthclone::archive_error);
    }

}

BOOST_AUTO_TEST_CASE(tar_istream_view_multi_volume)
{
    auto stream = verify_input_file_stream(
        "data/archives/tar/multi-volume.tar");
    synthclone::tar_istream_view elements(stream);

    auto end = elements.end();

    auto iter = elements.begin();
    synthclone::verify_ne(iter, end);
    verify_tar_file_header(
        *iter, "volume", 1, std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    ++iter;
    synthclone::verify_ne(iter, end);
    verify_archive_file_fragments(
        iter, end, std::vector<std::byte> {std::byte(0)});

    synthclone::verify_eq(iter, end);
}

BOOST_AUTO_TEST_CASE(basic_tar_ostream_iterator_elements)
{
    synthclone::temporary_file file;
    const auto& temp_path = file.path();
    auto stream = verify_output_file_stream(temp_path);

    {
        synthclone::tar_ostream_iterator iter(stream);

        iter = synthclone::tar_block_device_header(
            {
                .path = "block-device",
                .device_id = synthclone::archive_device_id(1, 2),
                .modification_time = std::chrono::nanoseconds(4000000000),
                .gid = 3,
                .group_name = "block-group",
                .uid = 5,
                .user_name = "block-user",
                .permissions =
                    std::filesystem::perms::group_read |
                    std::filesystem::perms::others_read |
                    std::filesystem::perms::owner_read
            });

        iter = synthclone::tar_character_device_header(
            {
                .path = "character-device",
                .device_id = synthclone::archive_device_id(6, 7),
                .modification_time = std::chrono::nanoseconds(9000000000),
                .gid = 8,
                .group_name = "character-group",
                .uid = 10,
                .user_name = "character-user",
                .permissions =
                    std::filesystem::perms::group_write |
                    std::filesystem::perms::others_write |
                    std::filesystem::perms::owner_write
            });

        iter = synthclone::tar_directory_header(
            {
                .path = "directory",
                .modification_time = std::chrono::nanoseconds(12000000000),
                .gid = 11,
                .group_name = "directory-group",
                .uid = 13,
                .user_name = "directory-user",
                .permissions =
                    std::filesystem::perms::group_exec |
                    std::filesystem::perms::others_exec |
                    std::filesystem::perms::owner_exec
            });

        iter = synthclone::tar_fifo_header(
            {
                .path = "fifo",
                .modification_time = std::chrono::nanoseconds(15000000000),
                .gid = 14,
                .group_name = "fifo-group",
                .uid = 16,
                .user_name = "fifo-user",
                .permissions =
                    std::filesystem::perms::set_gid |
                    std::filesystem::perms::set_uid |
                    std::filesystem::perms::sticky_bit
            });

        iter = synthclone::tar_file_header(
            {
                .path = "file",
                .size = 19,
                .modification_time = std::chrono::nanoseconds(18000000000),
                .gid = 17,
                .group_name = "file-group",
                .uid = 20,
                .user_name = "file-user",
                .permissions = std::filesystem::perms::none
            });

        std::array<std::byte, 19> file_bytes {
            std::byte(0),
            std::byte(1),
            std::byte(2),
            std::byte(3),
            std::byte(4),
            std::byte(5),
            std::byte(6),
            std::byte(7),
            std::byte(8),
            std::byte(9),
            std::byte(10),
            std::byte(11),
            std::byte(12),
            std::byte(13),
            std::byte(14),
            std::byte(15),
            std::byte(16),
            std::byte(17),
            std::byte(18)
        };
        iter = synthclone::archive_file_fragment(file_bytes);

        iter = synthclone::tar_hard_link_header(
            {
                .path = "hard-link",
                .link_path = "hard-link-path",
                .modification_time = std::chrono::nanoseconds(22000000000),
                .gid = 21,
                .group_name = "hard-link-group",
                .uid = 23,
                .user_name = "hard-link-user",
                .permissions = std::filesystem::perms::none
            });

        iter = synthclone::tar_symbolic_link_header(
            {
                .path = "symbolic-link",
                .link_path = "symbolic-link-path",
                .modification_time = std::chrono::nanoseconds(25000000000),
                .gid = 24,
                .group_name = "symbolic-link-group",
                .uid = 26,
                .user_name = "symbolic-link-user",
                .permissions = std::filesystem::perms::none
            });
    }

    stream.close();

    test_basic_tar_istream_elements(temp_path);
}

BOOST_AUTO_TEST_CASE(tar_ostream_iterator_copying)
{
    auto input_stream = verify_input_file_stream(
        "data/archives/tar/basic-header-types-ustar.tar");
    synthclone::tar_istream_view elements(input_stream);

    synthclone::temporary_file file;
    const auto& path = file.path();
    auto output_stream = verify_output_file_stream(path);

    {
        synthclone::tar_ostream_iterator iter(output_stream);
        std::ranges::copy(elements, std::move(iter));
    }

    output_stream.close();

    test_basic_tar_istream_elements(path);
}

BOOST_AUTO_TEST_CASE(tar_ostream_iterator_errors)
{

    {
        synthclone::temporary_file file;
        auto stream = verify_output_file_stream(file.path());
        synthclone::tar_ostream_iterator iter(stream);

        auto b = static_cast<std::byte>(1);
        synthclone::archive_file_fragment fragment(&b, 1);

        BOOST_CHECK_THROW(iter = fragment, synthclone::verification_error);
    }

    {
        synthclone::temporary_file file;
        auto stream = verify_output_file_stream(file.path());
        synthclone::tar_ostream_iterator iter(stream);

        iter = synthclone::tar_directory_header({.path = "foo"});

        auto b = static_cast<std::byte>(1);
        synthclone::archive_file_fragment fragment(&b, 1);

        BOOST_CHECK_THROW(iter = fragment, synthclone::verification_error);
    }

    {
        synthclone::temporary_file file;
        auto stream = verify_output_file_stream(file.path());
        synthclone::tar_ostream_iterator iter(stream);

        iter = synthclone::tar_file_header({.path = "foo", .size = 1});

        synthclone::tar_directory_header header({.path = "bar"});

        BOOST_CHECK_THROW(iter = header, synthclone::verification_error);
    }

    {
        synthclone::temporary_file file;
        auto stream = verify_output_file_stream(file.path());
        synthclone::tar_ostream_iterator iter(stream);

        iter = synthclone::tar_file_header({.path = "foo", .size = 1});

        std::array<std::byte, 2> bytes {std::byte(1), std::byte(0)};
        synthclone::archive_file_fragment fragment(bytes);

        BOOST_CHECK_THROW(iter = fragment, synthclone::verification_error);
    }

}

BOOST_AUTO_TEST_CASE(tar_ostream_iterator_files)
{
    synthclone::temporary_file file;
    const auto& path = file.path();
    auto output_stream = verify_output_file_stream(path);

    std::filesystem::path foo_bar_file_path("data/files/foo-bar.txt");

    {
        synthclone::tar_ostream_iterator iter(output_stream);
        std::array<std::byte, 4096> buffer;
        synthclone::add_file(
            synthclone::tar_minimal_header, iter, foo_bar_file_path,
            "foo-bar.txt", std::span(buffer));
        synthclone::add_file(
            synthclone::tar_minimal_header, iter, foo_bar_file_path,
            "foo-bar-1.txt", std::span(buffer.data(), 1));
        synthclone::add_file(
            synthclone::tar_minimal_header, iter, "data/files/empty.txt",
            "empty.txt", std::span(buffer));
    }

    auto stream = verify_input_file_stream(path);
    synthclone::tar_istream_view elements(stream);

    auto expected_stream = synthclone::open_ifstream(foo_bar_file_path);
    std::array<std::byte, 4104> expected_buffer;

    auto count = synthclone::read(expected_stream, std::span(expected_buffer));
    synthclone::verify_eq(4104, count);

    auto end = elements.end();

    auto iter = elements.begin();
    synthclone::verify_ne(iter, end);
    verify_tar_file_header(
        *iter, "foo-bar.txt", std::filesystem::file_size(foo_bar_file_path),
        std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    ++iter;
    synthclone::verify_ne(iter, end);
    verify_archive_file_fragments(iter, end, expected_buffer);

    verify_tar_file_header(
        *iter, "foo-bar-1.txt", std::filesystem::file_size(foo_bar_file_path),
        std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    ++iter;
    synthclone::verify_ne(iter, end);
    verify_archive_file_fragments(iter, end, expected_buffer);

    verify_tar_file_header(
        *iter, "empty.txt", 0, std::chrono::nanoseconds(0), 0, "", 0, "",
        std::filesystem::perms::none);

    ++iter;
    synthclone::verify_eq(iter, end);
}

BOOST_AUTO_TEST_CASE(tar_ostream_iterator_noops)
{
    synthclone::temporary_file file;
    const auto& path = file.path();
    auto output_stream = verify_output_file_stream(path);

    {
        synthclone::tar_ostream_iterator iter(output_stream);

        synthclone::verify_eq(std::addressof(iter), std::addressof(++iter));
        synthclone::verify_eq(std::addressof(iter), std::addressof(*iter));

        iter++;
    }

    output_stream.close();

    auto input_stream = verify_input_file_stream(path);
    synthclone::tar_istream_view elements(input_stream);

    synthclone::verify_eq(elements.begin(), elements.end());
}

BOOST_AUTO_TEST_SUITE_END()
