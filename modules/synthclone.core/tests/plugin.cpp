#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;

namespace {

    class test_instance final: public synthclone::plugin_instance {};

    class test_plugin final: public synthclone::plugin {

    public:

        test_plugin():
            metadata_(
                {
                    .identifier = "identifier",
                    .version = "version"
                })
        {
            // empty
        }

        std::unique_ptr<synthclone::plugin_instance>
        instantiate(synthclone::session_host& host)
        override final
        {
            return nullptr;
        }

        const synthclone::metadata&
        metadata()
        override final
        {
            return metadata_;
        }

    private:

        synthclone::metadata metadata_;

    };

    template<class T>
    void
    verify_empty_gen(std::generator<T> gen)
    {
        synthclone::verify_eq(gen.begin(), gen.end());
    }

}

BOOST_AUTO_TEST_SUITE(session)

BOOST_AUTO_TEST_CASE(instance)
{
    std::unique_ptr<synthclone::plugin_instance> instance(
        std::make_unique<test_instance>());

    verify_empty_gen(instance->capture_effect_types());
    verify_empty_gen(instance->exporter_types());
    verify_empty_gen(instance->importer_types());
    verify_empty_gen(instance->instrument_types());
}

BOOST_AUTO_TEST_CASE(plugin)
{
    std::unique_ptr<synthclone::plugin> plugin(
        std::make_unique<test_plugin>());
}

BOOST_AUTO_TEST_SUITE_END()
