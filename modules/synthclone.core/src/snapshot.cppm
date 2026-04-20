/**
 * @file
 *
 * Contains functionality for modelling and managing session snapshots.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:snapshot;

import std;

import synthclone.archive;
import synthclone.external.boost.interprocess;
import synthclone.external.boost.iostreams;
import synthclone.external.boost.json;
import synthclone.external.boost.numeric;
import synthclone.external.boost.system;
import synthclone.util;

import :app;
import :audio;
import :json_core;
import :metadata_core;
import :midi;
import :operation;
import :state;

///////////////////////////////////////////////////////////////////////////////
// synthclone::snapshot_error
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Identifies an error specific to snapshot functionality.
     */

    export
    class snapshot_error: public std::runtime_error {

    public:

        using std::runtime_error::runtime_error;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_snapshot
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Contains a snapshot of a component, including plugin/type information
     * and saved component state.
     */

    export
    class component_snapshot final {

    public:

        /**
         * Move constructor.
         */

        component_snapshot(component_snapshot&&) = default;

        /**
         * Copy constructor.
         */

        component_snapshot(const component_snapshot&) = default;

        /**
         * Constructs a `component_snapshot` instance.
         *
         * @param plugin_identifier
         *   The identifier for the plugin that created the component.
         * @param type_identifier
         *   The identifier for the component type.
         * @param type_version
         *   The version for the component type.
         * @param state
         *   The optional state of the component.
         */

        constexpr
        component_snapshot(
            metadata_element plugin_identifier,
            metadata_element type_identifier,
            metadata_element type_version,
            std::optional<state_value> state = std::nullopt
        ) noexcept:
            state_(std::move(state)),
            plugin_identifier_(std::move(plugin_identifier)),
            type_identifier_(std::move(type_identifier)),
            type_version_(std::move(type_version))
        {
            // empty
        }

        /**
         * Move assignment operator.
         */

        component_snapshot&
        operator=(component_snapshot&&) = default;

        /**
         * Copy assignment operator.
         */

        component_snapshot&
        operator=(const component_snapshot& other)
        {
            *this = component_snapshot(other);
            return *this;
        }

        /**
         * Gets the plugin identifier.
         *
         * @return
         *   The plugin identifier.
         */

        constexpr
        const metadata_element&
        plugin_identifier() const noexcept
        {
            return plugin_identifier_;
        }

        /**
         * Gets the optional component state.
         *
         * @return
         *   The optional state.
         */

        constexpr
        const std::optional<state_value>&
        state() const noexcept
        {
            return state_;
        }

        /**
         * Gets the type identifier.
         *
         * @return
         *   The type identifier.
         */

        constexpr
        const metadata_element&
        type_identifier() const noexcept
        {
            return type_identifier_;
        }

        /**
         * Gets the type version.
         *
         * @return
         *   The type version.
         */

        constexpr
        const metadata_element&
        type_version() const noexcept
        {
            return type_version_;
        }

    private:

        std::optional<state_value> state_;
        metadata_element plugin_identifier_;
        metadata_element type_identifier_;
        metadata_element type_version_;

    };

    /**
     * Compares two `component_snapshot` instances for equality.
     *
     * @param lhs
     *   The first instance.
     * @param rhs
     *   The second instance.
     *
     * @return
     *   A boolean indicating the result.
     */

    export
    constexpr
    bool
    operator==(
        const component_snapshot& lhs,
        const component_snapshot& rhs
    ) noexcept
    {
        return (lhs.plugin_identifier() == rhs.plugin_identifier()) &&
            (lhs.type_identifier() == rhs.type_identifier()) &&
            (lhs.type_version() == rhs.type_version()) &&
            (lhs.state() == rhs.state());
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::component_snapshot_view
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class T>
    class snapshot_view_iterator_base: private nonmovable {

    public:

        virtual
        ~snapshot_view_iterator_base() = default;

        virtual
        T&&
        operator*() const = 0;

        virtual
        void
        operator++() = 0;

        virtual
        bool
        operator==(std::default_sentinel_t sentinel) const = 0;

    protected:

        explicit
        snapshot_view_iterator_base() = default;

    };

    template<std::input_iterator I, std::sentinel_for<I> S>
    class snapshot_view_iterator_impl final:
        public snapshot_view_iterator_base<std::iter_value_t<I>> {

    public:

        snapshot_view_iterator_impl(I&& iter, S&& sentinel):
            snapshot_view_iterator_base<std::iter_value_t<I>>(),
            iter_(std::move(iter)),
            sentinel_(std::move(sentinel))
        {
            // empty
        }

        std::iter_rvalue_reference_t<I>
        operator*() const override final
        {
            return *iter_;
        }

        void
        operator++() override final
        {
            ++iter_;
        }

        bool
        operator==(std::default_sentinel_t) const override final
        {
            return iter_ == sentinel_;
        }

    private:

        I iter_;
        S sentinel_;

    };

    template<std::ranges::input_range R>
    requires (std::ranges::sized_range<R>)
    class snapshot_view_impl;

    /**
     * Type-erased input iterator proxy type that encapsulates an iterator and
     * sentinel provided with an input view.
     *
     * @tparam T
     *   The snapshot element type.
     */

    template<class T>
    class snapshot_view_iterator final: private noncopyable {

    public:

        /**
         * Difference type used with this iterator type.
         */

        using difference_type = std::ptrdiff_t;

        /**
         * The type of element this iterator iterates over.
         */

        using value_type = T;

        /**
         * Gets the current element.
         *
         * @return
         *   The current element.
         */

        T&&
        operator*() const
        {
            return *(*impl_);
        }

        /**
         * Advances the iterator to the next element.
         *
         * @return
         *   This iterator.
         */

        snapshot_view_iterator&
        operator++()
        {
            ++(*impl_);
            return *this;
        }

        /**
         * Advances the iterator to the next element.
         */

        void
        operator++(int)
        {
            ++(*this);
        }

        /**
         * Checks if an iterator has reached the end of the view it's iterating
         * over.
         *
         * @param iter
         *   The iterator to check.
         * @param sentinel
         *   A default sentinel instance.
         *
         * @return
         *   A boolean indicating the result.
         */

        friend
        bool
        operator==(
            const snapshot_view_iterator& iter,
            std::default_sentinel_t sentinel
        )
        {
            return *(iter.impl_) == sentinel;
        }

    private:

        template<std::ranges::input_range R>
        requires (std::ranges::sized_range<R>)
        friend
        class snapshot_view_impl;

        template<std::input_iterator I, std::sentinel_for<I> S>
        requires (std::same_as<std::iter_value_t<I>, T>)
        snapshot_view_iterator(I&& iter, S&& sentinel):
            impl_(
                std::make_unique<snapshot_view_iterator_impl<I, S>>(
                    std::move(iter), std::move(sentinel)))
        {
            // empty
        }

        std::unique_ptr<snapshot_view_iterator_base<T>> impl_;

    };

    template<class T>
    class snapshot_view_base: private nonmovable {

    public:

        virtual
        ~snapshot_view_base() = default;

        virtual
        snapshot_view_iterator<T>
        begin() = 0;

    protected:

        explicit
        snapshot_view_base() = default;

    };

    template<std::ranges::input_range R>
    requires (std::ranges::sized_range<R>)
    class snapshot_view_impl final:
        public snapshot_view_base<std::ranges::range_value_t<R>> {

    public:

        snapshot_view_impl(R&& range):
            snapshot_view_base<std::ranges::range_value_t<R>>(),
            range_(std::move(range))
        {
            // empty
        }

        snapshot_view_iterator<std::ranges::range_value_t<R>>
        begin() override final
        {
            return {range_.begin(), range_.end()};
        }

    private:

        R range_;

    };

    /**
     * Type-erased input view type that encapsulates snapshot elements.
     *
     * @tparam T
     *   The snapshot element type.
     */

    template<class T>
    class snapshot_view final:
        public std::ranges::view_interface<snapshot_view<T>> {

    public:

        /**
         * The iterator type.
         */

        using iterator = snapshot_view_iterator<T>;

        /**
         * The sentinel type.
         */

        using sentinel = std::default_sentinel_t;

        /**
         * Consructs a `snapshot_view` instance that encapsulates the given
         * range.
         *
         * @param range
         *   The range to encapsulate.
         */

        template<std::ranges::input_range R>
        requires (
            std::ranges::sized_range<R> &&
            std::same_as<std::ranges::range_value_t<R>, T>
        )
        snapshot_view(R&& range):
            size_(static_cast<std::size_t>(range.size())),
            impl_(std::make_unique<snapshot_view_impl<R>>(std::move(range)))
        {
            // empty
        }

        /**
         * Gets an iterator that points at the beginning of the encapsulated
         * range.
         *
         * @return
         *   The iterator.
         */

        inline
        iterator
        begin()
        {
            return impl_->begin();
        }

        /**
         * Gets an iterator that represents the end of the encapsulated range.
         *
         * @return
         *   The sentinel.
         */

        constexpr
        std::default_sentinel_t
        end() const noexcept
        {
            return std::default_sentinel;
        }

        /**
         * Gets the size of the encapsulated range.
         *
         * @return
         *   The size.
         */

        constexpr
        std::size_t
        size() const noexcept
        {
            return size_;
        }

    private:

        std::size_t size_;
        std::unique_ptr<snapshot_view_base<T>> impl_;

    };

    /**
     * View type that encapsulates a range of `component_snapshot` elements.
     */

    export
    using component_snapshot_view = snapshot_view<component_snapshot>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::effect_chain_snapshot
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Snapshot of an effect chain in a session snapshot.
     */

    export
    class effect_chain_snapshot final: private noncopyable {

    public:

        /**
         * Constructs an `effect_chain_snapshot` instance.
         *
         * @param name
         *   The effect chain name.
         * @param effects
         *   A view containing snapshots of the effects in the chain.
         */

        inline explicit
        effect_chain_snapshot(
            utf8_line name,
            component_snapshot_view effects
        ) noexcept:
            name_(std::move(name)),
            effects_(std::move(effects))
        {
            // empty
        }

        /**
         * Gets a view containing snapshots of the effects in the chain.
         *
         * @return
         *   The view.
         */

        component_snapshot_view&&
        effects() && noexcept
        {
            return std::move(effects_);
        }

        /**
         * Gets a view containing snapshots of the effects in the chain.
         *
         * @return
         *   The view.
         */

        component_snapshot_view&
        effects() & noexcept
        {
            return effects_;
        }

        /**
         * Gets the effect chain name.
         *
         * @return
         *   The name.
         */

        const utf8_line&
        name() const noexcept
        {
            return name_;
        }

    private:

        utf8_line name_;
        component_snapshot_view effects_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::effect_chain_snapshot_view
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * View type that encapsulates a range of `effect_chain_snapshot` elements.
     */

    export
    using effect_chain_snapshot_view = snapshot_view<effect_chain_snapshot>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::zone_snapshot_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `zone_snapshot` instances using aggregate
     * initialization.
     */

    export
    struct zone_snapshot_init_args final {

        /**
         * The MIDI channel.
         */

        midi_channel channel;

        /**
         * The MIDI note.
         */

        midi_note note;

        /**
         * The MIDI velocity.
         */

        midi_velocity velocity;

        /**
         * The optional MIDI aftertouch value.
         */

        std::optional<midi_aftertouch> aftertouch;

        /**
         * The optional MIDI channel pressure value.
         */

        std::optional<midi_channel_pressure> channel_pressure;

        /**
         * The optional MIDI control values.
         */

        midi_control_map controls;

        /**
         * The time that a sampler should spend capturing instrument audio
         * after the MIDI parameters are sent.
         */

        audio_duration sample_duration = 0;

        /**
         * The additional time that a sampler should spend capturing instrument
         * audio after the MIDI note off event is sent.
         */

        audio_duration release_duration = 0;

        /**
         * The index of the effect chain used by the zone.
         */

        std::optional<std::size_t> effect_chain_index;

        /**
         * An optional audio source referencing the dry (without effects)
         * sample associated with the zone.
         */

        std::optional<audio_source> dry_sample;

        /**
         * An optional audio source referencing the wet (with effects) sample
         * associated with the zone.
         */

        std::optional<audio_source> wet_sample;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::zone_snapshot
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    zone_snapshot_init_args&&
    verify_zone_snapshot_init_args(zone_snapshot_init_args&& args)
    {
        verify(
            static_cast<bool>(args.dry_sample) || (! args.wet_sample),
            "cannot create a zone without a dry sample when a wet sample is "
            "set");

        return std::move(args);
    }

    /**
     * Snapshot of a zone in a session snapshot.
     */

    export
    class zone_snapshot final {

    public:

        /**
         * Move constructor.
         */

        zone_snapshot(zone_snapshot&&) = default;

        /**
         * Copy constructor.
         */

        zone_snapshot(const zone_snapshot&) = default;

        /**
         * Constructs a `zone_snapshot` instance using the given arguments.
         *
         * @params args
         *   The arguments to use.
         */

        inline
        zone_snapshot(zone_snapshot_init_args args):
            zone_snapshot(
                verify_zone_snapshot_init_args(std::move(args)), validated)
        {
            // empty
        }

        /**
         * Move assignment operator.
         */

        zone_snapshot&
        operator=(zone_snapshot&&) = default;

        /**
         * Copy assignment operator.
         */

        zone_snapshot&
        operator=(const zone_snapshot& other)
        {
            *this = zone_snapshot(other);
            return *this;
        }

        /**
         * Gets the optional MIDI aftertouch value.
         *
         * @returns
         *   The aftertouch value.
         */

        constexpr
        std::optional<midi_aftertouch>
        aftertouch() const noexcept
        {
            return aftertouch_;
        }

        /**
         * Gets the MIDI channel.
         *
         * @returns
         *   The MIDI channel.
         */

        constexpr
        midi_channel
        channel() const noexcept
        {
            return channel_;
        }

        /**
         * Gets the optional MIDI channel pressure value.
         *
         * @returns
         *   The pressure value.
         */

        constexpr
        std::optional<midi_channel_pressure>
        channel_pressure() const noexcept
        {
            return channel_pressure_;
        }

        /**
         * Gets the MIDI control values.
         *
         * @returns
         *   The control values.
         */

        constexpr
        const midi_control_map&
        controls() const noexcept
        {
            return controls_;
        }

        /**
         * Gets the optional dry sample associated with the zone.
         *
         * @return
         *   The sample snapshot.
         */

        constexpr
        const std::optional<audio_source>&
        dry_sample() const noexcept
        {
            return dry_sample_;
        }

        /**
         * Gets the optional effect chain index.
         *
         * @returns
         *   The optional index.
         */

        constexpr
        std::optional<std::size_t>
        effect_chain_index() const noexcept
        {
            return effect_chain_index_;
        }

        /**
         * Gets the MIDI note value.
         *
         * @returns
         *   The note value.
         */

        constexpr
        midi_note
        note() const noexcept
        {
            return note_;
        }

        /**
         * Gets the release duration.
         *
         * @returns
         *   The release duration.
         */

        constexpr
        audio_duration
        release_duration() const noexcept
        {
            return release_duration_;
        }

        /**
         * Gets the sample duration.
         *
         * @returns
         *   The sample duration.
         */

        constexpr
        audio_duration
        sample_duration() const noexcept
        {
            return sample_duration_;
        }

        /**
         * Gets the MIDI velocity.
         *
         * @returns
         *   The MIDI velocity.
         */

        constexpr
        midi_velocity
        velocity() const noexcept
        {
            return velocity_;
        }

        /**
         * Gets the optional wet sample associated with the zone.
         *
         * @return
         *   The sample snapshot.
         */

        constexpr
        const std::optional<audio_source>&
        wet_sample() const noexcept
        {
            return wet_sample_;
        }

    private:

        inline
        zone_snapshot(zone_snapshot_init_args&& args, validated_t):
            controls_(std::move(args.controls)),
            dry_sample_(std::move(args.dry_sample)),
            wet_sample_(std::move(args.wet_sample)),
            effect_chain_index_(args.effect_chain_index),
            sample_duration_(args.sample_duration),
            release_duration_(args.release_duration),
            aftertouch_(args.aftertouch),
            channel_pressure_(args.channel_pressure),
            channel_(args.channel),
            note_(args.note),
            velocity_(args.velocity)
        {
            // empty
        }

        midi_control_map controls_;

        std::optional<audio_source> dry_sample_;
        std::optional<audio_source> wet_sample_;

        std::optional<std::size_t> effect_chain_index_;

        audio_duration sample_duration_;
        audio_duration release_duration_;

        std::optional<midi_aftertouch> aftertouch_;
        std::optional<midi_channel_pressure> channel_pressure_;

        midi_channel channel_;
        midi_note note_;
        midi_velocity velocity_;

    };

    /**
     * Gets a boolean indicating whether or not two `zone_snapshot` instances
     * contain the same data.
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
    operator==(const zone_snapshot& lhs, const zone_snapshot& rhs) noexcept
    {
        return (lhs.channel() == rhs.channel()) &&
            (lhs.note() == rhs.note()) &&
            (lhs.velocity() == rhs.velocity()) &&
            (lhs.aftertouch() == rhs.aftertouch()) &&
            (lhs.channel_pressure() == rhs.channel_pressure()) &&
            (lhs.controls() == rhs.controls()) &&
            (lhs.sample_duration() == rhs.sample_duration()) &&
            (lhs.release_duration() == rhs.release_duration()) &&
            (lhs.effect_chain_index() == rhs.effect_chain_index()) &&
            (lhs.dry_sample() == rhs.dry_sample()) &&
            (lhs.wet_sample() == rhs.wet_sample());
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::zone_snapshot_view
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * View type that encapsulates a range of `zone_snapshot` elements.
     */

    export
    using zone_snapshot_view = snapshot_view<zone_snapshot>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::session_snapshot_init_args
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Used to initialize `session_snapshot` instances using aggregate
     * initialization.
     */

    export
    struct session_snapshot_init_args final {

        /**
         * The session sample rate.
         */

        audio_sample_rate sample_rate;

        /**
         * The session channel count.
         */

        audio_channel_count channel_count;

        /**
         * A view of the effect chains contained in the session.
         */

        effect_chain_snapshot_view effect_chains =
            std::views::as_rvalue(std::views::empty<effect_chain_snapshot>);

        /**
         * A view of the exporters contained in the session.
         */

        component_snapshot_view exporters =
            std::views::as_rvalue(std::views::empty<component_snapshot>);

        /**
         * A view of the importers contained in the session.
         */

        component_snapshot_view importers =
            std::views::as_rvalue(std::views::empty<component_snapshot>);

        /**
         * The optional sampler contained in the session.
         */

        std::optional<component_snapshot> sampler = std::nullopt;

        /**
         * A view of the zones contained in the session.
         */

        zone_snapshot_view zones =
            std::views::as_rvalue(std::views::empty<zone_snapshot>);

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::session_snapshot
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Represents a snapshot of a `synthclone` session.
     */

    export
    class session_snapshot final: public noncopyable {

    public:

        /**
         * Constructs a `session_snapshot` instance.
         *
         * @param args
         *   The arguments to use.
         */

        inline
        session_snapshot(session_snapshot_init_args args) noexcept:
            effect_chains_(std::move(args.effect_chains)),
            exporters_(std::move(args.exporters)),
            importers_(std::move(args.importers)),
            zones_(std::move(args.zones)),
            sampler_(std::move(args.sampler)),
            sample_rate_(args.sample_rate),
            channel_count_(args.channel_count)
        {
            // empty
        }

        /**
         * Gets the session channel count.
         *
         * @return
         *   The channel count.
         */

        constexpr
        audio_channel_count
        channel_count() const noexcept
        {
            return channel_count_;
        }

        /**
         * Gets a view of snapshots of the effect chains contained in the
         * session.
         *
         * @return
         *   The view.
         */

        inline
        effect_chain_snapshot_view&&
        effect_chains() && noexcept
        {
            return std::move(effect_chains_);
        }

        /**
         * Gets a view of snapshots of the effect chains contained in the
         * session.
         *
         * @return
         *   The view.
         */

        inline
        effect_chain_snapshot_view&
        effect_chains() & noexcept
        {
            return effect_chains_;
        }

        /**
         * Gets a view of snapshots of the exporters contained in the session.
         *
         * @return
         *   The view.
         */

        inline
        component_snapshot_view&&
        exporters() && noexcept
        {
            return std::move(exporters_);
        }

        /**
         * Gets a view of snapshots of the exporters contained in the session.
         *
         * @return
         *   The view.
         */

        inline
        component_snapshot_view&
        exporters() & noexcept
        {
            return exporters_;
        }

        /**
         * Gets a view of snapshots of the importers contained in the session.
         *
         * @return
         *   The view.
         */

        inline
        component_snapshot_view&&
        importers() && noexcept
        {
            return std::move(importers_);
        }

        /**
         * Gets a view of snapshots of the importers contained in the session.
         *
         * @return
         *   The view.
         */

        inline
        component_snapshot_view&
        importers() & noexcept
        {
            return importers_;
        }

        /**
         * Gets the session sample rate.
         *
         * @return
         *   The sample rate.
         */

        constexpr
        audio_sample_rate
        sample_rate() const noexcept
        {
            return sample_rate_;
        }

        /**
         * Gets a snapshot of the optional sampler contained in the session.
         *
         * @return
         *   The optional snapshot.
         */

        constexpr
        const std::optional<component_snapshot>&
        sampler() const & noexcept
        {
            return sampler_;
        }

        /**
         * Gets a snapshot of the optional sampler contained in the session.
         *
         * @return
         *   The optional snapshot.
         */

        constexpr
        std::optional<component_snapshot>&
        sampler() & noexcept
        {
            return sampler_;
        }

        /**
         * Gets a snapshot of the optional sampler contained in the session.
         *
         * @return
         *   The optional snapshot.
         */

        constexpr
        std::optional<component_snapshot>&&
        sampler() && noexcept
        {
            return std::move(sampler_);
        }

        /**
         * Gets a view of snapshots of the zones contained in the session.
         *
         * @return
         *   The view.
         */

        inline
        zone_snapshot_view&&
        zones() && noexcept
        {
            return std::move(zones_);
        }

        /**
         * Gets a view of snapshots of the zones contained in the session.
         *
         * @return
         *   The view.
         */

        inline
        zone_snapshot_view&
        zones() & noexcept
        {
            return zones_;
        }

    private:

        effect_chain_snapshot_view effect_chains_;
        component_snapshot_view exporters_;
        component_snapshot_view importers_;
        zone_snapshot_view zones_;

        std::optional<component_snapshot> sampler_;

        audio_sample_rate sample_rate_;
        audio_channel_count channel_count_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::snapshot_restore_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Variant type containing types of messages that may be emitted in a
     * `restore_snapshot()` operation.
     */

    export
    using snapshot_restore_message = std::variant<
        operation_progress_message,
        operation_status_message,
        operation_warning_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::restore_snapshot()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    class snapshot_json_header_visitor final {

    public:

        explicit
        snapshot_json_header_visitor(
            const std::filesystem::path& path
        ) noexcept:
            path_(path)
        {
            // empty
        }

        template<class T>
        [[noreturn]]
        void
        operator()(const T& header)
        {
            throw snapshot_error(
                std::format(
                    "{0:?} is not a valid session snapshot: first archive "
                    "header is not a file header",
                    path_.string()));
        }

        void
        operator()(const tar_file_header& header)
        {
            if (header.path() != "snapshot.json") [[unlikely]] {
                throw snapshot_error(
                    std::format(
                        "{0:?} is not a valid session snapshot: first archive "
                        "header path is not set to `snapshot.json`",
                        path_.string()));
            }
        }

    private:

        std::filesystem::path path_;

    };

    class snapshot_json_fragment_visitor final {

    public:

        snapshot_json_fragment_visitor(
            const std::filesystem::path& path,
            boost::json::stream_parser& json_parser
        ) noexcept:
            json_parser_(json_parser),
            path_(path)
        {
            // empty
        }

        template<class T>
        bool
        operator()(const T& header)
        {
            return false;
        }

        bool
        operator()(const archive_file_fragment& fragment)
        {
            auto size = fragment.size();

            std::size_t bytes_written;
            try {
                bytes_written = json_parser_.write(
                    reinterpret_cast<const char*>(fragment.data()), size);
            } catch (const boost::system::system_error& e) {
                std::throw_with_nested(
                    snapshot_error(
                        std::format(
                            "{0:?} is not a valid session snapshot: failed to "
                            "parse `snapshot.json`: {1}",
                            path_.string(), e.what())));
            }

            assume(
                bytes_written == size,
                "archive file fragment contained {0} bytes, but only {1} "
                "bytes were consumed by the JSON parser",
                size, bytes_written);

            return true;
        }

    private:

        boost::json::stream_parser& json_parser_;
        const std::filesystem::path& path_;
        std::error_code error_code_;

    };

    using snapshot_sample_path_map =
        std::unordered_map<std::filesystem::path, std::filesystem::path>;

    class snapshot_sample_header_visitor final {

    public:

        constexpr
        snapshot_sample_header_visitor(
            const std::filesystem::path& archive_path,
            audio_channel_count channel_count,
            snapshot_sample_path_map& sample_path_map
        ):
            archive_path_(archive_path),
            frame_size_(channel_count.value() * sizeof(audio_sample)),
            sample_path_map_(sample_path_map)
        {
            // empty
        }

        template<class T>
        std::optional<std::pair<std::filesystem::path, bool>>
        operator()(const T& header) const
        {
            return std::make_pair(header.path(), false);
        }

        [[noreturn]]
        std::optional<std::pair<std::filesystem::path, bool>>
        operator()(const archive_file_fragment&) const
        {
            assume_unreachable(
                "tar archive header expected, but found file fragment");
        }

        std::optional<std::pair<std::filesystem::path, bool>>
        operator()(const tar_directory_header& header) const noexcept
        {
            return std::nullopt;
        }

        std::optional<std::pair<std::filesystem::path, bool>>
        operator()(const tar_file_header& header) const
        {
            const auto& header_path = header.path();

            auto iter = sample_path_map_.find(header_path);
            if (iter == sample_path_map_.end()) {
                return std::make_pair(header_path, false);
            }

            auto size = header.size();
            if ((size % frame_size_) != 0) [[unlikely]] {
                throw snapshot_error(
                    std::format(
                        "{0:?} is not a valid session snapshot: archived file "
                        "{1:?} has size of {2}, which isn't evenly divisible "
                        "by frame size of {3}",
                        archive_path_.string(), header_path.string(), size,
                        frame_size_));
            }

            std::filesystem::path write_path(std::move(iter->second));
            sample_path_map_.erase(iter);
            return std::make_pair(write_path, true);
        }

    private:

        const std::filesystem::path& archive_path_;
        std::size_t frame_size_;
        snapshot_sample_path_map& sample_path_map_;

    };

    class snapshot_sample_fragment_visitor final {

    public:

        explicit
        snapshot_sample_fragment_visitor(const std::filesystem::path& path):
            stream_(
                open_ofstream(
                    path, std::ios_base::binary | std::ios_base::out))
        {
            // empty
        }

        template<class T>
        constexpr
        bool
        operator()(const T& header) const
        {
            return false;
        }

        bool
        operator()(const archive_file_fragment& fragment)
        {
            write(stream_, fragment);
            return true;
        }

    private:

        std::ofstream stream_;

    };

    template<arithmetic N, json_number_type T, json_path_type P>
    N
    validate_snapshot_number(const json_number_node<T, P>& node)
    {
        auto element = node.element();
        try {
            return boost::numeric_cast<N>(element);
        } catch (const boost::bad_numeric_cast&) {
            std::throw_with_nested(
                json_validation_error(
                    std::format(
                        "expected value of type {0}, but numeric value {1} "
                        "could not be converted to that type",
                        boost::core::demangle(typeid(T).name()), element)));
        }
    }

    struct snapshot_version_portion_visitor final {

        template<json_integral_type T, json_path_type P>
        std::uint_least32_t
        operator()(const json_number_node<T, P>& node) const
        {
            return validate_snapshot_number<std::uint_least32_t>(node);
        }

    };

    struct snapshot_version_visitor final {

        template<json_path_type P>
        app_version
        operator()(const json_object_node<P>& node) const
        {
            return app_version(
                traverse(node, "major", snapshot_version_portion_visitor{}),
                traverse(node, "minor", snapshot_version_portion_visitor{}),
                traverse(node, "patch", snapshot_version_portion_visitor{}));
        }

    };

    struct snapshot_sample_rate_visitor final {

        template<json_integral_type T, json_path_type P>
        audio_sample_rate
        operator()(const json_number_node<T, P>& node) const
        {
            auto value = validate_snapshot_number<
                audio_sample_rate::scalar_type
            >(node);
            auto result = audio_sample_rate::create(value);
            if (! result) {
                throw json_validation_error(
                    std::format(
                        "{0} is not a valid audio sample rate", value));
            }
            return result.value();
        }

    };

    struct snapshot_channel_count_visitor final {

        template<json_integral_type T, json_path_type P>
        audio_channel_count
        operator()(const json_number_node<T, P>& node) const
        {
            auto value = validate_snapshot_number<
                audio_channel_count::scalar_type
            >(node);
            auto result = audio_channel_count::create(value);
            if (! result) {
                throw json_validation_error(
                    std::format(
                        "{0} is not a valid audio channel count", value));
            }
            return result.value();
        }

    };

    struct snapshot_endianness_visitor final {

        template<json_path_type P>
        audio_endianness
        operator()(const json_string_node<P>& node) const
        {
            const auto& s = node.element();
            if (s == "big") {
                return audio_endianness::big;
            }
            if (s == "little") {
                return audio_endianness::little;
            }
            throw json_validation_error(
                std::format(
                    "{0:?} is not a valid audio endianness string",
                    static_cast<std::string_view>(s)));
        }

    };

    struct snapshot_metadata_element_visitor final {

        template<json_path_type P>
        metadata_element
        operator()(const json_string_node<P>& node) const
        {
            const auto& s = node.element();
            try {
                return metadata_element(s);
            } catch (const unicode_error& e) {
                std::throw_with_nested(
                    json_validation_error(
                        std::format(
                            "{0:?}: {1}", static_cast<std::string_view>(s),
                            e.what())));
            }
        }

    };

    struct snapshot_state_json_visitor final {

        template<class T>
        state_value
        operator()(const T& element) const
        {
            return element;
        }

        state_value
        operator()(const boost::json::array& element) const
        {
            return state_array(
                std::from_range,
                std::views::transform(
                    element,
                    [this](const boost::json::value& value) {
                        return boost::json::visit(*this, value);
                    }));
        }

        state_value
        operator()(const boost::json::object& element) const
        {
            return state_map(
                std::from_range,
                std::views::transform(
                    element,
                    [this](const boost::json::key_value_pair& pair) {
                        return std::make_pair(
                            pair.key(),
                            boost::json::visit(*this, pair.value()));
                    }));
        }

        state_value
        operator()(const boost::json::string& element) const
        {
            return static_cast<std::string_view>(element);
        }

    };

    struct snapshot_state_value_visitor final {

        template<json_path_type P, class T>
        state_value
        operator()(const basic_json_node<P, T>& node) const
        {
            return snapshot_state_json_visitor{}(node.element());
        }

    };

    struct snapshot_component_visitor final {

        template<json_path_type P>
        component_snapshot
        operator()(const json_object_node<P>& node) const
        {
            return component_snapshot(
                traverse(
                    node, "plugin-identifier",
                    snapshot_metadata_element_visitor{}),
                traverse(
                    node, "type-identifier",
                    snapshot_metadata_element_visitor{}),
                traverse(
                    node, "type-version", snapshot_metadata_element_visitor{}),
                traverse_if(node, "state", snapshot_state_value_visitor{}));
        }

    };

    constexpr boost::json::array::size_type snapshot_json_array_zero_index = 0;

    struct snapshot_component_array_visitor final {

        template<json_path_type P>
        auto
        operator()(const json_array_node<P>& node) const
        {
            return std::views::as_rvalue(
                std::views::all(
                    std::ranges::to<std::vector<component_snapshot>>(
                        std::views::transform(
                            std::views::iota(
                                snapshot_json_array_zero_index,
                                node.element().size()),
                            [&node](std::size_t i) {
                                return traverse(
                                    node, i, snapshot_component_visitor{});
                            }))));
        }

    };

    struct snapshot_effect_chain_visitor final {

        template<json_path_type P>
        effect_chain_snapshot
        operator()(const json_object_node<P>& node) const
        {
            return effect_chain_snapshot(
                traverse(node, "name", snapshot_metadata_element_visitor{}),
                traverse(node, "effects", snapshot_component_array_visitor{}));
        }

    };

    struct snapshot_effect_chain_array_visitor final {

        template<json_path_type P>
        auto
        operator()(const json_array_node<P>& node) const
        {
            return std::views::all(
                std::ranges::to<std::vector<effect_chain_snapshot>>(
                    std::views::transform(
                        std::views::iota(
                            snapshot_json_array_zero_index,
                            node.element().size()),
                        [&node](std::size_t i) {
                            return traverse(
                                node, i, snapshot_effect_chain_visitor{});
                        })));
        }

    };

    struct snapshot_sampler_visitor final {

        template<json_path_type P>
        std::optional<component_snapshot>
        operator()(const json_null_node<P>& /*node*/) const noexcept
        {
            return std::nullopt;
        }

        template<json_path_type P>
        std::optional<component_snapshot>
        operator()(const json_object_node<P>& node) const
        {
            return snapshot_component_visitor{}(node);
        }

    };

    struct snapshot_midi_channel_visitor final {

        template<json_integral_type T, json_path_type P>
        midi_channel
        operator()(const json_number_node<T, P>& node) const
        {
            auto value =
                validate_snapshot_number<midi_channel::scalar_type>(node);
            auto result = midi_channel::create(value);
            if (! result) {
                throw json_validation_error(
                    std::format(
                        "{0} is not a valid MIDI channel byte", value));
            }
            return result.value();
        }

    };

    struct snapshot_midi_note_visitor final {

        template<json_integral_type T, json_path_type P>
        midi_note
        operator()(const json_number_node<T, P>& node) const
        {
            auto value =
                validate_snapshot_number<midi_note::scalar_type>(node);
            auto result = midi_note::create(value);
            if (! result) {
                throw json_validation_error(
                    std::format("{0} is not a valid MIDI note value", value));
            }
            return result.value();
        }

    };

    struct snapshot_midi_velocity_visitor final {

        template<json_integral_type T, json_path_type P>
        midi_velocity
        operator()(const json_number_node<T, P>& node) const
        {
            auto value =
                validate_snapshot_number<midi_velocity::scalar_type>(node);
            auto result = midi_velocity::create(value);
            if (! result) {
                throw json_validation_error(
                    std::format(
                        "{0} is not a valid MIDI velocity value", value));
            }
            return result.value();
        }

    };

    struct snapshot_midi_aftertouch_visitor final {

        template<json_path_type P>
        std::optional<midi_aftertouch>
        operator()(const json_null_node<P>& /*node*/) const
        {
            return std::nullopt;
        }

        template<json_integral_type T, json_path_type P>
        std::optional<midi_aftertouch>
        operator()(const json_number_node<T, P>& node) const
        {
            auto value =
                validate_snapshot_number<midi_aftertouch::scalar_type>(node);
            auto result = midi_aftertouch::create(value);
            if (! result) {
                throw json_validation_error(
                    std::format(
                        "{0} is not a valid MIDI aftertouch value", value));
            }
            return result.value();
        }

    };

    struct snapshot_midi_channel_pressure_visitor final {

        template<json_path_type P>
        std::optional<midi_channel_pressure>
        operator()(const json_null_node<P>& /*node*/) const
        {
            return std::nullopt;
        }

        template<json_integral_type T, json_path_type P>
        std::optional<midi_channel_pressure>
        operator()(const json_number_node<T, P>& node) const
        {
            auto value = validate_snapshot_number<
                midi_channel_pressure::scalar_type
            >(node);
            auto result = midi_channel_pressure::create(value);
            if (! result) {
                throw json_validation_error(
                    std::format(
                        "{0} is not a valid MIDI channel pressure value",
                        value));
            }
            return result.value();
        }

    };

    struct snapshot_midi_control_index_visitor final {

        template<json_integral_type T, json_path_type P>
        midi_control_index
        operator()(const json_number_node<T, P>& node) const
        {
            auto value = validate_snapshot_number<
                midi_control_index::scalar_type
            >(node);
            auto result = midi_control_index::create(value);
            if (! result) {
                throw json_validation_error(
                    std::format(
                        "{0} is not a valid MIDI control index", value));
            }
            return result.value();
        }

    };

    struct snapshot_midi_control_value_visitor final {

        template<json_integral_type T, json_path_type P>
        midi_control_value
        operator()(const json_number_node<T, P>& node) const
        {
            auto value = validate_snapshot_number<
                midi_control_value::scalar_type
            >(node);
            auto result = midi_control_value::create(value);
            if (! result) {
                throw json_validation_error(
                    std::format(
                        "{0} is not a valid MIDI control value", value));
            }
            return result.value();
        }

    };

    struct snapshot_midi_control_pair_visitor final {

        template<json_path_type P>
        std::pair<midi_control_index, midi_control_value>
        operator()(const json_array_node<P>& node) const
        {
            auto size = node.element().size();
            if (size != 2) [[unlikely]] {
                throw json_validation_error(
                    std::format(
                        "expected array of size 2, but found array of size "
                        "{0}",
                        size));
            }
            return std::make_pair(
                traverse(node, 0, snapshot_midi_control_index_visitor{}),
                traverse(node, 1, snapshot_midi_control_value_visitor{}));
        }

    };

    struct snapshot_midi_control_array_visitor final {

        template<json_path_type P>
        auto
        operator()(const json_array_node<P>& node) const
        {
            return std::ranges::to<midi_control_map>(
                std::views::transform(
                    std::views::iota(
                        snapshot_json_array_zero_index, node.element().size()),
                    [&node](std::size_t i) {
                        return traverse(
                            node, i, snapshot_midi_control_pair_visitor{});
                    }));
        }

    };

    struct snapshot_audio_duration_visitor final {

        template<json_integral_type T, json_path_type P>
        audio_duration
        operator()(const json_number_node<T, P>& node) const
        {
            auto value =
                validate_snapshot_number<audio_duration::scalar_type>(node);
            auto result = audio_duration::create(value);
            if (! result) {
                throw json_validation_error(
                    std::format("{0} is not a valid audio duration", value));
            }
            return result.value();
        }

    };

    class snapshot_effect_chain_index_visitor final {

    public:

        constexpr
        snapshot_effect_chain_index_visitor(std::size_t count) noexcept:
            count_(count)
        {
            // empty
        }

        template<json_path_type P>
        std::optional<std::size_t>
        operator()(const json_null_node<P>& /*node*/) const noexcept
        {
            return std::nullopt;
        }

        template<json_integral_type T, json_path_type P>
        std::optional<std::size_t>
        operator()(const json_number_node<T, P>& node) const
        {
            auto index = validate_snapshot_number<std::size_t>(node);
            if (index >= count_) [[unlikely]] {
                throw json_validation_error(
                    std::format(
                        "index {0} is greater than or equal to effect chain "
                        "count {1}",
                        index, count_));
            }
            return index;
        }

    private:

        std::size_t count_;

    };

    class snapshot_audio_source_visitor final {

    public:

        constexpr
        snapshot_audio_source_visitor(
            const audio_traits& traits,
            const std::filesystem::path& sample_directory_path,
            snapshot_sample_path_map& sample_path_map
        ) noexcept:
            traits_(traits),
            sample_directory_path_(sample_directory_path),
            sample_path_map_(sample_path_map)
        {
            // empty
        }

        template<json_path_type P>
        std::optional<audio_source>
        operator()(const json_null_node<P>& /*node*/) const noexcept
        {
            return std::nullopt;
        }

        template<json_path_type P>
        std::optional<audio_source>
        operator()(const json_string_node<P>& node)
        {
            std::filesystem::path archive_path(
                static_cast<std::string_view>(node.element()));
            synthclone::temporary_file audio_file(sample_directory_path_);
            const auto& audio_file_path = audio_file.path();
            audio_source source(audio_file_path, traits_);
            sample_path_map_.emplace(archive_path, audio_file_path);
            audio_file.release();
            return source;
        }

    private:

        const audio_traits& traits_;
        const std::filesystem::path& sample_directory_path_;
        snapshot_sample_path_map& sample_path_map_;

    };

    class snapshot_zone_visitor final {

    public:

        constexpr
        snapshot_zone_visitor(
            const audio_traits& traits,
            std::size_t effect_chain_count,
            const std::filesystem::path& sample_directory_path,
            snapshot_sample_path_map& sample_path_map
        ) noexcept:
            traits_(traits),
            effect_chain_count_(effect_chain_count),
            sample_directory_path_(sample_directory_path),
            sample_path_map_(sample_path_map)
        {
            // empty
        }

        template<json_path_type P>
        zone_snapshot
        operator()(const json_object_node<P>& node)
        {
            snapshot_audio_source_visitor audio_source_visitor(
                traits_, sample_directory_path_, sample_path_map_);
            auto dry_sample = traverse(
                node, "dry-sample", audio_source_visitor);
            auto wet_sample = traverse(
                node, "wet-sample", audio_source_visitor);
            if (
                (! dry_sample) && (static_cast<bool>(wet_sample))
            ) [[unlikely]] {
                throw json_validation_error(
                    "'wet-sample' cannot be set when 'dry-sample' is set to "
                    "'null'");
            }

            return zone_snapshot(
                {
                    .channel = traverse(
                        node, "channel", snapshot_midi_channel_visitor{}),
                    .note = traverse(
                        node, "note", snapshot_midi_note_visitor{}),
                    .velocity = traverse(
                        node, "velocity", snapshot_midi_velocity_visitor{}),
                    .aftertouch = traverse(
                        node, "aftertouch",
                        snapshot_midi_aftertouch_visitor{}),
                    .channel_pressure = traverse(
                        node, "channel-pressure",
                        snapshot_midi_channel_pressure_visitor{}),
                    .controls = traverse(
                        node, "controls",
                        snapshot_midi_control_array_visitor{}),
                    .sample_duration = traverse(
                        node, "sample-duration",
                        snapshot_audio_duration_visitor{}),
                    .release_duration = traverse(
                        node, "release-duration",
                        snapshot_audio_duration_visitor{}),
                    .effect_chain_index = traverse(
                        node, "effect-chain-index",
                        snapshot_effect_chain_index_visitor{
                            effect_chain_count_}),
                    .dry_sample = std::move(dry_sample),
                    .wet_sample = std::move(wet_sample)
                });
        }

    private:

        const audio_traits& traits_;
        std::size_t effect_chain_count_;
        const std::filesystem::path& sample_directory_path_;
        snapshot_sample_path_map& sample_path_map_;

    };

    class snapshot_zone_array_visitor final {

    public:

        constexpr
        snapshot_zone_array_visitor(
            const audio_traits& traits,
            std::size_t effect_chain_count
        ) noexcept:
            traits_(traits),
            effect_chain_count_(effect_chain_count)
        {
            // empty
        }

        template<json_path_type P>
        auto
        operator()(const json_array_node<P>& node) const
        {
            temporary_directory sample_directory;
            snapshot_sample_path_map sample_path_map;

            snapshot_zone_visitor zone_visitor(
                traits_, effect_chain_count_, sample_directory.path(),
                sample_path_map);

            // XXX: Using the lambda to preserve temporary directory state is a
            // hack.  We can do better.
            return std::make_pair(
                std::views::transform(
                    std::views::as_rvalue(
                        std::views::all(
                            std::ranges::to<std::vector<zone_snapshot>>(
                                std::views::transform(
                                    std::views::iota(
                                        snapshot_json_array_zero_index,
                                        node.element().size()),
                                    [&node, &zone_visitor](std::size_t i) {
                                        return traverse(node, i, zone_visitor);
                                    })))),
                    [sample_directory = std::move(sample_directory)](
                        zone_snapshot&& snapshot
                    ) -> zone_snapshot&& {
                        return std::move(snapshot);
                    }),
                sample_path_map);
        }

    private:

        const audio_traits& traits_;
        std::size_t effect_chain_count_;

    };

    struct snapshot_session_visitor final {

        template<json_path_type P>
        std::tuple<session_snapshot, app_version, snapshot_sample_path_map>
        operator()(const json_object_node<P>& node) const
        {
            // Eventually, we may need to do something different depending on
            // the version we find in the JSON.
            auto version = traverse(
                node, "version", snapshot_version_visitor{});

            auto sample_rate = traverse(
                node, "sample-rate", snapshot_sample_rate_visitor{});
            auto channel_count = traverse(
                node, "channel-count", snapshot_channel_count_visitor{});
            auto endianness = traverse(
                node, "endianness", snapshot_endianness_visitor{});

            audio_traits traits(
                audio_format::raw, audio_codec::pcm_f32, endianness,
                sample_rate, channel_count);

            auto effect_chains = traverse(
                node, "effect-chains", snapshot_effect_chain_array_visitor{});
            auto [zones, sample_path_map] = traverse(
                node, "zones",
                snapshot_zone_array_visitor(traits, effect_chains.size()));

            return std::make_tuple(
                session_snapshot(
                    {
                        .sample_rate = sample_rate,
                        .channel_count = channel_count,
                        .effect_chains = std::views::as_rvalue(
                            std::views::all(std::move(effect_chains))),
                        .exporters = std::views::as_rvalue(
                            std::views::all(
                                traverse(
                                    node, "exporters",
                                    snapshot_component_array_visitor{}))),
                        .importers = std::views::as_rvalue(
                            std::views::all(
                                traverse(
                                    node, "importers",
                                    snapshot_component_array_visitor{}))),
                        .sampler = traverse(
                            node, "sampler", snapshot_sampler_visitor{}),
                        .zones = std::move(zones)
                    }),
                version, std::move(sample_path_map));
        }

    };

    struct snapshot_root_visitor final {

        template<json_path_type P>
        std::tuple<session_snapshot, app_version, snapshot_sample_path_map>
        operator()(const json_object_node<P>& node) const
        {
            return traverse(
                node, "synthclone-session", snapshot_session_visitor{});
        }

    };

    constexpr app_version snapshot_version;

    /**
     * Restores a snapshot from a snapshot archive on disk.
     *
     * @param path
     *   The path to the snapshot archive to restore.
     * @param result
     *   The out parameter that will contain the session snapshot when the
     *   message generator has been exhausted.
     *
     * @return
     *   A generator that emits messages about the session restoration
     *   operation.  Note that the generator *must* be exhausted before the
     *   restoration operation is considered complete.
     */

    export
    std::generator<snapshot_restore_message>
    restore_snapshot(
        const std::filesystem::path& path,
        out_param<session_snapshot>& result
    )
    {
        // XXX: This function isn't emitting `operation_progress_message`
        // instances yet.

        // Open the session snapshot.  The session snapshot is a .tar.gz
        // archive.
        co_yield operation_status_message("opening session snapshot archive");

        auto input_stream = open_ifstream(
            path, std::ios_base::binary | std::ios_base::in);

        boost::iostreams::filtering_stream<boost::iostreams::input>
            decompressor_stream;
        decompressor_stream.push(boost::iostreams::gzip_decompressor());
        decompressor_stream.push(input_stream);

        tar_istream_view view(decompressor_stream);

        // The first entry should be a non-zero size file named
        // `snapshot.json`.
        co_yield operation_status_message("finding session snapshot JSON");

        auto archive_end = view.end();
        auto archive_iter = view.begin();
        if (archive_iter == archive_end) [[unlikely]] {
            throw snapshot_error(
                std::format(
                    "{0:?} is not a valid session snapshot: archive is empty",
                    path.string()));
        }

        std::visit(snapshot_json_header_visitor(path), *archive_iter);

        // Parse the JSON file.
        co_yield operation_status_message("parsing snapshot JSON");

        boost::json::stream_parser json_parser;
        snapshot_json_fragment_visitor json_fragment_visitor(
            path, json_parser);
        for (++archive_iter; archive_iter != archive_end; ++archive_iter) {
            if (! std::visit(json_fragment_visitor, *archive_iter)) {
                break;
            }
        }

        std::error_code error_code;
        json_parser.finish(error_code);
        if (error_code) [[unlikely]] {
            throw snapshot_error(
                std::format(
                    "{0:?} is not a valid session snapshot: `snapshot.json` "
                    "not valid: {1}",
                    path.string(), error_code.message()));
        }

        auto root_node = json_parser.release();

        // Validate the JSON.
        co_yield operation_status_message("validating snapshot JSON");
        auto [session_snapshot, archive_version, sample_path_map] = traverse(
            root_node, snapshot_root_visitor{});

        if (snapshot_version != archive_version) {
            co_yield operation_warning_message(
                std::format(
                    "session was saved by version {0}.{1}.{2}, but current "
                    "version is {3}.{4}.{5}",
                    archive_version.major(), archive_version.minor(),
                    archive_version.patch(), snapshot_version.major(),
                    snapshot_version.minor(), snapshot_version.patch()));
        }

        // Unpack archived samples.
        co_yield operation_status_message("unpacking archived samples");

        snapshot_sample_header_visitor sample_header_visitor(
            path, session_snapshot.channel_count(), sample_path_map);
        while (archive_iter != archive_end) {

        process_header:
            auto visit_result = std::visit(
                sample_header_visitor, *archive_iter);
            if (visit_result == std::nullopt) {
                ++archive_iter;
                continue;
            }

            auto [result_path, is_sample] = *visit_result;
            if (! is_sample) [[unlikely]] {
                co_yield operation_warning_message(
                    std::format(
                        "ignoring archive entry for {0:?}",
                        result_path.string()));
                archive_iter.advance_header();
                continue;
            }

            snapshot_sample_fragment_visitor sample_fragment_visitor(
                result_path);
            for (++archive_iter;; ++archive_iter) {
                if (archive_iter == archive_end) {
                    goto archive_done;
                }
                if (! std::visit(sample_fragment_visitor, *archive_iter)) {
                    goto process_header;
                }
            }
        }

archive_done:
        if (! sample_path_map.empty()) [[unlikely]] {
            throw snapshot_error(
                std::format(
                    "{0:?} is not a valid session snapshot: {1} samples "
                    "referenced in `snapshot.json` were not found in archive",
                    path.string(), sample_path_map.size()));
        }

        result = std::move(session_snapshot);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::snapshot_save_message
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Variant type containing types of messages that may be emitted in a
     * `save_snapshot()` operation.
     */

    export
    using snapshot_save_message = std::variant<
        operation_progress_message,
        operation_status_message,
        operation_warning_message
    >;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::save_snapshot()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    template<class V>
    boost::json::array
    build_json_snapshot_array(V view)
    {
        boost::json::array array;
        array.reserve(view.size());
        for (const auto& value: view) {
            array.push_back(value);
        }
        return array;
    }

    template<class T>
    boost::json::value
    dump_optional_snapshot_value(const std::optional<T>& element)
    {
        return element ? boost::json::value(*element) :
            boost::json::value(nullptr);
    }

    struct component_snapshot_state_dump_visitor final {

        template<class T>
        boost::json::value
        operator()(const T& value)
        {
            return value;
        }

        boost::json::value
        operator()(const state_string& str)
        {
            return boost::json::string_view(str.data(), str.size());
        }

        boost::json::value
        operator()(const state_array& array)
        {
            auto view = std::views::transform(
                array,
                [this](const state_value& value) -> boost::json::value {
                    return value.visit(*this);
                });
            return boost::json::array(view.begin(), view.end());
        }

        boost::json::value
        operator()(const state_map& map)
        {
            auto view = std::views::transform(
                map,
                [this](
                    const state_map::value_type& p
                ) -> boost::json::key_value_pair {
                    const auto& key = p.first;
                    return boost::json::key_value_pair(
                        boost::json::string_view(key.data(), key.size()),
                        p.second.visit(*this));
                });
            return boost::json::object(view.begin(), view.end());
        }

    };

    template<class T>
    boost::json::value
    dump_component_snapshot(const T& element)
    {
        const auto& state = element.state();
        return state ?
            boost::json::object {
                {"plugin-identifier", element.plugin_identifier()},
                {"type-identifier", element.type_identifier()},
                {"type-version", element.type_version()},
                {
                    "state",
                    state->visit(component_snapshot_state_dump_visitor{})
                }
            } :
            boost::json::object {
                {"plugin-identifier", element.plugin_identifier()},
                {"type-identifier", element.type_identifier()},
                {"type-version", element.type_version()}
            };
    }

    boost::json::value
    dump_component_snapshot_sequence(component_snapshot_view&& view)
    {
        return build_json_snapshot_array(
            std::views::transform(
                std::move(view),
                [](const component_snapshot& snapshot) {
                    return dump_component_snapshot(snapshot);
                }));
    }

    boost::json::object
    dump_effect_chain_snapshot(effect_chain_snapshot&& snapshot)
    {
        return {
            {"name", snapshot.name()},
            {
                "effects",
                dump_component_snapshot_sequence(std::move(snapshot).effects())
            }
        };
    }

    boost::json::array
    dump_effect_chain_snapshot_sequence(effect_chain_snapshot_view&& view)
    {
        return build_json_snapshot_array(
            std::views::transform(
                std::views::as_rvalue(std::move(view)),
                [](effect_chain_snapshot&& snapshot) {
                    return dump_effect_chain_snapshot(std::move(snapshot));
                }));
    }

    boost::json::array
    dump_zone_snapshot_controls(const midi_control_map& controls)
    {
        return build_json_snapshot_array(
            std::views::transform(
                std::views::all(controls),
                [](const midi_control_map::value_type& pair) {
                    return boost::json::array {
                        pair.first.value(),
                        pair.second.value()
                    };
                }));
    }

    boost::json::value
    dump_zone_snapshot_sample(
        const std::optional<audio_source>& sample,
        const std::filesystem::path& samples_directory,
        const audio_traits& traits,
        audio_copier& copier,
        std::size_t& count
    )
    {
        if (! sample) {
            return boost::json::value(nullptr);
        }

        {
            audio_input_stream input_stream(*sample);
            audio_output_stream output_stream(
                samples_directory / std::to_string(count), traits);
            for (auto p: copier.copy(input_stream, output_stream)) {
                // XXX: We currently ignore copier updates.  This probably
                // isn't the *best* thing to do.
            }
        }

        boost::json::value s(std::format("samples/{0}", count));
        ++count;
        return s;
    }

    boost::json::object
    dump_zone_snapshot(
        const zone_snapshot& snapshot,
        const std::filesystem::path& samples_directory_path,
        const audio_traits& traits,
        audio_copier& copier,
        std::size_t& sample_count
    )
    {
        return {
            {"channel", snapshot.channel().value()},
            {"note", snapshot.note().value()},
            {"velocity", snapshot.velocity().value()},
            {
                "aftertouch",
                dump_optional_snapshot_value(snapshot.aftertouch())
            },
            {
                "channel-pressure",
                dump_optional_snapshot_value(snapshot.channel_pressure())
            },
            {"controls", dump_zone_snapshot_controls(snapshot.controls())},
            {"sample-duration", snapshot.sample_duration().value()},
            {"release-duration", snapshot.release_duration().value()},
            {
                "effect-chain-index",
                dump_optional_snapshot_value(snapshot.effect_chain_index())
            },
            {
                "dry-sample",
                dump_zone_snapshot_sample(
                    snapshot.dry_sample(), samples_directory_path, traits,
                    copier, sample_count)
            },
            {
                "wet-sample",
                dump_zone_snapshot_sample(
                    snapshot.wet_sample(), samples_directory_path, traits,
                    copier, sample_count)
            }
        };
    }

    boost::json::array
    dump_zone_snapshot_sequence(
        zone_snapshot_view&& view,
        const std::filesystem::path& samples_directory_path,
        const audio_traits& traits,
        audio_copier& copier,
        std::size_t& sample_count
    )
    {
        return build_json_snapshot_array(
            std::views::transform(
                std::move(view),
                [&samples_directory_path, &traits, &copier, &sample_count](
                    const zone_snapshot& zone
                ) mutable {
                    return dump_zone_snapshot(
                        zone, samples_directory_path, traits, copier,
                        sample_count);
                }));
    }

    constexpr audio_endianness snapshot_endianness =
        std::endian::native == std::endian::big ? audio_endianness::big :
        audio_endianness::little;
    constexpr std::string snapshot_endianness_str(
        snapshot_endianness == audio_endianness::big ? "big" : "little");

    /**
     * Saves a snapshot to a snapshot archive on disk.
     *
     * @param snapshot
     *   The snapshot to save.
     * @param path
     *   The path to save the snapshot archive to.
     * @param io_buffer
     *   A buffer to use for archive I/O.
     *
     * @return
     *   A generator that emits messages about the session save operation.
     *   Note that the generator *must* be exhausted before the save operation
     *   is considered complete.
     */

    export
    std::generator<snapshot_save_message>
    save_snapshot(
        session_snapshot&& snapshot,
        const std::filesystem::path& snapshot_path,
        std::span<std::byte> io_buffer
    )
    {
        // XXX: This function isn't emitting `operation_progress_message`
        // instances yet.

        // This is a *long* function, but it's mostly straightforward.  Don't
        // be intimidated by its size.

        // Dump all of the samples we'll store in the archive while creating
        // the JSON configuration.
        co_yield operation_status_message(
            "generating JSON configuration and dumping samples");

        temporary_directory samples_directory;
        const auto& samples_directory_path = samples_directory.path();

        auto channel_count = snapshot.channel_count();
        auto sample_rate = snapshot.sample_rate();

        audio_copier copier;
        audio_traits traits(
            audio_format::raw, audio_codec::pcm_f32, snapshot_endianness,
            sample_rate, channel_count);
        std::size_t sample_count = 0;

        const auto& sampler = snapshot.sampler();
        auto snapshot_json_str = boost::json::serialize(
            boost::json::object {
                {
                    "synthclone-session",
                    {

                        {
                            "version",
                            {
                                {"major", snapshot_version.major()},
                                {"minor", snapshot_version.minor()},
                                {"patch", snapshot_version.patch()}
                            }
                        },

                        {"sample-rate", sample_rate.value()},
                        {"channel-count", channel_count.value()},
                        {"endianness", snapshot_endianness_str},

                        {
                            "sampler",
                            sampler ? dump_component_snapshot(*sampler) :
                                boost::json::value(nullptr)
                        },

                        {
                            "effect-chains",
                            dump_effect_chain_snapshot_sequence(
                                std::move(snapshot).effect_chains())
                        },

                        {
                            "exporters",
                            dump_component_snapshot_sequence(
                                std::move(snapshot).exporters())
                        },

                        {
                            "importers",
                            dump_component_snapshot_sequence(
                                std::move(snapshot).importers())
                        },

                        {
                            "zones",
                            dump_zone_snapshot_sequence(
                                std::move(snapshot.zones()),
                                samples_directory_path, traits, copier,
                                sample_count)
                        }
                    }
                }
            });

        // Create a .tar.gz archive.  Put the archive in a temporary file as
        // we're building it so that we don't modify the actual path if an
        // error occurs.
        //
        // XXX: Consider adding options to specify compressor configuration.
        co_yield operation_status_message(
            "generating session snapshot archive");

        temporary_file archive_file;
        const auto& archive_file_path = archive_file.path();

        {
            auto output_stream = open_ofstream(
                archive_file_path,
                std::ios_base::binary | std::ios_base::out);

            boost::iostreams::filtering_stream<boost::iostreams::output>
                compressor_stream;
            compressor_stream.push(boost::iostreams::gzip_compressor());
            compressor_stream.push(output_stream);

            tar_ostream_iterator iter(compressor_stream);

            // Write the JSON to the archive.
            co_yield operation_status_message("writing session snapshot JSON");

            *iter = tar_file_header(
                {
                    .path = "snapshot.json",
                    .size = static_cast<archive_size_t>(
                        snapshot_json_str.size())
                });
            ++iter;

            *iter = archive_file_fragment(
                std::as_bytes(std::span(snapshot_json_str)));
            ++iter;

            // Write the samples directory to the archive.
            co_yield operation_status_message("writing samples directory");

            *iter = tar_directory_header({.path = "samples/"});
            ++iter;

            // Write the samples to the archive.
            for (std::size_t i = 0; i < sample_count; ++i) {
                co_yield operation_status_message(
                    std::format("writing sample {0} of {1}", i, sample_count));
                add_file(
                    tar_minimal_header, iter,
                    samples_directory_path / std::to_string(i),
                    std::format("samples/{0}", i), io_buffer);
            }
        }

        // Move the archive to the given snapshot path.
        co_yield operation_status_message(
            std::format("moving archive to {0:?}", snapshot_path.string()));

        // XXX: Cross device link failures.
        std::filesystem::rename(archive_file_path, snapshot_path);
        archive_file.release();
    }

    /**
     * Saves a snapshot to a snapshot archive on disk.
     *
     * @param snapshot
     *   The snapshot to save.
     * @param path
     *   The path to save the snapshot archive to.
     *
     * @return
     *   A generator that emits messages about the session save operation.
     *   Note that the generator *must* be exhausted before the save operation
     *   is considered complete.
     */

    export
    std::generator<snapshot_save_message>
    save_snapshot(
        session_snapshot&& snapshot,
        const std::filesystem::path& snapshot_path
    )
    {
        dynamic_buffer<std::byte> io_buffer(
            boost::interprocess::mapped_region::get_page_size());

        // We need to `co_yield` instead of just returning the generator
        // because `io_buffer` needs to stay alive.
        co_yield std::ranges::elements_of(
            save_snapshot(std::move(snapshot), snapshot_path, io_buffer));
    }

}
