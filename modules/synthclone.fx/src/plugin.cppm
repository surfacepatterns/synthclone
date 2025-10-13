export module synthclone.fx:plugin;

import synthclone.core;
import synthclone.plugin;

import :reverser;

///////////////////////////////////////////////////////////////////////////////
// synthclone::make_fx_plugin()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    class fx_plugin_instance final: public plugin_instance {

    public:

        inline explicit
        fx_plugin_instance(session_host& host):
            host_(host)
        {
            // empty
        }

        std::generator<capture_effect_type>
        capture_effect_types()
        override final
        {
            co_yield make_reverser_type(host_);
        }

    private:

        session_host& host_;

    };

    class fx_plugin final: public plugin {

    public:

        explicit
        fx_plugin():
            metadata_(
                generate_simple_metadata(
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

        std::unique_ptr<plugin_instance>
        instantiate(session_host& host)
        override final
        {
            return std::make_unique<fx_plugin_instance>(host);
        }

        const class metadata&
        metadata()
        override final
        {
            return metadata_;
        }

    private:

        class metadata metadata_;

    };

    export
    std::unique_ptr<plugin>
    make_fx_plugin()
    {
        return std::make_unique<fx_plugin>();
    }

}
