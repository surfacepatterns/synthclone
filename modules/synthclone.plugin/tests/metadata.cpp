#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.plugin;
import synthclone.test;

namespace {

    std::string
    generate_expected_version()
    {
        return std::format(
            "{0}.{1}.{2}", synthclone::project_version_major,
            synthclone::project_version_minor,
            synthclone::project_version_patch);
    }

    void
    verify_metadata(
        const synthclone::metadata_init_args& args,
        const synthclone::metadata_element& identifier,
        const std::optional<synthclone::metadata_element>& title,
        const std::vector<synthclone::metadata_element>& category,
        const std::vector<synthclone::metadata_element>& creators,
        const std::vector<synthclone::metadata_element>& contributors,
        const synthclone::metadata_text& description
    )
    {
        synthclone::verify_eq(identifier, args.identifier);
        synthclone::verify_eq(generate_expected_version(), args.version);
        synthclone::verify_eq(title, args.title);
        synthclone::verify_eq(synthclone::project_url, args.url);
        synthclone::verify_eq("BSD-3-Clause", args.license);

        const auto& metadata_category = args.category;
        BOOST_CHECK_EQUAL_COLLECTIONS(
            category.begin(), category.end(), metadata_category.begin(),
            metadata_category.end());

        const auto& metadata_creators = args.creators;
        BOOST_CHECK_EQUAL_COLLECTIONS(
            creators.begin(), creators.end(), metadata_creators.begin(),
            metadata_creators.end());

        const auto& metadata_contributors = args.contributors;
        BOOST_CHECK_EQUAL_COLLECTIONS(
            contributors.begin(), contributors.end(),
            metadata_contributors.begin(), metadata_contributors.end());

        synthclone::verify_eq(description, args.description);
    }

}

BOOST_AUTO_TEST_SUITE(metadata)

BOOST_AUTO_TEST_CASE(simple)
{
    verify_metadata(
        synthclone::generate_simple_metadata(
            {
                .identifier = "some.identifier",
                .title = "Some Title"
            }),
        "some.identifier", "Some Title", {}, {}, {}, "");
    verify_metadata(
        synthclone::generate_simple_metadata(
            {
                .identifier = "some.other.identifier",
                .title = "title",
                .category = {"component", "category"},
                .creators = {"me", "myself", "I"},
                .contributors = {"him", "her", "them"},
                .description = "description"
            }),
        "some.other.identifier", "title", {"component", "category"},
        {"me", "myself", "I"}, {"him", "her", "them"}, "description");
}

BOOST_AUTO_TEST_SUITE_END()
