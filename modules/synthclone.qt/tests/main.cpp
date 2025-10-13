#define BOOST_TEST_ALTERNATIVE_INIT_API
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_MODULE synthclone_qt

#include <boost/test/unit_test.hpp>

import synthclone.external.qt.widgets;

bool
init_unit_tests()
{
    return true;
}

int
main(int argc, char **argv)
{
    ::QApplication app(argc, argv);
    return boost::unit_test::unit_test_main(init_unit_tests, argc, argv);
}
