module;

#include <boost/io/ios_state.hpp>

export module synthclone.external.boost.io;

export
namespace boost::io {

    using boost::io::ios_exception_saver;
    using boost::io::ios_iostate_saver;

}
