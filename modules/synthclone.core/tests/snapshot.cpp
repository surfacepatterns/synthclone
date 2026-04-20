#include <boost/test/unit_test.hpp>

import std;

import synthclone.archive;
import synthclone.core;
import synthclone.core.test;
import synthclone.external.boost.interprocess;
import synthclone.external.boost.iostreams;
import synthclone.test;
import synthclone.util;

namespace {

    // XXX: This could really use some refactoring.

    template<class I, class... Args, std::size_t... Indices>
    requires (
        (sizeof...(Args) == sizeof...(Indices)) &&
        (
            std::same_as<std::remove_cvref_t<Args>, std::filesystem::path> &&
            ...
        )
    )
    void
    add_session_snapshot_samples(
        I& iter,
        synthclone::dynamic_buffer<std::byte>& buffer,
        std::index_sequence<Indices...>,
        Args&&... sample_paths
    )
    {
        (
            synthclone::add_file(
                synthclone::tar_minimal_header, iter,
                std::forward<Args>(sample_paths),
                std::format("samples/{0}", Indices), buffer),
            ...
        );
    }

    template<class... Args>
    requires (
        std::same_as<std::remove_cvref_t<Args>, std::filesystem::path> && ...
    )
    void
    build_session_snapshot(
        const std::filesystem::path& snapshot_path,
        const std::filesystem::path& json_path,
        Args&&... sample_paths
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "build_session_snapshot", snapshot_path, json_path,
                std::forward<Args>(sample_paths)...));

        auto output_stream = synthclone::open_ofstream(
            snapshot_path, std::ios_base::binary | std::ios_base::out);

        boost::iostreams::filtering_stream<boost::iostreams::output>
            compressor_stream;
        compressor_stream.push(boost::iostreams::gzip_compressor());
        compressor_stream.push(output_stream);

        synthclone::tar_ostream_iterator iter(compressor_stream);
        synthclone::dynamic_buffer<std::byte> buffer(
            boost::interprocess::mapped_region::get_page_size());
        synthclone::add_file(
            synthclone::tar_minimal_header, iter, json_path, "snapshot.json",
            buffer);

        *iter = synthclone::tar_directory_header({.path = "samples"});
        ++iter;

        add_session_snapshot_samples(
            iter, buffer, std::make_index_sequence<sizeof...(Args)>{},
            std::forward<Args>(sample_paths)...);
    }

    template<std::ranges::range R>
    void
    build_session_snapshot(
        const std::filesystem::path& snapshot_path,
        R&& elements
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "build_session_snapshot", snapshot_path, elements));

        auto output_stream = synthclone::open_ofstream(
            snapshot_path, std::ios_base::binary | std::ios_base::out);

        boost::iostreams::filtering_stream<boost::iostreams::output>
            compressor_stream;
        compressor_stream.push(boost::iostreams::gzip_compressor());
        compressor_stream.push(output_stream);

        std::ranges::copy(
            elements, synthclone::tar_ostream_iterator(compressor_stream));
    }

    template<std::ranges::range R>
    void
    build_session_snapshot(
        const std::filesystem::path& snapshot_path,
        const std::filesystem::path& json_path,
        R&& elements
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "build_session_snapshot", snapshot_path, elements));

        auto output_stream = synthclone::open_ofstream(
            snapshot_path, std::ios_base::binary | std::ios_base::out);

        boost::iostreams::filtering_stream<boost::iostreams::output>
            compressor_stream;
        compressor_stream.push(boost::iostreams::gzip_compressor());
        compressor_stream.push(output_stream);

        synthclone::tar_ostream_iterator iter(compressor_stream);
        synthclone::dynamic_buffer<std::byte> buffer(
            boost::interprocess::mapped_region::get_page_size());
        synthclone::add_file(
            synthclone::tar_minimal_header, iter, json_path, "snapshot.json",
            buffer);

        *iter = synthclone::tar_directory_header({.path = "samples"});
        ++iter;

        std::ranges::copy(elements, std::move(iter));
    }

    std::tuple<
        synthclone::session_snapshot,
        synthclone::temporary_file,
        synthclone::temporary_file
    >
    make_complex_session_snapshot(synthclone::audio_endianness endianness)
    {
        std::array<synthclone::component_snapshot, 2> effects {
            synthclone::component_snapshot(
                "effect-plugin-1", "effect-type-1", "effect-version-1",
                synthclone::state_map {
                    {
                        "foo",
                        synthclone::state_array {
                            nullptr,
                            false,
                            true,
                            -1,
                            11111111111UL,
                            1.1,
                            "bar",
                        }
                    }
                }),
            synthclone::component_snapshot(
                "effect-plugin-2", "effect-type-2", "effect-version-2",
                std::nullopt)
        };

        std::array<synthclone::effect_chain_snapshot, 2> effect_chains {
            synthclone::effect_chain_snapshot(
                "foo",
                std::views::as_rvalue(
                    std::views::empty<synthclone::component_snapshot>)),
            synthclone::effect_chain_snapshot(
                "bar",
                std::views::as_rvalue(std::views::all(std::move(effects))))
        };

        std::array<synthclone::component_snapshot, 2> exporters {
            synthclone::component_snapshot(
                "exporter-plugin-1", "exporter-type-1", "exporter-version-1",
                synthclone::state_map {
                    {
                        "foo",
                        synthclone::state_array {
                            nullptr,
                            false,
                            true,
                            -2,
                            22222222222UL,
                            2.2,
                            "bar",
                        }
                    }
                }),
            synthclone::component_snapshot(
                "exporter-plugin-2", "exporter-type-2", "exporter-version-2",
                std::nullopt)
        };

        std::array<synthclone::component_snapshot, 2> importers {
            synthclone::component_snapshot(
                "importer-plugin-1", "importer-type-1", "importer-version-1",
                synthclone::state_map {
                    {
                        "foo",
                        synthclone::state_array {
                            nullptr,
                            false,
                            true,
                            -3,
                            33333333333UL,
                            3.3,
                            "bar",
                        }
                    }
                }),
            synthclone::component_snapshot(
                "importer-plugin-2", "importer-type-2", "importer-version-2",
                std::nullopt)
        };

        std::optional<synthclone::component_snapshot> sampler(
            synthclone::component_snapshot(
                "sampler-plugin-1", "sampler-type-1", "sampler-version-1",
                synthclone::state_map {
                    {
                        "foo",
                        synthclone::state_array {
                            nullptr,
                            false,
                            true,
                            -4,
                            44444444444UL,
                            4.4,
                            "bar",
                        }
                    }
                }));

        synthclone::audio_channel_count channel_count(2);
        synthclone::audio_sample_rate sample_rate(48000);
        synthclone::audio_traits traits(
            synthclone::audio_format::raw, synthclone::audio_codec::pcm_f32,
            endianness, sample_rate, channel_count);

        synthclone::temporary_file dry_sample_file;
        const auto& dry_sample_path = dry_sample_file.path();

        synthclone::temporary_file wet_sample_file;
        const auto& wet_sample_path = wet_sample_file.path();

        {
            synthclone::audio_output_stream dry_stream(dry_sample_path, traits);
            std::array<synthclone::audio_sample, 2> dry_samples {0.1, 0.2};
            dry_stream.write(dry_samples);

            synthclone::audio_output_stream wet_stream(wet_sample_path, traits);
            std::array<synthclone::audio_sample, 2> wet_samples {0.3, 0.4};
            wet_stream.write(wet_samples);
        }

        std::array<synthclone::zone_snapshot, 2> zones {
            synthclone::zone_snapshot(
                {
                    .channel = 1,
                    .note = 32,
                    .velocity = 127
                }),
            synthclone::zone_snapshot(
                {
                    .channel = 15,
                    .note = 64,
                    .velocity = 64,
                    .aftertouch = 32,
                    .channel_pressure = 96,
                    .controls = {{1, 108}, {64, 127}},
                    .sample_duration = 3700,
                    .release_duration = 1800,
                    .effect_chain_index = 0,
                    .dry_sample = synthclone::audio_source(
                        dry_sample_path, traits),
                    .wet_sample = synthclone::audio_source(
                        wet_sample_path, traits)
                })
        };

        return std::make_tuple(
            synthclone::session_snapshot(
                {
                    .sample_rate = sample_rate,
                    .channel_count = channel_count,
                    .effect_chains = std::views::as_rvalue(
                        std::views::all(std::move(effect_chains))),
                    .exporters = std::views::as_rvalue(
                        std::views::all(std::move(exporters))),
                    .importers = std::views::as_rvalue(
                        std::views::all(std::move(importers))),
                    .sampler = std::move(sampler),
                    .zones = std::views::as_rvalue(
                        std::views::all(std::move(zones)))
                }),
            std::move(dry_sample_file), std::move(wet_sample_file));
    }

    synthclone::session_snapshot
    make_minimal_session_snapshot()
    {
        return synthclone::session_snapshot(
            {
                .sample_rate = 44100,
                .channel_count = 1
            });
    }

    void
    verify_component_snapshot(
        const synthclone::component_snapshot& snapshot,
        const synthclone::metadata_element& expected_plugin_identifier,
        const synthclone::metadata_element& expected_type_identifier,
        const synthclone::metadata_element& expected_type_version,
        const std::optional<synthclone::state_value>& expected_state
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_component_snapshot", snapshot,
                expected_plugin_identifier, expected_type_identifier,
                expected_type_version, expected_state));

        synthclone::verify_eq(
            expected_plugin_identifier, snapshot.plugin_identifier());
        synthclone::verify_eq(
            expected_type_identifier, snapshot.type_identifier());
        synthclone::verify_eq(expected_type_version, snapshot.type_version());
        synthclone::verify_eq(expected_state, snapshot.state());
    }

    template<class E>
    void
    verify_component_snapshots(
        E&& expected_snapshots,
        synthclone::component_snapshot_view& actual_snapshots
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_component_snapshots", expected_snapshots,
                actual_snapshots));

        synthclone::verify_eq(
            expected_snapshots.size(), actual_snapshots.size());
        BOOST_REQUIRE(
            std::ranges::equal(expected_snapshots, actual_snapshots));
    }

    template<class R>
    void
    verify_effect_chain_snapshot(
        synthclone::effect_chain_snapshot& snapshot,
        const synthclone::utf8_line& expected_name,
        R&& expected_effects
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_effect_chain_snapshot", snapshot, expected_name,
                expected_effects));

        synthclone::verify_eq(expected_name, snapshot.name());
        verify_component_snapshots(expected_effects, snapshot.effects());
        synthclone::verify_eq(
            std::addressof(snapshot.effects()),
            std::addressof(
                static_cast<const synthclone::component_snapshot_view&>(
                    std::move(snapshot).effects())));
    }

    template<class E>
    void
    verify_effect_chain_snapshots(
        E&& expected_snapshots,
        synthclone::effect_chain_snapshot_view& actual_snapshots
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_effect_chain_snapshots", expected_snapshots,
                actual_snapshots));

        synthclone::verify_eq(
            expected_snapshots.size(), actual_snapshots.size());

        auto expected_end = expected_snapshots.end();
        auto expected_iter = expected_snapshots.begin();
        for (auto&& actual_snapshot: actual_snapshots) {
            synthclone::verify_ne(expected_iter, expected_end);
            auto&& expected_snapshot = *expected_iter;
            verify_effect_chain_snapshot(
                actual_snapshot, expected_snapshot.name(),
                expected_snapshot.effects());
            ++expected_iter;
        }
        synthclone::verify_eq(expected_iter, expected_end);
    }

    template<bool ValidateAudioStreams = false>
    void
    verify_zone_snapshot_audio_source(
        const std::optional<synthclone::audio_source>& expected_source,
        const std::optional<synthclone::audio_source>& actual_source
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_zone_snapshot_audio_source", expected_source,
                actual_source));

        if constexpr (ValidateAudioStreams) {
            if (! expected_source) {
                BOOST_REQUIRE(! actual_source);
            } else {
                BOOST_REQUIRE(static_cast<bool>(actual_source));
                synthclone::verify_audio_source(
                    *expected_source, *actual_source);
            }
        } else {
            synthclone::verify_eq(expected_source, actual_source);
        }
    }

    template<bool ValidateAudioStreams = false>
    void
    verify_zone_snapshot(
        const synthclone::zone_snapshot& snapshot,
        synthclone::midi_channel expected_channel,
        synthclone::midi_note expected_note,
        synthclone::midi_velocity expected_velocity,
        std::optional<synthclone::midi_aftertouch> expected_aftertouch,
        std::optional<synthclone::midi_channel_pressure> expected_pressure,
        const synthclone::midi_control_map& expected_controls,
        synthclone::audio_duration expected_sample_duration,
        synthclone::audio_duration expected_release_duration,
        const std::optional<std::size_t>& expected_effect_chain_index,
        const std::optional<synthclone::audio_source>& expected_dry_sample,
        const std::optional<synthclone::audio_source>& expected_wet_sample
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_zone_snapshot", snapshot, expected_channel,
                expected_note, expected_velocity, expected_aftertouch,
                expected_pressure, expected_controls, expected_sample_duration,
                expected_release_duration, expected_effect_chain_index,
                expected_dry_sample, expected_wet_sample));

        synthclone::verify_eq(expected_channel, snapshot.channel());
        synthclone::verify_eq(expected_note, snapshot.note());
        synthclone::verify_eq(expected_velocity, snapshot.velocity());
        synthclone::verify_eq(expected_aftertouch, snapshot.aftertouch());
        synthclone::verify_eq(expected_pressure, snapshot.channel_pressure());
        synthclone::verify_eq(expected_controls, snapshot.controls());
        synthclone::verify_eq(
            expected_sample_duration, snapshot.sample_duration());
        synthclone::verify_eq(
            expected_release_duration, snapshot.release_duration());
        synthclone::verify_eq(
            expected_effect_chain_index, snapshot.effect_chain_index());

        verify_zone_snapshot_audio_source<ValidateAudioStreams>(
            expected_dry_sample, snapshot.dry_sample());
        verify_zone_snapshot_audio_source<ValidateAudioStreams>(
            expected_wet_sample, snapshot.wet_sample());
    }

    template<bool ValidateAudioStreams = false>
    void
    verify_zone_snapshot(
        const synthclone::zone_snapshot& expected_snapshot,
        const synthclone::zone_snapshot& actual_snapshot
    )
    {
        verify_zone_snapshot<ValidateAudioStreams>(
            actual_snapshot, expected_snapshot.channel(),
            expected_snapshot.note(), expected_snapshot.velocity(),
            expected_snapshot.aftertouch(),
            expected_snapshot.channel_pressure(), expected_snapshot.controls(),
            expected_snapshot.sample_duration(),
            expected_snapshot.release_duration(),
            expected_snapshot.effect_chain_index(),
            expected_snapshot.dry_sample(), expected_snapshot.wet_sample());
    }

    template<bool ValidateAudioStreams = false, class E>
    void
    verify_zone_snapshots(
        E&& expected_snapshots,
        synthclone::zone_snapshot_view& actual_snapshots
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_zone_snapshots", expected_snapshots,
                actual_snapshots));

        synthclone::verify_eq(
            expected_snapshots.size(), actual_snapshots.size());

        auto expected_end = expected_snapshots.end();
        auto expected_iter = expected_snapshots.begin();

        auto actual_end = actual_snapshots.end();
        for (
            auto actual_iter = actual_snapshots.begin();
            actual_iter != actual_end;
            ++actual_iter, ++expected_iter
        ) {
            synthclone::verify_ne(expected_iter, expected_end);
            verify_zone_snapshot<ValidateAudioStreams>(
                *expected_iter, *actual_iter);
        }
        synthclone::verify_eq(expected_iter, expected_end);
    }

    template<
        bool ValidateAudioStreams = false,
        class C,
        class E,
        class I,
        class Z
    >
    void
    verify_session_snapshot(
        synthclone::session_snapshot& snapshot,
        synthclone::audio_sample_rate expected_sample_rate,
        synthclone::audio_channel_count expected_channel_count,
        C&& expected_effect_chains,
        E&& expected_exporters,
        I&& expected_importers,
        const std::optional<synthclone::component_snapshot>& expected_sampler,
        Z&& expected_zones
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_session_snapshot", snapshot, expected_sample_rate,
                expected_channel_count, expected_effect_chains,
                expected_exporters, expected_importers, expected_sampler,
                expected_zones));

        synthclone::verify_eq(expected_sample_rate, snapshot.sample_rate());
        synthclone::verify_eq(
            expected_channel_count, snapshot.channel_count());
        verify_effect_chain_snapshots(
            std::forward<C>(expected_effect_chains), snapshot.effect_chains());
        verify_component_snapshots(
            std::forward<E>(expected_exporters), snapshot.exporters());
        verify_component_snapshots(
            std::forward<I>(expected_importers), snapshot.importers());
        synthclone::verify_eq(expected_sampler, snapshot.sampler());
        verify_zone_snapshots<ValidateAudioStreams>(
            std::forward<Z>(expected_zones), snapshot.zones());

        synthclone::verify_eq(
            std::addressof(snapshot.effect_chains()),
            std::addressof(
                static_cast<const synthclone::effect_chain_snapshot_view&>(
                    std::move(snapshot).effect_chains())));
        synthclone::verify_eq(
            std::addressof(snapshot.exporters()),
            std::addressof(
                static_cast<const synthclone::component_snapshot_view&>(
                    std::move(snapshot).exporters())));
        synthclone::verify_eq(
            std::addressof(snapshot.importers()),
            std::addressof(
                static_cast<const synthclone::component_snapshot_view&>(
                    std::move(snapshot).importers())));
        synthclone::verify_eq(
            std::addressof(snapshot.sampler()),
            std::addressof(
                static_cast<
                    const std::optional<synthclone::component_snapshot>&
                >(std::move(snapshot).sampler())));
        synthclone::verify_eq(
            std::addressof(snapshot.zones()),
            std::addressof(
                static_cast<const synthclone::zone_snapshot_view&>(
                    std::move(snapshot).zones())));
    }

    template<bool ValidateAudioStreams = false>
    void
    verify_session_snapshot(
        synthclone::session_snapshot& expected_snapshot,
        synthclone::session_snapshot& actual_snapshot
    )
    {
        verify_session_snapshot<ValidateAudioStreams>(
            actual_snapshot, expected_snapshot.sample_rate(),
            expected_snapshot.channel_count(),
            expected_snapshot.effect_chains(), expected_snapshot.exporters(),
            expected_snapshot.importers(), expected_snapshot.sampler(),
            expected_snapshot.zones());
    }

    template<class... Args>
    requires (
        std::same_as<std::remove_cvref_t<Args>, std::filesystem::path> && ...
    )
    void
    verify_session_restoration(
        synthclone::session_snapshot& expected_snapshot,
        const std::filesystem::path& json_path,
        Args&&... sample_paths
    )
    {
        synthclone::temporary_file snapshot_file;
        const auto& snapshot_file_path = snapshot_file.path();
        build_session_snapshot(
            snapshot_file_path, json_path,
            std::forward<Args>(sample_paths)...);

        synthclone::out_param<synthclone::session_snapshot> result;
        auto gen = restore_snapshot(snapshot_file_path, result);
        for (const auto& message: gen) {
            // empty
        }

        BOOST_REQUIRE(static_cast<bool>(result));
        verify_session_snapshot<true>(expected_snapshot, result.value());
    }

    void
    verify_session_restoration_json_traversal_error(
        const std::filesystem::path& json_path,
        const std::string& expected_element_path
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_session_restoration_json_traversal_error", json_path,
                expected_element_path));

        synthclone::temporary_file snapshot_file;
        const auto& snapshot_file_path = snapshot_file.path();
        build_session_snapshot(snapshot_file_path, json_path);

        synthclone::out_param<synthclone::session_snapshot> result;
        auto gen = restore_snapshot(snapshot_file_path, result);

        auto end = gen.end();
        auto iter = gen.begin();
        while (iter != end) {
            try {
                ++iter;
            } catch (const synthclone::json_traversal_error& e) {
                synthclone::verify_eq(expected_element_path, e.path());
                return;
            }
        }

        BOOST_FAIL(
            std::format(
                "snapshot created with JSON from {0} failed to trigger a JSON "
                "error",
                json_path.string()));
    }

    template<std::ranges::range R>
    void
    verify_session_restoration_snapshot_error(R&& elements)
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_session_restoration_snapshot_error", elements));

        synthclone::temporary_file snapshot_file;
        const auto& snapshot_file_path = snapshot_file.path();
        build_session_snapshot(snapshot_file_path, elements);

        synthclone::out_param<synthclone::session_snapshot> result;
        auto gen = restore_snapshot(snapshot_file_path, result);

        auto end = gen.end();
        auto iter = gen.begin();
        while (iter != end) {
            try {
                ++iter;
            } catch (const synthclone::snapshot_error& e) {
                return;
            }
        }

        BOOST_FAIL("snapshot did not trigger a snapshot error");
    }

    template<std::ranges::range R>
    void
    verify_session_restoration_snapshot_error(
        const std::filesystem::path& json_path,
        R&& elements
    )
    {
        BOOST_TEST_INFO_SCOPE(
            synthclone::make_test_info(
                "verify_session_restoration_snapshot_error", json_path,
                elements));

        synthclone::temporary_file snapshot_file;
        const auto& snapshot_file_path = snapshot_file.path();
        build_session_snapshot(snapshot_file_path, json_path, elements);

        synthclone::out_param<synthclone::session_snapshot> result;
        auto gen = restore_snapshot(snapshot_file_path, result);

        auto end = gen.end();
        auto iter = gen.begin();
        while (iter != end) {
            try {
                ++iter;
            } catch (const synthclone::snapshot_error& e) {

                synthclone::trace("restoration error: {0}", e.what());

                return;
            }
        }

        BOOST_FAIL("snapshot did not trigger a snapshot error");
    }

    void
    verify_session_save(
        synthclone::session_snapshot&& snapshot_1,
        synthclone::session_snapshot&& snapshot_2
    )
    {
        synthclone::temporary_file file;
        const auto& path = file.path();

        for (
            const auto& message:
            synthclone::save_snapshot(std::move(snapshot_1), path)
        ) {
            // empty
        }

        synthclone::out_param<synthclone::session_snapshot> result;
        auto gen = restore_snapshot(path, result);
        for (const auto& message: gen) {
            // empty
        }

        BOOST_REQUIRE(static_cast<bool>(result));
        verify_session_snapshot<true>(snapshot_2, result.value());
    }

}

BOOST_AUTO_TEST_SUITE(snapshot)

BOOST_AUTO_TEST_CASE(basic_component_snapshots)
{
    synthclone::component_snapshot snapshot_1("plugin", "type", "version");
    verify_component_snapshot(
        snapshot_1, "plugin", "type", "version", std::nullopt);

    synthclone::component_snapshot snapshot_2(
        "other-plugin", "other-type", "other-version", 32);
    verify_component_snapshot(
        snapshot_2, "other-plugin", "other-type", "other-version", 32);

    synthclone::component_snapshot snapshot_3(snapshot_1);
    synthclone::verify_eq(snapshot_1, snapshot_3);
    synthclone::verify_ne(snapshot_1, snapshot_2);

    synthclone::component_snapshot snapshot_4(std::move(snapshot_2));
    synthclone::verify_ne(snapshot_1, snapshot_4);
    verify_component_snapshot(
        snapshot_4, "other-plugin", "other-type", "other-version", 32);

    snapshot_2 = snapshot_3;
    synthclone::verify_eq(snapshot_3, snapshot_2);
    synthclone::verify_ne(snapshot_2, snapshot_4);

    snapshot_1 = std::move(snapshot_4);
    synthclone::verify_ne(snapshot_1, snapshot_3);
    verify_component_snapshot(
        snapshot_1, "other-plugin", "other-type", "other-version", 32);
}

BOOST_AUTO_TEST_CASE(basic_component_snapshot_views)
{
    synthclone::component_snapshot_view view_1(
        std::views::as_rvalue(
            std::views::empty<synthclone::component_snapshot>));
    verify_component_snapshots(
        std::views::empty<synthclone::component_snapshot>, view_1);

    std::vector<synthclone::component_snapshot> snapshots {
        synthclone::component_snapshot("plugin-1", "type-1", "version-1"),
        synthclone::component_snapshot("plugin-2", "type-2", "version-2"),
        synthclone::component_snapshot("plugin-3", "type-3", "version-3"),
        synthclone::component_snapshot("plugin-4", "type-4", "version-4"),
        synthclone::component_snapshot("plugin-5", "type-5", "version-5")
    };
    synthclone::component_snapshot_view view_2(
        std::views::as_rvalue(std::views::all(snapshots)));
    verify_component_snapshots(snapshots, view_2);
}

BOOST_AUTO_TEST_CASE(basic_effect_chain_snapshots)
{
    synthclone::effect_chain_snapshot snapshot_1(
        "foo",
        std::views::as_rvalue(
            std::views::empty<synthclone::component_snapshot>));
    verify_effect_chain_snapshot(
        snapshot_1, "foo", std::views::empty<synthclone::component_snapshot>);

    std::vector<synthclone::component_snapshot> effects {
        synthclone::component_snapshot("plugin-1", "type-1", "version-1"),
        synthclone::component_snapshot("plugin-2", "type-2", "version-2"),
        synthclone::component_snapshot("plugin-3", "type-3", "version-3"),
        synthclone::component_snapshot("plugin-4", "type-4", "version-4"),
        synthclone::component_snapshot("plugin-5", "type-5", "version-5")
    };
    synthclone::effect_chain_snapshot snapshot_2(
        "bar", std::views::as_rvalue(std::views::all(effects)));
    verify_effect_chain_snapshot(snapshot_2, "bar", effects);
}

BOOST_AUTO_TEST_CASE(basic_effect_chain_snapshot_views)
{
    synthclone::effect_chain_snapshot_view view_1(
        std::views::as_rvalue(
            std::views::empty<synthclone::effect_chain_snapshot>));
    verify_effect_chain_snapshots(
        std::views::empty<synthclone::effect_chain_snapshot>, view_1);

    std::array<synthclone::component_snapshot, 3> effects_1 {
        synthclone::component_snapshot("plugin-1", "type-1", "version-1"),
        synthclone::component_snapshot("plugin-2", "type-2", "version-2"),
        synthclone::component_snapshot("plugin-3", "type-3", "version-3")
    };
    std::array<synthclone::component_snapshot, 2> effects_2 {
        synthclone::component_snapshot("plugin-4", "type-4", "version-4"),
        synthclone::component_snapshot("plugin-5", "type-5", "version-5")
    };
    std::array<synthclone::effect_chain_snapshot, 2> snapshots {
        synthclone::effect_chain_snapshot(
            "foo", std::views::as_rvalue(std::views::all(effects_1))),
        synthclone::effect_chain_snapshot(
            "bar", std::views::as_rvalue(std::views::all(effects_2)))
    };
    synthclone::effect_chain_snapshot_view view_2(
        std::views::as_rvalue(std::views::all(snapshots)));
    verify_effect_chain_snapshots(snapshots, view_2);
}

BOOST_AUTO_TEST_CASE(basic_session_snapshots)
{
    synthclone::session_snapshot snapshot_1(
        {
            .sample_rate = 48000,
            .channel_count = 1
        });
    verify_session_snapshot(
        snapshot_1, 48000, 1,
        std::views::empty<synthclone::effect_chain_snapshot>,
        std::views::empty<synthclone::component_snapshot>,
        std::views::empty<synthclone::component_snapshot>, std::nullopt,
        std::views::empty<synthclone::zone_snapshot>);

    std::array<synthclone::component_snapshot, 3> effects_1 {
        synthclone::component_snapshot("plugin-1", "type-1", "version-1"),
        synthclone::component_snapshot("plugin-2", "type-2", "version-2"),
        synthclone::component_snapshot("plugin-3", "type-3", "version-3")
    };
    std::array<synthclone::component_snapshot, 2> effects_2 {
        synthclone::component_snapshot("plugin-4", "type-4", "version-4"),
        synthclone::component_snapshot("plugin-5", "type-5", "version-5")
    };
    std::array<synthclone::effect_chain_snapshot, 2> effect_chain_snapshots {
        synthclone::effect_chain_snapshot(
            "foo", std::views::as_rvalue(std::views::all(effects_1))),
        synthclone::effect_chain_snapshot(
            "bar", std::views::as_rvalue(std::views::all(effects_2)))
    };

    std::array<synthclone::component_snapshot, 2> exporter_snapshots {
        synthclone::component_snapshot("plugin-6", "type-6", "version-6"),
        synthclone::component_snapshot("plugin-7", "type-7", "version-7")
    };

    std::array<synthclone::component_snapshot, 2> importer_snapshots {
        synthclone::component_snapshot("plugin-8", "type-8", "version-8"),
        synthclone::component_snapshot("plugin-9", "type-9", "version-9")
    };

    std::optional<synthclone::component_snapshot> sampler_snapshot(
        synthclone::component_snapshot("plugin-10", "type-10", "version-10"));

    std::vector<synthclone::zone_snapshot> zone_snapshots {
        synthclone::zone_snapshot(
            {
                .channel = 1,
                .note = 32,
                .velocity = 127
            }),
        synthclone::zone_snapshot(
            {
                .channel = 15,
                .note = 64,
                .velocity = 64,
                .aftertouch = 32,
                .channel_pressure = 96,
                .controls = {{1, 108}, {64, 127}},
                .sample_duration = 3700,
                .release_duration = 1800,
                .effect_chain_index = 2,
                .dry_sample = synthclone::audio_source("dry"),
                .wet_sample = synthclone::audio_source("wet")
            })
    };

    synthclone::session_snapshot snapshot_2(
        {
            .sample_rate = 44100,
            .channel_count = 2,
            .effect_chains = std::views::as_rvalue(
                std::views::all(effect_chain_snapshots)),
            .exporters = std::views::as_rvalue(
                std::views::all(exporter_snapshots)),
            .importers = std::views::as_rvalue(
                std::views::all(importer_snapshots)),
            .sampler = sampler_snapshot,
            .zones = std::views::as_rvalue(std::views::all(zone_snapshots))
        });
    verify_session_snapshot(
        snapshot_2, 44100, 2, effect_chain_snapshots, exporter_snapshots,
        importer_snapshots, sampler_snapshot, zone_snapshots);
}

BOOST_AUTO_TEST_CASE(basic_zone_snapshots)
{
    synthclone::zone_snapshot snapshot_1(
        {
            .channel = 1,
            .note = 32,
            .velocity = 127
        });
    verify_zone_snapshot(
        snapshot_1, 1, 32, 127, std::nullopt, std::nullopt, {}, 0, 0,
        std::nullopt, std::nullopt, std::nullopt);

    synthclone::midi_control_map controls {{1, 108}, {64, 127}};
    synthclone::audio_source dry_sample("dry");
    synthclone::audio_source wet_sample("wet");
    synthclone::zone_snapshot snapshot_2(
        {
            .channel = 15,
            .note = 64,
            .velocity = 64,
            .aftertouch = 32,
            .channel_pressure = 96,
            .controls = controls,
            .sample_duration = 3700,
            .release_duration = 1800,
            .effect_chain_index = 2,
            .dry_sample = dry_sample,
            .wet_sample = wet_sample
        });
    verify_zone_snapshot(
        snapshot_2, 15, 64, 64, 32, 96, controls, 3700, 1800, 2, dry_sample,
        wet_sample);

    synthclone::zone_snapshot snapshot_3(snapshot_1);
    synthclone::verify_eq(snapshot_1, snapshot_3);
    synthclone::verify_ne(snapshot_2, snapshot_3);

    synthclone::zone_snapshot snapshot_4(std::move(snapshot_1));
    synthclone::verify_ne(snapshot_2, snapshot_4);
    synthclone::verify_eq(snapshot_3, snapshot_4);

    snapshot_1 = snapshot_2;
    synthclone::verify_eq(snapshot_1, snapshot_2);
    synthclone::verify_ne(snapshot_1, snapshot_4);

    snapshot_1 = std::move(snapshot_4);
    synthclone::verify_ne(snapshot_1, snapshot_2);
    synthclone::verify_eq(snapshot_1, snapshot_3);
}

BOOST_AUTO_TEST_CASE(basic_zone_snapshot_views)
{
    synthclone::zone_snapshot_view view_1(
        std::views::as_rvalue(std::views::empty<synthclone::zone_snapshot>));
    verify_zone_snapshots(
        std::views::empty<synthclone::zone_snapshot>, view_1);

    std::vector<synthclone::zone_snapshot> snapshots {
        synthclone::zone_snapshot(
            {
                .channel = 1,
                .note = 32,
                .velocity = 127
            }),
        synthclone::zone_snapshot(
            {
                .channel = 15,
                .note = 64,
                .velocity = 64,
                .aftertouch = 32,
                .channel_pressure = 96,
                .controls = {{1, 108}, {64, 127}},
                .sample_duration = 3700,
                .release_duration = 1800,
                .effect_chain_index = 2,
                .dry_sample = synthclone::audio_source("dry"),
                .wet_sample = synthclone::audio_source("wet")
            })
    };
    synthclone::zone_snapshot_view view_2(
        std::views::as_rvalue(std::views::all(snapshots)));
    verify_zone_snapshots(snapshots, view_2);
}

BOOST_AUTO_TEST_CASE(session_snapshot_restoration)
{
    {
        auto expected_snapshot = make_minimal_session_snapshot();
        verify_session_restoration(
            expected_snapshot, "data/snapshots/json/minimal.json");
    }

    {
        auto [snapshot, dry_sample_file, wet_sample_file] =
            make_complex_session_snapshot(synthclone::audio_endianness::big);
        verify_session_restoration(
            snapshot, "data/snapshots/json/complex.json",
            dry_sample_file.path(), wet_sample_file.path());
    }
}

BOOST_AUTO_TEST_CASE(session_snapshot_restoration_snapshot_errors)
{
    verify_session_restoration_snapshot_error(
        std::views::empty<synthclone::tar_element>);
    verify_session_restoration_snapshot_error(
        std::vector<synthclone::tar_element> {
            synthclone::tar_directory_header({.path = "foo"})
        });
    verify_session_restoration_snapshot_error(
        std::vector<synthclone::tar_element> {
            synthclone::tar_file_header(
                {.path = "snapshotish.json", .size = 0})
        });
    verify_session_restoration_snapshot_error(
        std::vector<synthclone::tar_element> {
            synthclone::tar_file_header({.path = "snapshot.json", .size = 0})
        });

    {
        std::vector<std::byte> broken_json {
            std::byte('\"'), std::byte('\"'), std::byte('\"')
        };
        verify_session_restoration_snapshot_error(
            std::vector<synthclone::tar_element> {
                synthclone::tar_file_header(
                    {
                        .path = "snapshot.json",
                        .size = static_cast<synthclone::archive_size_t>(
                            broken_json.size())
                    }),
                synthclone::archive_file_fragment(std::span(broken_json))
            });
    }

    verify_session_restoration_snapshot_error(
        "data/snapshots/json/complex.json",
        std::views::empty<synthclone::tar_element>);

    {
        std::vector<std::byte> broken_sample {
            std::byte(0), std::byte(0), std::byte(0)
        };
        verify_session_restoration_snapshot_error(
            "data/snapshots/json/complex.json",
            std::vector<synthclone::tar_element> {
                synthclone::tar_file_header(
                    {
                        .path = "samples/0",
                        .size = static_cast<synthclone::archive_size_t>(
                            broken_sample.size())
                    }),
                synthclone::archive_file_fragment(std::span(broken_sample))
            });
    }
}

BOOST_AUTO_TEST_CASE(session_snapshot_restoration_json_traversal_errors)
{
    // Root

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-root-type.json", "$");

    // Session

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-session-missing.json", "$");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-session-type.json", "$.synthclone-session");

    // Version

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-version-missing.json",
        "$.synthclone-session");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-version-type.json",
        "$.synthclone-session.version");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-version-major-invalid.json",
        "$.synthclone-session.version.major");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-version-major-missing.json",
        "$.synthclone-session.version");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-version-major-type.json",
        "$.synthclone-session.version.major");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-version-minor-invalid.json",
        "$.synthclone-session.version.minor");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-version-minor-missing.json",
        "$.synthclone-session.version");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-version-minor-type.json",
        "$.synthclone-session.version.minor");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-version-patch-invalid.json",
        "$.synthclone-session.version.patch");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-version-patch-missing.json",
        "$.synthclone-session.version");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-version-patch-type.json",
        "$.synthclone-session.version.patch");

    // Sample rate

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sample-rate-invalid.json",
        "$.synthclone-session.sample-rate");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sample-rate-missing.json",
        "$.synthclone-session");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sample-rate-type.json",
        "$.synthclone-session.sample-rate");

    // Channel count

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-channel-count-invalid.json",
        "$.synthclone-session.channel-count");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-channel-count-missing.json",
        "$.synthclone-session");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-channel-count-type.json",
        "$.synthclone-session.channel-count");

    // Endianness

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-endianness-invalid.json",
        "$.synthclone-session.endianness");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-endianness-missing.json",
        "$.synthclone-session");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-endianness-type.json",
        "$.synthclone-session.endianness");

    // Sampler

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sampler-missing.json",
        "$.synthclone-session");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sampler-type.json",
        "$.synthclone-session.sampler");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sampler-plugin-identifier-invalid.json",
        "$.synthclone-session.sampler.plugin-identifier");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sampler-plugin-identifier-missing.json",
        "$.synthclone-session.sampler");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sampler-plugin-identifier-type.json",
        "$.synthclone-session.sampler.plugin-identifier");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sampler-type-identifier-invalid.json",
        "$.synthclone-session.sampler.type-identifier");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sampler-type-identifier-missing.json",
        "$.synthclone-session.sampler");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sampler-type-identifier-type.json",
        "$.synthclone-session.sampler.type-identifier");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sampler-type-version-invalid.json",
        "$.synthclone-session.sampler.type-version");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sampler-type-version-missing.json",
        "$.synthclone-session.sampler");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-sampler-type-version-type.json",
        "$.synthclone-session.sampler.type-version");

    // Effect chains

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-chains-missing.json",
        "$.synthclone-session");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-chains-type.json",
        "$.synthclone-session.effect-chains");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-chain-type.json",
        "$.synthclone-session.effect-chains[0]");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-chain-name-invalid.json",
        "$.synthclone-session.effect-chains[0].name");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-chain-name-missing.json",
        "$.synthclone-session.effect-chains[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-chain-name-type.json",
        "$.synthclone-session.effect-chains[0].name");

    // Effects

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effects-missing.json",
        "$.synthclone-session.effect-chains[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effects-type.json",
        "$.synthclone-session.effect-chains[0].effects");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-type.json",
        "$.synthclone-session.effect-chains[0].effects[0]");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-plugin-identifier-invalid.json",
        "$.synthclone-session.effect-chains[0].effects[0].plugin-identifier");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-plugin-identifier-missing.json",
        "$.synthclone-session.effect-chains[0].effects[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-plugin-identifier-type.json",
        "$.synthclone-session.effect-chains[0].effects[0].plugin-identifier");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-type-identifier-invalid.json",
        "$.synthclone-session.effect-chains[0].effects[0].type-identifier");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-type-identifier-missing.json",
        "$.synthclone-session.effect-chains[0].effects[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-type-identifier-type.json",
        "$.synthclone-session.effect-chains[0].effects[0].type-identifier");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-type-version-invalid.json",
        "$.synthclone-session.effect-chains[0].effects[0].type-version");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-type-version-missing.json",
        "$.synthclone-session.effect-chains[0].effects[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-effect-type-version-type.json",
        "$.synthclone-session.effect-chains[0].effects[0].type-version");

    // Exporters

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporters-missing.json",
        "$.synthclone-session");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporters-type.json",
        "$.synthclone-session.exporters");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporter-type.json",
        "$.synthclone-session.exporters[0]");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporter-plugin-identifier-invalid.json",
        "$.synthclone-session.exporters[0].plugin-identifier");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporter-plugin-identifier-missing.json",
        "$.synthclone-session.exporters[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporter-plugin-identifier-type.json",
        "$.synthclone-session.exporters[0].plugin-identifier");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporter-type-identifier-invalid.json",
        "$.synthclone-session.exporters[0].type-identifier");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporter-type-identifier-missing.json",
        "$.synthclone-session.exporters[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporter-type-identifier-type.json",
        "$.synthclone-session.exporters[0].type-identifier");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporter-type-version-invalid.json",
        "$.synthclone-session.exporters[0].type-version");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporter-type-version-missing.json",
        "$.synthclone-session.exporters[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-exporter-type-version-type.json",
        "$.synthclone-session.exporters[0].type-version");

    // Importers

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importers-missing.json",
        "$.synthclone-session");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importers-type.json",
        "$.synthclone-session.importers");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importer-type.json",
        "$.synthclone-session.importers[0]");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importer-plugin-identifier-invalid.json",
        "$.synthclone-session.importers[0].plugin-identifier");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importer-plugin-identifier-missing.json",
        "$.synthclone-session.importers[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importer-plugin-identifier-type.json",
        "$.synthclone-session.importers[0].plugin-identifier");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importer-type-identifier-invalid.json",
        "$.synthclone-session.importers[0].type-identifier");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importer-type-identifier-missing.json",
        "$.synthclone-session.importers[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importer-type-identifier-type.json",
        "$.synthclone-session.importers[0].type-identifier");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importer-type-version-invalid.json",
        "$.synthclone-session.importers[0].type-version");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importer-type-version-missing.json",
        "$.synthclone-session.importers[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-importer-type-version-type.json",
        "$.synthclone-session.importers[0].type-version");

    // Zones

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zones-missing.json",
        "$.synthclone-session");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zones-type.json",
        "$.synthclone-session.zones");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-invalid.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-type.json",
        "$.synthclone-session.zones[0]");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-aftertouch-invalid.json",
        "$.synthclone-session.zones[0].aftertouch");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-aftertouch-missing.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-aftertouch-type.json",
        "$.synthclone-session.zones[0].aftertouch");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-channel-invalid.json",
        "$.synthclone-session.zones[0].channel");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-channel-missing.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-channel-type.json",
        "$.synthclone-session.zones[0].channel");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-channel-pressure-invalid.json",
        "$.synthclone-session.zones[0].channel-pressure");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-channel-pressure-missing.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-channel-pressure-type.json",
        "$.synthclone-session.zones[0].channel-pressure");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-dry-sample-missing.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-dry-sample-type.json",
        "$.synthclone-session.zones[0].dry-sample");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-effect-chain-index-invalid.json",
        "$.synthclone-session.zones[0].effect-chain-index");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-effect-chain-index-missing.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-effect-chain-index-type.json",
        "$.synthclone-session.zones[0].effect-chain-index");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-note-invalid.json",
        "$.synthclone-session.zones[0].note");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-note-missing.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-note-type.json",
        "$.synthclone-session.zones[0].note");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-release-duration-invalid.json",
        "$.synthclone-session.zones[0].release-duration");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-release-duration-missing.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-release-duration-type.json",
        "$.synthclone-session.zones[0].release-duration");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-sample-duration-invalid.json",
        "$.synthclone-session.zones[0].sample-duration");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-sample-duration-missing.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-sample-duration-type.json",
        "$.synthclone-session.zones[0].sample-duration");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-velocity-invalid.json",
        "$.synthclone-session.zones[0].velocity");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-velocity-missing.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-velocity-type.json",
        "$.synthclone-session.zones[0].velocity");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-wet-sample-missing.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-zone-wet-sample-type.json",
        "$.synthclone-session.zones[0].wet-sample");

    // Controls

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-controls-missing.json",
        "$.synthclone-session.zones[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-controls-type.json",
        "$.synthclone-session.zones[0].controls");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-control-invalid.json",
        "$.synthclone-session.zones[0].controls[0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-control-type.json",
        "$.synthclone-session.zones[0].controls[0]");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-control-index-invalid.json",
        "$.synthclone-session.zones[0].controls[0][0]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-control-index-type.json",
        "$.synthclone-session.zones[0].controls[0][0]");

    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-control-value-invalid.json",
        "$.synthclone-session.zones[0].controls[0][1]");
    verify_session_restoration_json_traversal_error(
        "data/snapshots/json/error-control-value-type.json",
        "$.synthclone-session.zones[0].controls[0][1]");
}

BOOST_AUTO_TEST_CASE(session_snapshot_saving)
{
    verify_session_save(
        make_minimal_session_snapshot(), make_minimal_session_snapshot());

    synthclone::audio_endianness endianness =
        std::endian::native == std::endian::big ?
        synthclone::audio_endianness::big :
        synthclone::audio_endianness::little;

    auto [snapshot_1, dry_sample_file_1, wet_sample_file_1] =
        make_complex_session_snapshot(endianness);
    auto [snapshot_2, dry_sample_file_2, wet_sample_file_2] =
        make_complex_session_snapshot(endianness);
    verify_session_save(std::move(snapshot_1), std::move(snapshot_2));
}

BOOST_AUTO_TEST_CASE(zone_snapshot_errors)
{
    synthclone::zone_snapshot_init_args args(
        {
            .channel = 1,
            .note = 32,
            .velocity = 127,
            .wet_sample = synthclone::audio_source("wet")
        });
    BOOST_REQUIRE_THROW(
        synthclone::zone_snapshot{args}, synthclone::verification_error);
}

BOOST_AUTO_TEST_SUITE_END()
