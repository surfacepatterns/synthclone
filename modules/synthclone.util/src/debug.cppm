/**
 * @file
 *
 * Contains debugging facilities.
 */

module;

#include <synthclone/config.h>

export module synthclone.util:debug;

import std;

import :string;

///////////////////////////////////////////////////////////////////////////////
// synthclone::assumption_error
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Exception type signifying that a bad assumption was made.
     */

    export
    class assumption_error final: public std::logic_error {

    public:

        using std::logic_error::logic_error;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::verification_error
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Exception type signifying that verification failed.
     */

    export
    class verification_error final: public std::logic_error {

    public:

        using std::logic_error::logic_error;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::assume()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class... Args>
    class diagnostic_info final {

    public:

        template<class T>
        requires (string_view_convertible<const T&>)
        consteval
        diagnostic_info(
            const T& s,
            std::source_location location = std::source_location::current()
        ) noexcept:
            location_(std::move(location)),
            s_(s)
        {
            // empty
        }

        constexpr
        const std::source_location&
        location() const noexcept
        {
            return location_;
        }

        constexpr
        const std::format_string<Args...>&
        string() const noexcept
        {
            return s_;
        }

    private:

        const std::source_location location_;
        const std::format_string<Args...> s_;

    };

    template<class... Args>
    [[noreturn]]
    void
    process_assumption_error(
        const diagnostic_info<Args...>& info,
        Args&&... args
    )
    {
        const auto& location = info.location();
        try {
            throw assumption_error(
                std::format(
                    "[{0}:{1}:{2}] [{3}]: bad assumption: {4}",
                    location.file_name(), location.line(), location.column(),
                    location.function_name(),
                    std::format(info.string(), std::forward<Args>(args)...)));
        } catch (const assumption_error&) {
            std::terminate();
        }
    }

    /**
     * Checks a condition that is assumed to *always* be true regardless of
     * when and how the code is executed.
     *
     * If the code is executed at compile time and/or `NDEBUG` is not defined
     * when the module is compiled, then the condition is explicitly checked;
     * otherwise, the compiler is directed to assume the condition is always
     * true.
     *
     * If an assumption check fails at compile time, then the compiler will
     * emit an error.
     *
     * If an assumption check fails at runtime, then `std::terminate()` will be
     * called with an `assumption_error` in flight.
     *
     * @note
     *   When `synthclone` supports C++26, the functionality here may be
     *   enhanced such that the formatted string is used for compiler
     *   diagnostics.
     *
     * @param condition
     *   The underlying condition.
     * @param info
     *   Diagnostic info containing the format string and the source location
     *   of the call site.
     * @param args
     *   The arguments to be formatted.
     */

    export
    template<class... Args>
    constexpr
    void
    assume(
        const bool condition,
        diagnostic_info<std::type_identity_t<Args>...> info,
        Args&&... args
    )
    {
        if consteval {
            if (! condition) [[unlikely]] {
                throw;
            }
        } else {

#ifndef NDEBUG
            if (! condition) [[unlikely]] {
                process_assumption_error(info, std::forward<Args>(args)...);
            }
#endif

        }
        [[assume(condition)]];
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::assume_unreachable()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Makes the assumption that a section of code will never be reached.
     *
     * If the code is executed at compile time, then the compiler will emit an
     * error.
     *
     * If the code is executed at runtime and `NDEBUG` is not defined, then
     * `std::terminate()` will be called with an `assumption_error` in flight;
     * otherwise, the compiler is directed to assume that the code will never
     * be reached, resulting in undefined behavior.
     *
     * @note
     *   When `synthclone` supports C++26, the functionality here may be
     *   enhanced such that the formatted string is used for compiler
     *   diagnostics.
     *
     * @param info
     *   Diagnostic info containing the format string and the source location
     *   of the call site.
     * @param args
     *   The arguments to be formatted.
     */

    export
    template<class... Args>
    [[noreturn]]
    constexpr
    void
    assume_unreachable(
        diagnostic_info<std::type_identity_t<Args>...> info,
        Args&&... args
    )
    {
        if consteval {
            throw;
        } else {

#ifndef NDEBUG
            process_assumption_error(info, std::forward<Args>(args)...);
#endif

            std::unreachable();
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::trace()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Emits the current source location and a formatted message to `stderr`
     * if `NDEBUG` isn't defined; otherwise, does nothing.
     *
     * @param info
     *   Diagnostic info containing the format string and the source location
     *   of the call site.
     * @param args
     *   The arguments to be formatted.
     */

    export
    template<class... Args>
    void
    trace(diagnostic_info<std::type_identity_t<Args>...> info, Args&&... args)
    {

#ifndef NDEBUG
        const auto& location = info.location();
        std::println(
            std::cerr, "[{0}:{1}:{2}] [{3}]: {4}", location.file_name(),
            location.line(), location.column(), location.function_name(),
            std::format(info.string(), std::forward<Args>(args)...));
        std::cerr.flush();
#endif

    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class... Args>
    [[noreturn]]
    void
    process_verification_error(
        const diagnostic_info<Args...>& info,
        Args&&... args
    )
    {
        const auto& location = info.location();
        throw verification_error(
            std::format(
                "[{0}:{1}:{2}] [{3}]: verification failed: {4}",
                location.file_name(), location.line(), location.column(),
                location.function_name(),
                std::format(info.string(), std::forward<Args>(args)...)));
    }

    /**
     * Verifies that a condition is true.
     *
     * If a verification check fails at compile time, then the compiler will
     * emit an error.
     *
     * If a verification check fails at runtime, then a `verification_error`
     * will be thrown with an error message generated using the source
     * location, format string, and arguments.
     *
     * @note
     *   When `synthclone` supports C++26, the functionality here may be
     *   enhanced such that the formatted string is used for compiler
     *   diagnostics.
     *
     * @param condition
     *   The underlying condition.
     * @param info
     *   Diagnostic info containing the format string and the source location
     *   of the call site.
     * @param args
     *   The arguments to be formatted.
     */

    export
    template<class... Args>
    constexpr
    void
    verify(
        const bool condition,
        diagnostic_info<std::type_identity_t<Args>...> info,
        Args&&... args
    )
    {
        if consteval {
            if (! condition) [[unlikely]] {
                throw;
            }
        } else {
            if (! condition) [[unlikely]] {
                process_verification_error(info, std::forward<Args>(args)...);
            }
        }
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_unreachable()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Verifies that a section of code will never be reached.
     *
     * If the code is executed at compile time, then the compiler will emit an
     * error.
     *
     * If the code is executed at runtime, then a `verification_error` will be
     * thrown with an error message generated using the source location, format
     * string, and arguments.
     *
     * @note
     *   When `synthclone` supports C++26, the functionality here may be
     *   enhanced such that the formatted string is used for compiler
     *   diagnostics.
     *
     * @param info
     *   Diagnostic info containing the format string and the source location
     *   of the call site.
     * @param args
     *   The arguments to be formatted.
     */

    export
    template<class... Args>
    [[noreturn]]
    constexpr
    void
    verify_unreachable(
        diagnostic_info<std::type_identity_t<Args>...> info,
        Args&&... args
    )
    {
        if consteval {
            throw;
        } else {
            process_verification_error(info, std::forward<Args>(args)...);
        }
    }

}
