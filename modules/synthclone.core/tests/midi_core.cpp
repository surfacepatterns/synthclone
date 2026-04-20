#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;
import synthclone.util;

namespace {

    template<class T, std::uint_least8_t Max = 127>
    void
    verify_byte_construction_ops()
    {
        for (std::uint_least8_t i = 0; i <= Max; ++i) {
            synthclone::verify_eq(T{i}, i);

            auto result = T::create(i);
            BOOST_REQUIRE(static_cast<bool>(result));
            synthclone::verify_eq(*result, i);
        }

        BOOST_CHECK_THROW(T{Max + 1}, synthclone::verification_error);

        auto result = T::create(Max + 1);
        BOOST_REQUIRE(! result);
        synthclone::verify_eq(
            result.error().value(),
            static_cast<int>(std::errc::invalid_argument));
    }

}

BOOST_AUTO_TEST_SUITE(midi_core)

BOOST_AUTO_TEST_CASE(aftertouch_construction)
{
    verify_byte_construction_ops<synthclone::midi_aftertouch>();
}

BOOST_AUTO_TEST_CASE(channel_construction)
{
    verify_byte_construction_ops<synthclone::midi_channel, 15>();
}

BOOST_AUTO_TEST_CASE(channel_pressure_construction)
{
    verify_byte_construction_ops<synthclone::midi_channel_pressure>();
}

BOOST_AUTO_TEST_CASE(control_index_construction)
{
    verify_byte_construction_ops<synthclone::midi_control_index>();
}

BOOST_AUTO_TEST_CASE(control_value_construction)
{
    verify_byte_construction_ops<synthclone::midi_control_value>();
}

BOOST_AUTO_TEST_CASE(note_construction)
{
    verify_byte_construction_ops<synthclone::midi_note>();
}

BOOST_AUTO_TEST_CASE(velocity_construction)
{
    verify_byte_construction_ops<synthclone::midi_velocity>();
}

BOOST_AUTO_TEST_SUITE_END()
