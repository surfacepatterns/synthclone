module;

#include <boost/scope/scope_exit.hpp>
#include <boost/scope/scope_fail.hpp>

export module synthclone.external.boost.scope;

export
namespace boost::scope {

    using boost::scope::scope_exit;
    using boost::scope::scope_fail;

}
