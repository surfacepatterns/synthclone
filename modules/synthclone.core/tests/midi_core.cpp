#include <cstdint>
#include <optional>

#include <boost/test/unit_test.hpp>

import synthclone.core;
import synthclone.test;
import synthclone.util;

namespace {

    template<class T, std::uint_least8_t Max = 127>
    void
    verify_byte_constructor()
    {
        for (std::uint_least8_t i = 0; i <= Max; ++i) {
            synthclone::verify_eq(T{i}, i);
        }

        BOOST_CHECK_THROW(T{Max + 1}, synthclone::verification_error);
    }

}

BOOST_AUTO_TEST_SUITE(midi_core)

BOOST_AUTO_TEST_CASE(aftertouch_constructor)
{
    verify_byte_constructor<synthclone::midi_aftertouch>();
}

BOOST_AUTO_TEST_CASE(channel_constructor)
{
    verify_byte_constructor<synthclone::midi_channel, 15>();
}

BOOST_AUTO_TEST_CASE(control_index_constructor)
{
    verify_byte_constructor<synthclone::midi_control_index>();
}

BOOST_AUTO_TEST_CASE(control_value_constructor)
{
    verify_byte_constructor<synthclone::midi_control_value>();
}

BOOST_AUTO_TEST_CASE(note_constructor)
{
    verify_byte_constructor<synthclone::midi_note>();
}

BOOST_AUTO_TEST_CASE(velocity_constructor)
{
    verify_byte_constructor<synthclone::midi_velocity>();
}

BOOST_AUTO_TEST_SUITE_END()
