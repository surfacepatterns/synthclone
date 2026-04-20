#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;

BOOST_AUTO_TEST_SUITE(metadata)

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
