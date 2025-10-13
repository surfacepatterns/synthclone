export module synthclone.test:concepts;

import std;

import synthclone.external.boost.type_traits;

///////////////////////////////////////////////////////////////////////////////
// synthclone::ostream_printable
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<class T>
    concept ostream_printable = boost::has_left_shift<std::ostream, T>::value;

}
