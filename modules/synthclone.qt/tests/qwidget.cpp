#include <boost/test/unit_test.hpp>

import std;

import synthclone.external.qt;
import synthclone.qt;
import synthclone.util;

BOOST_AUTO_TEST_SUITE(qwidget)

BOOST_AUTO_TEST_CASE(visibility_guard)
{
    auto ptr = std::make_unique<::QWidget>();
    BOOST_CHECK(! ptr->isVisible());

    {
        synthclone::qwidget_visibility_guard guard(ptr.get(), true);
        BOOST_CHECK(ptr->isVisible());
    }

    BOOST_CHECK(! ptr->isVisible());

    BOOST_CHECK_THROW(
        synthclone::qwidget_visibility_guard(nullptr, true),
        synthclone::verification_error);
}

BOOST_AUTO_TEST_SUITE_END()
