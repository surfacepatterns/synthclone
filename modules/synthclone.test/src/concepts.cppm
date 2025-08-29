module;

#include <boost/type_traits/has_left_shift.hpp>

export module synthclone.test:concepts;

import std;

///////////////////////////////////////////////////////////////////////////////
// synthclone::ostream_printable
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<class T>
    concept ostream_printable = boost::has_left_shift<std::ostream, T>::value;

}
