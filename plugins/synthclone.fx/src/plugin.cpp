import std;

import synthclone.core;
import synthclone.fx;

namespace synthclone {

    std::unique_ptr<plugin>
    make_plugin()
    {
        return make_fx_plugin();
    }

}
