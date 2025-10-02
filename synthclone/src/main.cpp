#include <boost/core/demangle.hpp>

import std;

import synthclone.app;

void
process_termination()
{
    std::println(
        ::stderr, "termination handler called\n\nstack trace:\n{0}",
        std::stacktrace::current());

    std::exception_ptr ptr(std::current_exception());
    if (ptr) {
        try {
            std::rethrow_exception(ptr);
        } catch (const std::exception& e) {
            std::println(
                ::stderr, "{0}: {1}\n",
                boost::core::demangle(typeid(e).name()), e.what());
        } catch (...) {
            std::println(::stderr, "unknown exception\n");
        }
    }

    std::fflush(::stderr);
}

int
main(int argc, char** argv)
{
    std::set_terminate(process_termination);
    return synthclone::run(argc, argv);
}
