/**
 * @file
 *
 * Contains plugin-oriented functionality.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:plugin;

import std;

import synthclone.util;

import :capture_effect;
import :exporter;
import :importer;
import :metadata_core;
import :sampler;

///////////////////////////////////////////////////////////////////////////////
// synthclone::plugin_metadata_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `plugin_metadata` instances using aggregate
     * initialization.
     */

    export
    struct plugin_metadata_init_args final {

        /**
         * The plugin identifier.
         */

        metadata_element identifier;

        /**
         * The plugin version.
         */

        metadata_element version;

        /**
         * The optional title for the plugin.
         */

        std::optional<metadata_element> title;

        /**
         * The optional URL that links to more information on the plugin.
         */

        std::optional<metadata_url> url;

        /**
         * The optional license descriptor for the plugin.
         */

        std::optional<metadata_element> license;

        /**
         * The creators of the plugin.
         */

        metadata_element_sequence creators;

        /**
         * The contributors to the plugin.
         */

        metadata_element_sequence contributors;

        /**
         * The description of the plugin.
         */

        metadata_text description;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::plugin_metadata
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains metadata about a plugin.
     */

    export
    class plugin_metadata final: public metadata {

    public:

        /**
         * Move constructor.
         */

        plugin_metadata(plugin_metadata&&) = default;

        /**
         * Copy constructor.
         */

        plugin_metadata(const plugin_metadata&) = default;

        /**
         * Constructs a `plugin_metadata` instance.
         *
         * @param args
         *   The data to use to populate the `metadata` instance.
         */

        constexpr
        plugin_metadata(plugin_metadata_init_args args) noexcept:
            metadata(
                std::move(args.identifier), std::move(args.version),
                std::move(args.title), std::move(args.url),
                std::move(args.license), std::move(args.creators),
                std::move(args.contributors), std::move(args.description))
        {
            // empty
        }

        /**
         * Move assignment operator.
         */

        plugin_metadata&
        operator=(plugin_metadata&&) = default;

        /**
         * Copy assignment operator.
         */

        plugin_metadata&
        operator=(const plugin_metadata& other)
        {
            *this = plugin_metadata(other);
            return *this;
        }

    };

    /**
     * Gets a boolean indicating whether or not two `plugin_metadata` instances
     * contain exactly the same metadata.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     *
     * @return
     *   The boolean indicator.
     */

    export
    constexpr
    bool
    operator==(const plugin_metadata& lhs, const plugin_metadata& rhs)
    {
        return equal(lhs, rhs);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::plugin_instance
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Represents a plugin instance.
     */

    export
    class plugin_instance: private nonmovable {

    public:

        /**
         * Destructor.
         */

        virtual
        ~plugin_instance() = default;

        /**
         * Gets a `capture_effect_type` instance for each capture effect type
         * provided by this plugin.
         *
         * @return
         *   A generator yielding `capture_effect_type` instances.
         */

        virtual
        std::generator<capture_effect_type>
        capture_effect_types()
        {
            co_return;
        }

        /**
         * Gets an `exporter_type` instance for each exporter type provided by
         * this plugin.
         *
         * @return
         *   A generator yielding `exporter_type` instances.
         */

        virtual
        std::generator<exporter_type>
        exporter_types()
        {
            co_return;
        }

        /**
         * Gets an `importer_type` instance for each importer type provided by
         * this plugin.
         *
         * @return
         *   A generator yielding `importer_type` instances.
         */

        virtual
        std::generator<importer_type>
        importer_types()
        {
            co_return;
        }

        /**
         * Gets the plugin metadata.
         *
         * @return
         *   The metadata.
         */

        virtual
        const plugin_metadata&
        metadata() = 0;

        /**
         * Gets a `sampler_type` instance for each sampler type provided by
         * this plugin.
         *
         * @return
         *   A generator yielding `sampler_type` instances.
         */

        virtual
        std::generator<sampler_type>
        sampler_types()
        {
            co_return;
        }

    protected:

        /**
         * Default constructor.
         */

        explicit
        plugin_instance() = default;

    };

}
