#include <boost/test/unit_test.hpp>

import std;

import synthclone.archive;
import synthclone.test;

namespace {

    void
    verify_device_id(
        const synthclone::archive_device_id& device_id,
        const synthclone::archive_device_major_t expected_major,
        const synthclone::archive_device_minor_t expected_minor
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_device_id", device_id, expected_major,
                expected_minor));

        synthclone::verify_eq(expected_major, device_id.major());
        synthclone::verify_eq(expected_minor, device_id.minor());
    }

}

BOOST_AUTO_TEST_SUITE(core)

BOOST_AUTO_TEST_CASE(device_ids)
{
    synthclone::archive_device_id device_id_1(2, 4);
    verify_device_id(device_id_1, 2, 4);

    synthclone::archive_device_id device_id_2(6, 8);
    verify_device_id(device_id_2, 6, 8);

    synthclone::archive_device_id device_id_3(device_id_1);
    verify_device_id(device_id_3, 2, 4);

    device_id_3 = device_id_2;
    verify_device_id(device_id_3, 6, 8);
}

BOOST_AUTO_TEST_SUITE_END()
