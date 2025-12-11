export module synthclone.fx:reverser;

import std;

import synthclone.core;
import synthclone.external.boost.interprocess;
import synthclone.plugin;
import synthclone.qt;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::make_reverser_type()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    class reverser_instance final: public capture_effect_instance {

    public:

        explicit
        reverser_instance():
            reverser_instance(
                boost::interprocess::mapped_region::get_page_size())
        {
            // empty
        }

        explicit
        reverser_instance(const std::size_t initial_buffer_size):
            buffer_(initial_buffer_size)
        {
            // empty
        }

        constexpr
        const dynamic_buffer<audio_sample>&
        buffer()
        const noexcept
        {
            return buffer_;
        }

        constexpr
        dynamic_buffer<audio_sample>&
        buffer()
        noexcept
        {
            return buffer_;
        }

    private:

        dynamic_buffer<audio_sample> buffer_;

    };

    void
    reverse_chunk(
        audio_input_stream& input_stream,
        std::span<audio_sample> buffer_span,
        audio_seek_offset offset,
        audio_output_stream& output_stream
    )
    {
        input_stream.seek(offset, audio_seek_origin::start);

        auto channel_count = input_stream.traits().channel_count().value();
        auto expected_read_count = buffer_span.size() / channel_count;
        auto read_count = input_stream.read(buffer_span);
        assume(
            read_count == expected_read_count,
            "expected to read {0} frames, but read {1} frames",
            expected_read_count, read_count);

        // Cache friendly in-place reversal.
        auto last_frame = buffer_span.end() - channel_count;
        for (audio_channel_count::scalar_type i = 0; i < channel_count; ++i) {
            for (
                auto buffer_left = buffer_span.begin() + i,
                buffer_right = last_frame + i;
                buffer_left < buffer_right;
                buffer_left += channel_count,
                buffer_right -= channel_count
            ) {
                std::swap(*buffer_left, *buffer_right);
            }
        }

        output_stream.write(buffer_span);
    }

    class reverser_core_ops final: public capture_effect_core_ops {

    public:

        reverser_core_ops(session_host& host):
            host_(host)
        {
            // empty
        }

        std::unique_ptr<capture_effect_instance>
        create()
        override final
        {
            return std::make_unique<reverser_instance>();
        }

        std::generator<capture_effect_run_message>
        run(
            capture_effect_instance& instance,
            audio_input_stream& input_stream,
            audio_output_stream& output_stream,
            std::stop_token /*stop_token*/
        )
        override final
        {
            auto& buffer = static_cast<reverser_instance&>(instance).buffer();

            auto size = buffer.size();
            auto channel_count = input_stream.traits().channel_count().value();
            if (size < channel_count) {
                host_.logger().log(
                    session_log_level::debug,
                    "increasing buffer size from {0} to {1} to accommodate "
                    "{1} channels of data",
                    size, channel_count);

                buffer.resize(channel_count);
                size = channel_count;

                co_yield component_state_changed_message();
            }

            co_yield component_status_message("Reversing ...");

            input_stream.seek(0, audio_seek_origin::end);
            auto total_frame_count = input_stream.tell();
            if (total_frame_count == 0) [[unlikely]] {
                co_yield component_progress_message(1.0);
                co_return;
            }

            std::span<audio_sample> buffer_span(buffer.data(), size);
            auto frame_count = size / channel_count;
            audio_seek_offset next_offset = frame_count;
            for (
                ;
                next_offset < total_frame_count;
                next_offset += frame_count
            ) {
                reverse_chunk(
                    input_stream, buffer_span, total_frame_count - next_offset,
                    output_stream);

                co_yield component_progress_message(
                    static_cast<float>(next_offset) /
                    static_cast<float>(total_frame_count));
            }

            reverse_chunk(
                input_stream,
                buffer_span.subspan(
                    0,
                    (frame_count - (next_offset - total_frame_count)) *
                    channel_count),
                0, output_stream);

            co_yield component_progress_message(1.0);
        }

    private:

        session_host& host_;

    };

    struct reverser_state_ops final: public capture_effect_state_ops {

        state_value
        dump(const capture_effect_instance& instance)
        override final
        {
            const auto& reverser = static_cast<const reverser_instance&>(
                instance);
            return state_map {
                { "buffer-size", reverser.buffer().size() }
            };
        }

        std::unique_ptr<capture_effect_instance>
        load(const state_value& state)
        override final
        {
            return std::make_unique<reverser_instance>(
                extract_number<std::size_t>(
                    extract_map(state).at("buffer-size")));
        }

    };

    export
    capture_effect_type
    make_reverser_type(session_host& host)
    {
        return capture_effect_type(
            {
                .core_ops = std::make_unique<reverser_core_ops>(host),
                .state_ops = std::make_unique<reverser_state_ops>(),
                .metadata = generate_simple_metadata(
                    {
                        .identifier = "synthclone.fx.reverser",
                        .title = "reverser",
                        .description = "Reverses the order of audio frames."
                    })
            });
    }

}
