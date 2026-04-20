module;

#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/test/unit_test.hpp>

export module synthclone.test:controller;

import std;

import synthclone.external.boost.core;
import synthclone.external.boost.unit_test;

///////////////////////////////////////////////////////////////////////////////
// synthclone::run_tests()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    bool
    init_unit_tests()
    {
        return true;
    }

    void
    process_test_termination()
    {
        std::println(
            std::cerr, "termination handler called\n\nstack trace:\n{0}",
            std::stacktrace::current());

        auto ptr = std::current_exception();
        if (ptr) {
            try {
                std::rethrow_exception(ptr);
            } catch (const std::exception& e) {
                std::println(
                    std::cerr, "{0}: {1}",
                    boost::core::demangle(typeid(e).name()), e.what());
            } catch (...) {
                std::println(std::cerr, "unknown exception");
            }
        }

        std::println(std::cerr, "");
        std::cerr.flush();
    }

    export
    inline
    int
    run_tests(int argc, char** argv)
    {
        std::set_terminate(process_test_termination);
        return boost::unit_test::unit_test_main(init_unit_tests, argc, argv);
    }

}
