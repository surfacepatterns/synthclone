#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_MODULE synthclone_fx

#include <boost/test/unit_test.hpp>

import synthclone.external.qt.gui;
import synthclone.test;

int
main(int argc, char **argv)
{
    ::QGuiApplication app(argc, argv);
    return synthclone::run_tests(argc, argv);
}
