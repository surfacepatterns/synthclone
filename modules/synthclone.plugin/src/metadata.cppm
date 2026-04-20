export module synthclone.plugin:metadata;

import std;

import synthclone.core;

///////////////////////////////////////////////////////////////////////////////
// synthclone::simple_metadata_version
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    constexpr inline
    std::string simple_metadata_version(project_version);

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::simple_component_metadata_init_args
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    struct simple_component_metadata_init_args final {

        metadata_element identifier;
        metadata_element title;
        metadata_element_sequence creators;
        metadata_element_sequence contributors;
        metadata_text description;
        metadata_element_sequence category;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::generate_simple_component_metadata()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    constexpr
    std::string_view simple_metadata_license("BSD-3-Clause");

    export
    component_metadata_init_args
    generate_simple_component_metadata(
        simple_component_metadata_init_args args
    )
    {
        return {
            .identifier = std::move(args.identifier),
            .version = simple_metadata_version,
            .title = std::move(args.title),
            .url = metadata_url(project_url),
            .license = metadata_element(simple_metadata_license),
            .creators = std::move(args.creators),
            .contributors = std::move(args.contributors),
            .description = std::move(args.description),
            .category = std::move(args.category)
        };
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::simple_plugin_metadata_init_args
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    struct simple_plugin_metadata_init_args final {

        metadata_element identifier;
        metadata_element title;
        metadata_element_sequence creators;
        metadata_element_sequence contributors;
        metadata_text description;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::generate_simple_plugin_metadata()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    plugin_metadata_init_args
    generate_simple_plugin_metadata(simple_plugin_metadata_init_args args)
    {
        return {
            .identifier = std::move(args.identifier),
            .version = simple_metadata_version,
            .title = std::move(args.title),
            .url = metadata_url(project_url),
            .license = metadata_element(simple_metadata_license),
            .creators = std::move(args.creators),
            .contributors = std::move(args.contributors),
            .description = std::move(args.description)
        };
    }

}
