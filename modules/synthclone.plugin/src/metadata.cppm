export module synthclone.plugin:metadata;

import std;

import synthclone.core;

///////////////////////////////////////////////////////////////////////////////
// synthclone::simple_metadata_init_args
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    struct simple_metadata_init_args final {

        metadata_element identifier;
        metadata_element title;
        metadata_element_sequence category;
        metadata_element_sequence creators;
        metadata_element_sequence contributors;
        metadata_text description;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::generate_simple_metadata()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    constexpr
    std::string_view simple_metadata_license("BSD-3-Clause");

    export
    metadata_init_args
    generate_simple_metadata(simple_metadata_init_args args)
    {
        return {
            .identifier = std::move(args.identifier),
            .version = std::format(
                "{0}.{1}.{2}", project_version_major, project_version_minor,
                project_version_patch),
            .title = std::move(args.title),
            .url = metadata_url(project_url),
            .license = metadata_element(simple_metadata_license),
            .category = std::move(args.category),
            .creators = std::move(args.creators),
            .contributors = std::move(args.contributors),
            .description = std::move(args.description)
        };
    }

}
