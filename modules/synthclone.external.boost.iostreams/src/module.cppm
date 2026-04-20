module;

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

export module synthclone.external.boost.iostreams;

export
namespace boost::iostreams {

    using boost::iostreams::filtering_stream;
    using boost::iostreams::gzip_compressor;
    using boost::iostreams::gzip_decompressor;
    using boost::iostreams::input;
    using boost::iostreams::output;

}
