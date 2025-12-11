#include <boost/core/demangle.hpp>

import std;

import synthclone.app;
import synthclone.qt;

template<std::derived_from<std::exception> T>
void
print_exception_summary(const T& e)
{
    std::println(
        ::stderr, "{0}: {1}",
        boost::core::demangle(typeid(e).name()), e.what());
}

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
        } catch (const synthclone::qml_error& e) {
            print_exception_summary(e);
            for (const auto& s: e.diagnostics()) {
                std::println(::stderr, "\t{0}", s);
            }
        } catch (const std::exception& e) {
            print_exception_summary(e);
        } catch (...) {
            std::println(::stderr, "unknown exception");
        }
    }

    std::println(::stderr, "");
    std::fflush(::stderr);
}

int
main(int argc, char** argv)
{
    std::set_terminate(process_termination);
    return synthclone::run(argc, argv);
}
