module;

#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/class_of.hpp>
#include <boost/callable_traits/return_type.hpp>

export module synthclone.external.boost:callable_traits;

export
namespace boost::callable_traits {

    using boost::callable_traits::args_t;
    using boost::callable_traits::class_of_t;
    using boost::callable_traits::return_type_t;

}
