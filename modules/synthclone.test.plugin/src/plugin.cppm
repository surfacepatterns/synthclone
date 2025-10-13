export module synthclone.test.plugin:plugin;

import std;

import synthclone.core;
import synthclone.test;

///////////////////////////////////////////////////////////////////////////////
// synthclone::load_capture_effect_type()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    capture_effect_type
    load_capture_effect_type(
        std::unique_ptr<plugin_instance>& instance,
        const metadata_element& identifier
    )
    {
        // XXX: We can uncomment this code when we're using a version of
        // `clang` that doesn't have this bug:
        //
        //     https://github.com/llvm/llvm-project/issues/138558
        //
        // BOOST_TEST_INFO_SCOPE(
        //     make_test_info(
        //         "synthclone::load_capture_effect_type", instance,
        //         identifier));

        auto gen = instance->capture_effect_types();
        auto iter = std::ranges::find_if(
            gen,
            [&identifier](const capture_effect_type& t) -> bool {
                return t.metadata().identifier() == identifier;
            });

        verify_ne(iter, gen.end());

        return *iter;
    }

}
