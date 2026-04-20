module;

#include <boost/json.hpp>

export module synthclone.external.boost.json;

export
namespace boost::json {

    using boost::json::array;
    using boost::json::key_value_pair;
    using boost::json::object;
    using boost::json::stream_parser;
    using boost::json::string;
    using boost::json::string_view;
    using boost::json::value;

    using boost::json::serialize;
    using boost::json::visit;

}
