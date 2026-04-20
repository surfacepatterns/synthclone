#define BOOST_TEST_MODULE synthclone_core
#define BOOST_TEST_NO_MAIN

#include <boost/test/unit_test.hpp>

import synthclone.test;

int
main(int argc, char **argv)
{
    return synthclone::run_tests(argc, argv);
}
