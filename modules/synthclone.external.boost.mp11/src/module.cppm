module;

#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/bind.hpp>
#include <boost/mp11/list.hpp>

export module synthclone.external.boost.mp11;

export
namespace boost::mp11 {

    using boost::mp11::mp_apply;
    using boost::mp11::mp_arg;
    using boost::mp11::mp_back;
    using boost::mp11::mp_bind;
    using boost::mp11::mp_filter_q;
    using boost::mp11::mp_find;
    using boost::mp11::mp_front;
    using boost::mp11::mp_list;
    using boost::mp11::mp_pop_back;
    using boost::mp11::mp_pop_front;
    using boost::mp11::mp_transform_q;

}
