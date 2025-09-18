/**
 * @file
 *
 * Contains interfaces for plugins.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:plugin;

import std;

import synthclone.util;

import :effect;
import :exporter;
import :importer;
import :metadata;
import :sampler;
import :session;

///////////////////////////////////////////////////////////////////////////////
// synthclone::plugin_instance
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Represents a plugin instance loaded into a specific session.
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
         * Gets an `effect_type` instance for each effect type provided by this
         * plugin.
         *
         * @return
         *   A generator yielding `effect_type` instances.
         */

        virtual
        std::generator<effect_type>
        effect_types()
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

///////////////////////////////////////////////////////////////////////////////
// synthclone::plugin
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Represents a `synthclone` plugin.
     *
     * XXX: Add details on how `synthclone` instantiates `plugin` subclasses.
     */

    export
    class plugin: private nonmovable {

    public:

        /**
         * Destructor.
         */

        virtual
        ~plugin() = default;

        /**
         * Creates a plugin for the session described by the given host.
         *
         * @param host
         *   Allows the plugin to interact with `synthclone` and get session
         *   properties.
         *
         * @return
         *   The plugin instance.
         */

        virtual
        std::unique_ptr<plugin_instance>
        instantiate(session_host& host) = 0;

        /**
         * Gets the metadata for the plugin.
         *
         * @return
         *   The metadata.
         */

        virtual
        const metadata&
        metadata() = 0;

    protected:

        /**
         * Default constructor.
         */

        explicit
        plugin() = default;

    };

}
