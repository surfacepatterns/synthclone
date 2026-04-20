import std;

import synthclone.core;
import synthclone.fx;

namespace synthclone {

    std::unique_ptr<plugin_instance>
    make_plugin_instance()
    {
        return make_fx_plugin_instance();
    }

}
