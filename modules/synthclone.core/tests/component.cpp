#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;

namespace {

    void
    verify_component_metadata(
        const synthclone::component_metadata& metadata,
        const synthclone::metadata_element& identifier,
        const synthclone::metadata_element& version,
        const std::optional<synthclone::metadata_element>& title,
        const std::optional<synthclone::metadata_url>& url,
        const std::optional<synthclone::metadata_element>& license,
        const std::vector<synthclone::metadata_element>& creators,
        const std::vector<synthclone::metadata_element>& contributors,
        const synthclone::metadata_text& description,
        const std::vector<synthclone::metadata_element>& category
    )
    {
        synthclone::verify_eq(identifier, metadata.identifier());
        synthclone::verify_eq(version, metadata.version());
        synthclone::verify_eq(title, metadata.title());
        synthclone::verify_eq(url, metadata.url());
        synthclone::verify_eq(license, metadata.license());

        const auto& metadata_creators = metadata.creators();
        BOOST_CHECK_EQUAL_COLLECTIONS(
            creators.begin(), creators.end(), metadata_creators.begin(),
            metadata_creators.end());

        const auto& metadata_contributors = metadata.contributors();
        BOOST_CHECK_EQUAL_COLLECTIONS(
            contributors.begin(), contributors.end(),
            metadata_contributors.begin(), metadata_contributors.end());

        synthclone::verify_eq(description, metadata.description());

        const auto& metadata_category = metadata.category();
        BOOST_CHECK_EQUAL_COLLECTIONS(
            category.begin(), category.end(), metadata_category.begin(),
            metadata_category.end());
    }

}

BOOST_AUTO_TEST_SUITE(component)

BOOST_AUTO_TEST_CASE(metadata)
{
    synthclone::component_metadata metadata_1(
        {
            .identifier = "some.identifier",
            .version = "some-version"
        });

    verify_component_metadata(
        metadata_1, "some.identifier", "some-version", std::nullopt,
        std::nullopt, std::nullopt, {}, {}, "", {});

    synthclone::component_metadata metadata_2(
        {
            .identifier = "some.other.identifier",
            .version = "1.2.3",
            .title = "title",
            .url = "http://host/path",
            .license = "license",
            .creators = {"me", "myself", "I"},
            .contributors = {"him", "her", "them"},
            .description = "description",
            .category = {"component", "category"}
        });
    verify_component_metadata(
        metadata_2, "some.other.identifier", "1.2.3", "title",
        "http://host/path", "license", {"me", "myself", "I"},
        {"him", "her", "them"}, "description", {"component", "category"});

    synthclone::verify_ne(metadata_1, metadata_2);

    synthclone::component_metadata metadata_3(metadata_1);
    synthclone::verify_eq(metadata_1, metadata_3);

    synthclone::component_metadata metadata_4(std::move(metadata_1));
    synthclone::verify_eq(metadata_3, metadata_4);

    metadata_1 = metadata_2;
    synthclone::verify_eq(metadata_1, metadata_2);

    metadata_4 = std::move(metadata_1);
    synthclone::verify_eq(metadata_2, metadata_4);

    synthclone::component_metadata metadata_5(
        {
            .identifier = "some.other.identifier",
            .version = "1.2.3",
            .title = "title",
            .url = "http://host/path",
            .license = "license",
            .creators = {"me", "myself", "I"},
            .contributors = {"him", "her", "them"},
            .description = "description",
            .category = {"different-component", "different-category"}
        });
    synthclone::verify_ne(metadata_2, metadata_5);
}

BOOST_AUTO_TEST_SUITE_END()
