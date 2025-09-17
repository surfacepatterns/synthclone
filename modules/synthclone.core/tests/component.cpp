#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;
import synthclone.util;

BOOST_AUTO_TEST_SUITE(component)

BOOST_AUTO_TEST_CASE(progress_message)
{
    synthclone::component_progress_message message_1(0.5);
    synthclone::verify_eq(0.5, message_1.progress());

    synthclone::component_progress_message message_2(0.75);
    synthclone::verify_eq(0.75, message_2.progress());

    synthclone::component_progress_message message_3(message_1);
    synthclone::verify_eq(0.5, message_3.progress());

    message_1 = message_2;
    synthclone::verify_eq(0.75, message_1.progress());

    BOOST_CHECK_THROW(
        synthclone::component_progress_message(1.1),
        synthclone::verification_error);
    BOOST_CHECK_THROW(
        synthclone::component_progress_message(-0.1),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_CASE(status_message)
{
    synthclone::component_status_message message_1("foo");
    synthclone::verify_eq("foo", message_1.status());

    synthclone::component_status_message message_2("bar");
    synthclone::verify_eq("bar", message_2.status());

    synthclone::component_status_message message_3(message_1);
    synthclone::verify_eq("foo", message_3.status());

    synthclone::component_status_message message_4(std::move(message_1));
    synthclone::verify_eq("foo", message_4.status());

    message_1 = message_2;
    synthclone::verify_eq("bar", message_1.status());

    message_2 = std::move(message_3);
    synthclone::verify_eq("foo", message_2.status());
}

BOOST_AUTO_TEST_SUITE_END()
