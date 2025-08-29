module;

#include <boost/core/demangle.hpp>

export module synthclone.test:debug;

import std;

import :concepts;

///////////////////////////////////////////////////////////////////////////////
// make_debug_formattable()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<std::formattable<char> T>
    const T&
    make_debug_formattable(const T& arg)
    {
        return arg;
    }

    export
    template<ostream_printable T>
    requires (! std::formattable<T, char>)
    std::string
    make_debug_formattable(const T& arg)
    {
        std::ostringstream stream;
        stream << arg;
        return std::move(stream).str();
    }

    export
    template<class T>
    requires (! (std::formattable<T, char> || ostream_printable<T>))
    std::string
    make_debug_formattable(const T& arg)
    {
        return std::format(
            "{0}@{1}", boost::core::demangle(typeid(T).name()),
            reinterpret_cast<std::uintptr_t>(std::addressof(arg)));
    }

}
