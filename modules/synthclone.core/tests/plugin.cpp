#include <boost/test/unit_test.hpp>

import std;

import synthclone.core;
import synthclone.test;

namespace {

    class test_instance final: public synthclone::plugin_instance {

    public:

        test_instance():
            metadata_(
                {
                    .identifier = "identifier",
                    .version = "version"
                })
        {
            // empty
        }


        const synthclone::plugin_metadata&
        metadata() override final
        {
            return metadata_;
        }

    private:

        synthclone::plugin_metadata metadata_;

    };

    template<class T>
    void
    verify_empty_gen(std::generator<T> gen)
    {
        synthclone::verify_eq(gen.begin(), gen.end());
    }

    void
    verify_plugin_metadata(
        const synthclone::plugin_metadata& metadata,
        const synthclone::metadata_element& identifier,
        const synthclone::metadata_element& version,
        const std::optional<synthclone::metadata_element>& title,
        const std::optional<synthclone::metadata_url>& url,
        const std::optional<synthclone::metadata_element>& license,
        const std::vector<synthclone::metadata_element>& creators,
        const std::vector<synthclone::metadata_element>& contributors,
        const synthclone::metadata_text& description
    )
    {
        synthclone::verify_eq(identifier, metadata.identifier());
        synthclone::verify_eq(version, metadata.version());
        synthclone::verify_eq(title, metadata.title());
        synthclone::verify_eq(url, metadata.url());
        synthclone::verify_eq(license, metadata.license());

        const auto& metadata_creators = metadata.creators();
        BOOST_CHECK_EQUAL_COLLECTIONS(
            creators.begin(), creators.end(), metadata_creators.begin(),
            metadata_creators.end());

        const auto& metadata_contributors = metadata.contributors();
        BOOST_CHECK_EQUAL_COLLECTIONS(
            contributors.begin(), contributors.end(),
            metadata_contributors.begin(), metadata_contributors.end());

        synthclone::verify_eq(description, metadata.description());
    }

}

BOOST_AUTO_TEST_SUITE(plugin)

BOOST_AUTO_TEST_CASE(instance)
{
    std::unique_ptr<synthclone::plugin_instance> instance(
        std::make_unique<test_instance>());

    verify_empty_gen(instance->capture_effect_types());
    verify_empty_gen(instance->exporter_types());
    verify_empty_gen(instance->importer_types());
    verify_empty_gen(instance->sampler_types());
}

BOOST_AUTO_TEST_CASE(metadata)
{
    synthclone::plugin_metadata metadata_1(
        {
            .identifier = "some.identifier",
            .version = "some-version"
        });

    verify_plugin_metadata(
        metadata_1, "some.identifier", "some-version", std::nullopt,
        std::nullopt, std::nullopt, {}, {}, "");

    synthclone::plugin_metadata metadata_2(
        {
            .identifier = "some.other.identifier",
            .version = "1.2.3",
            .title = "title",
            .url = "http://host/path",
            .license = "license",
            .creators = {"me", "myself", "I"},
            .contributors = {"him", "her", "them"},
            .description = "description"
        });

    verify_plugin_metadata(
        metadata_2, "some.other.identifier", "1.2.3", "title",
        "http://host/path", "license", {"me", "myself", "I"},
        {"him", "her", "them"}, "description");

    synthclone::verify_ne(metadata_1, metadata_2);

    synthclone::plugin_metadata metadata_3(metadata_1);
    synthclone::verify_eq(metadata_1, metadata_3);

    synthclone::plugin_metadata metadata_4(std::move(metadata_1));
    synthclone::verify_eq(metadata_3, metadata_4);

    metadata_1 = metadata_2;
    synthclone::verify_eq(metadata_1, metadata_2);

    metadata_4 = std::move(metadata_1);
    synthclone::verify_eq(metadata_2, metadata_4);
}

BOOST_AUTO_TEST_SUITE_END()
