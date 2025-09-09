#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;

namespace {

    void
    verify_metadata(
        const synthclone::metadata& metadata,
        const synthclone::metadata_element& identifier,
        const synthclone::metadata_element& version,
        const std::optional<synthclone::metadata_element>& title,
        const std::optional<synthclone::metadata_url>& url,
        const std::optional<synthclone::metadata_element>& license,
        const std::vector<synthclone::metadata_element>& category,
        const std::vector<synthclone::metadata_element>& creators,
        const std::vector<synthclone::metadata_element>& contributors,
        const synthclone::metadata_text& description
    )
    {
        synthclone::verify_eq(identifier, metadata.identifier());
        synthclone::verify_eq(version, metadata.version());
        synthclone::verify_eq(title, metadata.title());
        synthclone::verify_eq(url, metadata.url());
        synthclone::verify_eq(license, metadata.license());

        const auto& metadata_category = metadata.category();
        BOOST_CHECK_EQUAL_COLLECTIONS(
            category.begin(), category.end(), metadata_category.begin(),
            metadata_category.end());

        const auto& metadata_creators = metadata.creators();
        BOOST_CHECK_EQUAL_COLLECTIONS(
            creators.begin(), creators.end(), metadata_creators.begin(),
            metadata_creators.end());

        const auto& metadata_contributors = metadata.contributors();
        BOOST_CHECK_EQUAL_COLLECTIONS(
            contributors.begin(), contributors.end(),
            metadata_contributors.begin(), metadata_contributors.end());

        synthclone::verify_eq(description, metadata.description());
    }

}

BOOST_AUTO_TEST_SUITE(metadata)

BOOST_AUTO_TEST_CASE(metadata)
{
    synthclone::metadata metadata_1(
        {
            .identifier = "some.identifier",
            .version = "some-version"
        });

    verify_metadata(
        metadata_1, "some.identifier", "some-version", std::nullopt,
        std::nullopt, std::nullopt, {}, {}, {}, "");

    synthclone::metadata metadata_2(
        {
            .identifier = "some.other.identifier",
            .version = "1.2.3",
            .title = "title",
            .url = "http://host/path",
            .license = "license",
            .category = {"component", "category"},
            .creators = {"me", "myself", "I"},
            .contributors = {"him", "her", "them"},
            .description = "description"
        });

    verify_metadata(
        metadata_2, "some.other.identifier", "1.2.3", "title",
        "http://host/path", "license", {"component", "category"},
        {"me", "myself", "I"}, {"him", "her", "them"}, "description");

    synthclone::verify_ne(metadata_1, metadata_2);

    synthclone::metadata metadata_3(metadata_1);
    synthclone::verify_eq(metadata_1, metadata_3);

    synthclone::metadata metadata_4(std::move(metadata_1));
    synthclone::verify_eq(metadata_3, metadata_4);

    metadata_1 = metadata_2;
    synthclone::verify_eq(metadata_1, metadata_2);

    metadata_4 = std::move(metadata_1);
    synthclone::verify_eq(metadata_2, metadata_4);
}

BOOST_AUTO_TEST_CASE(metadata_elements)
{
    synthclone::metadata_element element_1("foo");
    synthclone::verify_eq(element_1, "foo");

    synthclone::metadata_element element_2("bar");
    synthclone::verify_eq(element_2, "bar");

    synthclone::verify_ne(element_1, element_2);

    synthclone::metadata_element element_3(element_1);
    synthclone::verify_eq(element_1, element_3);

    synthclone::metadata_element element_4(std::move(element_1));
    synthclone::verify_eq(element_3, element_4);

    element_1 = element_2;
    synthclone::verify_eq(element_1, element_2);

    element_4 = std::move(element_1);
    synthclone::verify_eq(element_2, element_4);

    BOOST_CHECK_THROW(
        synthclone::metadata_element(""), synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_element("foo\n"), synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_element("foo\v"), synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_element("foo\f"), synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_element("foo\r"), synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_element("foo\U00000085"),
        synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_element("foo\U00002028"),
        synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_element("foo\U00002029"),
        synthclone::metadata_error);
}

BOOST_AUTO_TEST_CASE(metadata_urls)
{
    synthclone::metadata_url url_1("http://somewhere.dev/yup");
    synthclone::verify_eq(url_1, "http://somewhere.dev/yup");

    synthclone::metadata_url url_2("https://you.know.com/uh-huh");
    synthclone::verify_eq(url_2, "https://you.know.com/uh-huh");

    synthclone::verify_ne(url_1, url_2);

    synthclone::metadata_url url_3(url_1);
    synthclone::verify_eq(url_1, url_3);

    synthclone::metadata_url url_4(std::move(url_1));
    synthclone::verify_eq(url_3, url_4);

    url_1 = url_2;
    synthclone::verify_eq(url_1, url_2);

    url_4 = std::move(url_1);
    synthclone::verify_eq(url_2, url_4);

    BOOST_CHECK_THROW(
        synthclone::metadata_url(""), synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_url("//authority/path"),
        synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_url("scheme:path"), synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_url("scheme:/path"), synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_url("bad$@scheme://authority/path"),
        synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_url("scheme://bad-authorit%Mf/"),
        synthclone::metadata_error);
    BOOST_CHECK_THROW(
        synthclone::metadata_url("scheme://authority/bad-path%tt"),
        synthclone::metadata_error);
}

BOOST_AUTO_TEST_SUITE_END()
