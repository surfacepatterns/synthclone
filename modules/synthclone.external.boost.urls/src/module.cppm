module;

#include <boost/url/param.hpp>
#include <boost/url/parse.hpp>

export module synthclone.external.boost.urls;

export
namespace boost::urls {

    using boost::urls::param;
    using boost::urls::parse_absolute_uri;
    using boost::urls::parse_relative_ref;

}
