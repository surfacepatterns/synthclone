#include <boost/test/unit_test.hpp>

import synthclone.fx;
import synthclone.test;

BOOST_AUTO_TEST_SUITE(plugin)

BOOST_AUTO_TEST_CASE(make)
{
    auto ptr = synthclone::make_fx_plugin_instance();

    const auto& metadata = ptr->metadata();
    synthclone::verify_eq("synthclone.fx", metadata.identifier());
}

BOOST_AUTO_TEST_SUITE_END()
