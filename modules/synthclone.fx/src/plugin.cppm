export module synthclone.fx:plugin;

import synthclone.core;
import synthclone.plugin;

import :reverser;

///////////////////////////////////////////////////////////////////////////////
// synthclone::make_fx_plugin_instance()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    class fx_plugin_instance final: public plugin_instance {

    public:

        inline explicit
        fx_plugin_instance():
            metadata_(
                generate_simple_plugin_metadata(
                    {
                        .identifier = "synthclone.fx",
                        .title = "synthclone effects suite",
                        .description =
                            "Contains various `synthclone` specific effects "
                            "(reverser)."
                    }))
        {
            // empty
        }

        std::generator<capture_effect_type>
        capture_effect_types() override final
        {
            co_yield make_reverser_type();
        }

        const plugin_metadata&
        metadata() override final
        {
            return metadata_;
        }

    private:

        plugin_metadata metadata_;

    };

    export
    std::unique_ptr<plugin_instance>
    make_fx_plugin_instance()
    {
        return std::make_unique<fx_plugin_instance>();
    }

}
