#include <boost/test/unit_test.hpp>

import std;

import synthclone.test;
import synthclone.util;

namespace {

    std::string
    to_string(const std::filesystem::filesystem_error& e)
    {
        const auto& code = e.code();
        return std::format(
            "{{code={{{0}, {1:?}}}, path1={2:?}, path2={3:?}, message={4:?}}}",
            code.value(), code.message(), e.path1().string(),
            e.path2().string(), e.what());
    }

    void
    verify_directory(
        const std::filesystem::path& path,
        const std::filesystem::path& expected_parent_path
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_directory", path, expected_parent_path));

        synthclone::verify_eq(path.parent_path(), expected_parent_path);
        BOOST_CHECK(std::filesystem::is_directory(path));
    }

    void
    verify_file(
        const std::filesystem::path& path,
        const std::filesystem::path& expected_parent_path
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_file", path, expected_parent_path));

        synthclone::verify_eq(path.parent_path(), expected_parent_path);
        BOOST_CHECK(std::filesystem::is_regular_file(path));
    }

}

BOOST_AUTO_TEST_SUITE(filesystem)

// XXX: It'd be nice to have death tests for exceptions that can be raised by
// `temporary_directory` and `temporary_file` destructors.

BOOST_AUTO_TEST_CASE(temporary_directory_construct_errors)
{
    synthclone::temporary_directory directory;
    auto bad_path = directory.path() / "this-parent-certainly-will-not-exist";

    BOOST_CHECK_EXCEPTION(
        synthclone::temporary_directory{bad_path},
        std::filesystem::filesystem_error,
        [&bad_path](const std::filesystem::filesystem_error& e) -> bool {

            BOOST_TEST_INFO_SCOPE(
                (
                    std::format(
                        "temporary_directory_construct_errors::[lambda]({0})",
                        to_string(e))
                ));

            const auto& code = e.code();
            return (e.path1() == bad_path) &&
                (
                    code.value() ==
                    static_cast<int>(std::errc::no_such_file_or_directory)
                ) &&
                (code.category() == std::generic_category()) &&
                e.path2().empty();
        });
}

BOOST_AUTO_TEST_CASE(temporary_directory_double_reset)
{
    synthclone::temporary_directory directory;
    std::filesystem::path directory_path(directory.path());
    verify_directory(directory_path, std::filesystem::temp_directory_path());

    directory.reset();
    BOOST_CHECK(! std::filesystem::exists(directory_path));

    directory.reset();
}

BOOST_AUTO_TEST_CASE(temporary_directory_move_assign)
{
    synthclone::temporary_directory directory;
    std::filesystem::path directory_path = directory.path();
    verify_directory(directory_path, std::filesystem::temp_directory_path());

    synthclone::temporary_directory directory_2;
    std::filesystem::path directory_path_2 = directory_2.path();
    verify_directory(directory_path_2, std::filesystem::temp_directory_path());

    directory_2 = std::move(directory);

    verify_directory(directory_path, std::filesystem::temp_directory_path());
    BOOST_CHECK(! std::filesystem::exists(directory_path_2));
}

BOOST_AUTO_TEST_CASE(temporary_directory_move_construct)
{
    synthclone::temporary_directory directory;
    std::filesystem::path directory_path(directory.path());
    verify_directory(directory_path, std::filesystem::temp_directory_path());

    {
        synthclone::temporary_directory directory_2(std::move(directory));
        BOOST_CHECK_EQUAL(directory_2.path(), directory_path);
        verify_directory(
            directory_path, std::filesystem::temp_directory_path());
    }

    BOOST_CHECK(! std::filesystem::exists(directory_path));
}

BOOST_AUTO_TEST_CASE(temporary_directory_release)
{
    synthclone::temporary_directory parent_directory;
    const auto& parent_path = parent_directory.path();
    std::filesystem::path temp_dir_path;

    {
        synthclone::temporary_directory directory(parent_path);
        temp_dir_path = directory.path();
        verify_directory(temp_dir_path, parent_path);
        directory.release();
    }

    verify_directory(temp_dir_path, parent_path);
}

BOOST_AUTO_TEST_CASE(temporary_directory_reset_error)
{
    synthclone::temporary_directory parent_directory;
    const auto& parent_path = parent_directory.path();

    synthclone::temporary_directory directory(parent_path);
    std::filesystem::path directory_path(directory.path());
    verify_directory(directory_path, parent_path);

    parent_directory.reset();
    BOOST_REQUIRE(! std::filesystem::exists(directory_path));

    BOOST_CHECK_EXCEPTION(
        directory.reset(), std::filesystem::filesystem_error,
        [&directory_path](const std::filesystem::filesystem_error& e) -> bool {

            BOOST_TEST_INFO_SCOPE(
                (
                    std::format(
                        "temporary_directory_reset_error::[lambda]({0})",
                        to_string(e))
                ));

            const auto& code = e.code();
            return (e.path1() == directory_path) &&
                (
                    code.value() ==
                    static_cast<int>(std::errc::no_such_file_or_directory)
                ) &&
                (code.category() == std::generic_category()) &&
                e.path2().empty();
        });

    directory.release();
}

BOOST_AUTO_TEST_CASE(temporary_directory_typical_lifetime)
{
    std::filesystem::path temp_dir_path;

    {
        synthclone::temporary_directory directory;
        temp_dir_path = directory.path();
        verify_directory(
            temp_dir_path, std::filesystem::temp_directory_path());
    }

    BOOST_CHECK(! std::filesystem::exists(temp_dir_path));
}

BOOST_AUTO_TEST_CASE(temporary_directory_typical_reset)
{
    synthclone::temporary_directory directory;
    std::filesystem::path directory_path(directory.path());
    verify_directory(directory_path, std::filesystem::temp_directory_path());

    directory.reset();
    BOOST_CHECK(! std::filesystem::exists(directory_path));
}

BOOST_AUTO_TEST_CASE(temporary_file_construct_errors)
{
    synthclone::temporary_directory directory;
    auto bad_path = directory.path() / "this-parent-certainly-will-not-exist";

    BOOST_CHECK_EXCEPTION(
        synthclone::temporary_file{bad_path},
        std::filesystem::filesystem_error,
        [&bad_path](const std::filesystem::filesystem_error& e) -> bool {

            BOOST_TEST_INFO_SCOPE(
                (
                    std::format(
                        "temporary_directory_reset_error::[lambda]({0})",
                        to_string(e))
                ));

            const auto& code = e.code();
            return (e.path1() == bad_path) &&
                (
                    code.value() ==
                    static_cast<int>(std::errc::no_such_file_or_directory)
                ) &&
                (code.category() == std::generic_category()) &&
                e.path2().empty();
        });
}

BOOST_AUTO_TEST_CASE(temporary_file_double_reset)
{
    synthclone::temporary_file file;
    std::filesystem::path file_path(file.path());
    verify_file(file_path, std::filesystem::temp_directory_path());

    file.reset();
    BOOST_CHECK(! std::filesystem::exists(file_path));

    file.reset();
}

BOOST_AUTO_TEST_CASE(temporary_file_move_assign)
{
    synthclone::temporary_file file;
    std::filesystem::path file_path = file.path();
    verify_file(file_path, std::filesystem::temp_directory_path());

    synthclone::temporary_file file_2;
    std::filesystem::path file_path_2 = file_2.path();
    verify_file(file_path_2, std::filesystem::temp_directory_path());

    file_2 = std::move(file);

    verify_file(file_path, std::filesystem::temp_directory_path());
    BOOST_CHECK(! std::filesystem::exists(file_path_2));
}

BOOST_AUTO_TEST_CASE(temporary_file_move_construct)
{
    synthclone::temporary_file file;
    std::filesystem::path file_path(file.path());
    verify_file(file_path, std::filesystem::temp_directory_path());

    {
        synthclone::temporary_file file_2(std::move(file));
        synthclone::verify_eq(file_2.path(), file_path);
        verify_file(file_path, std::filesystem::temp_directory_path());
    }

    BOOST_CHECK(! std::filesystem::exists(file_path));
}

BOOST_AUTO_TEST_CASE(temporary_file_release)
{
    synthclone::temporary_directory parent_directory;
    const auto& parent_path = parent_directory.path();
    std::filesystem::path temp_file_path;

    {
        synthclone::temporary_file file(parent_path);
        temp_file_path = file.path();
        verify_file(temp_file_path, parent_path);
        file.release();
    }

    verify_file(temp_file_path, parent_path);
}

BOOST_AUTO_TEST_CASE(temporary_file_reset_error)
{
    synthclone::temporary_directory parent_directory;
    const auto& parent_path = parent_directory.path();

    synthclone::temporary_file file(parent_path);
    std::filesystem::path file_path(file.path());
    verify_file(file_path, parent_path);

    parent_directory.reset();
    BOOST_REQUIRE(! std::filesystem::exists(file_path));

    BOOST_CHECK_EXCEPTION(
        file.reset(), std::filesystem::filesystem_error,
        [&file_path](const std::filesystem::filesystem_error& e) -> bool {

            BOOST_TEST_INFO_SCOPE(
                (
                    std::format(
                        "temporary_directory_reset_error::[lambda]({0})",
                        to_string(e))
                ));

            const auto& code = e.code();
            return (e.path1() == file_path) &&
                (
                    code.value() ==
                    static_cast<int>(std::errc::no_such_file_or_directory)
                ) &&
                (code.category() == std::generic_category()) &&
                e.path2().empty();
        });

    file.release();
}

BOOST_AUTO_TEST_CASE(temporary_file_typical_lifetime)
{
    std::filesystem::path temp_file_path;

    {
        synthclone::temporary_file file;
        temp_file_path = file.path();
        verify_file(temp_file_path, std::filesystem::temp_directory_path());
    }

    BOOST_CHECK(! std::filesystem::exists(temp_file_path));
}

BOOST_AUTO_TEST_CASE(temporary_file_typical_reset)
{
    synthclone::temporary_file file;
    std::filesystem::path file_path(file.path());
    verify_file(file_path, std::filesystem::temp_directory_path());

    file.reset();
    BOOST_CHECK(! std::filesystem::exists(file_path));
}

BOOST_AUTO_TEST_SUITE_END()
