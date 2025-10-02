/**
 * @file
 *
 * Contains audio streaming facilities.
 */

module;

#include <synthclone/config.h>

export module synthclone.core:audio_io;

import std;

import synthclone.external.boost;
import synthclone.util;

import :audio_core;
import :audio_gen;
import :sndfile_io;

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_input_stream
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    void
    verify_audio_buffer(
        std::span<const audio_sample> buffer,
        const audio_traits& traits
    )
    {
        verify(buffer.data() != nullptr, "buffer pointer must not be NULL");

        const auto size = buffer.size();
        verify(size != 0, "buffer size must not be 0");

        const auto channel_count = traits.channel_count().value();
        verify(
            (size % channel_count) == 0,
            "buffer size {0} is not evenly divisible by channel count {1}",
            size, channel_count);
    }

    void
    verify_open_stream(const bool closed)
    {
        verify(! closed, "stream is not associated with an open file");
    }

    void
    verify_seek_origin(const audio_seek_origin origin)
    {
        verify(
            static_cast<std::size_t>(origin) < audio_seek_origin_count,
            "'{0}': invalid `audio_seek_origin` constant",
            get_identifier(origin));
    }

    /**
     * Allows for reading of samples in audio files.
     */

    export
    class audio_input_stream final: private noncopyable {

    public:

        /**
         * Constructs an `audio_input_stream` instance that references the
         * audio stored in the file at the given path.
         *
         * Audio traits will be inferred from the file contents.
         *
         * @param path
         *   The file path.
         */

        inline explicit
        audio_input_stream(const std::filesystem::path& path):
            impl_(path)
        {
            // empty
        }

        /**
         * Constructs an `audio_input_stream` instance that references the
         * audio stored in the file at the given path.
         *
         * @param path
         *   The file path.
         * @param traits
         *   The audio traits.
         */

        inline
        audio_input_stream(
            const std::filesystem::path& path,
            const audio_traits& traits
        ):
            impl_(path, traits)
        {
            // empty
        }

        /**
         * Closes the stream.
         */

        inline
        void
        close()
        {
            verify_open_stream(impl_.closed());
            impl_.close();
        }

        /**
         * Gets a boolean indicating whether or not the stream is closed.
         *
         * @return
         *   The boolean indicator.
         */

        inline
        bool
        closed()
        const noexcept
        {
            return impl_.closed();
        }

        /**
         * Reads audio samples into the given buffer.
         *
         * The number of samples read into the buffer will be the same as the
         * size of the buffer unless less samples are available for reading.
         *
         * @param buffer
         *   The buffer to read samples into.  The size of the buffer must be
         *   a value such that `(size % channels) == 0` so that read operations
         *   result in complete frames and end on frame boundaries.
         *
         * @return
         *   The number of samples read.
         */

        audio_frame_count
        read(std::span<audio_sample> buffer)
        {
            verify_open_stream(impl_.closed());
            verify_audio_buffer(buffer, impl_.traits());
            return impl_.read(buffer);
        }

        /**
         * Sets the position of the stream in the sample file.
         *
         * @param frames
         *   The frame count from the offset where the stream should be
         *   positioned.
         *
         * @param offset
         *   The offset for stream positioning.
         *
         * @returns
         *   The new position.
         */

        inline
        audio_frame_count
        seek(const audio_seek_offset frames, const audio_seek_origin origin)
        {
            verify_open_stream(impl_.closed());
            verify_seek_origin(origin);
            return impl_.seek(frames, origin);
        }

        /**
         * Gets a boolean indicating whether or not `seek()` and `tell()`
         * operations will work with the current stream.
         *
         * @returns
         *   The boolean indicator.
         */

        inline
        bool
        seekable()
        const
        {
            verify_open_stream(impl_.closed());
            return impl_.seekable();
        }

        /**
         * Gets the position of the stream in the sample file.
         *
         * @returns
         *   The current position.
         */

        inline
        audio_frame_count
        tell()
        {
            verify_open_stream(impl_.closed());
            return impl_.tell();
        }

        /**
         * Gets the characteristics of the audio in the input stream.
         *
         * @return
         *   The audio traits.
         */

        constexpr
        const audio_traits&
        traits()
        const noexcept
        {
            return impl_.traits();
        }

    private:

        audio_input_stream() = delete;

        sndfile_input_stream impl_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_output_stream
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Allows for the writing of samples to audio files.
     */

    export class audio_output_stream final: private noncopyable {

    public:

        /**
         * Constructs an `audio_output_stream` instance that writes audio to
         * the file at the given path with characteristics specified by the
         * given traits.
         *
         * @param path
         *   The file path.
         * @param traits
         *   The audio traits.
         */

        inline
        audio_output_stream(
            const std::filesystem::path& path,
            const audio_traits& traits
        ):
            impl_(path, traits)
        {
            // empty
        }

        /**
         * Closes the stream.
         */

        inline
        void
        close()
        {
            verify_open_stream(impl_.closed());
            impl_.close();
        }

        /**
         * Gets a boolean indicating whether or not the stream is closed.
         *
         * @return
         *   The boolean indicator.
         */

        inline
        bool
        closed() const noexcept
        {
            return impl_.closed();
        }

        /**
         * Sets the position of the stream in the sample file.
         *
         * @param frames
         *   The frame count from the offset where the stream should be
         *   positioned.
         *
         * @param offset
         *   The offset for stream positioning.
         *
         * @returns
         *   The new position.
         */

        inline
        audio_frame_count
        seek(const audio_seek_offset frames, const audio_seek_origin origin)
        {
            verify_open_stream(impl_.closed());
            verify_seek_origin(origin);
            return impl_.seek(frames, origin);
        }

        /**
         * Gets a boolean indicating whether or not `seek()` and `tell()`
         * operations will work with the current stream.
         *
         * @returns
         *   The boolean indicator.
         */

        inline
        bool
        seekable()
        {
            verify_open_stream(impl_.closed());
            return impl_.seekable();
        }

        /**
         * Gets the position of the stream in the sample file.
         *
         * @returns
         *   The current position.
         */

        inline
        audio_frame_count
        tell()
        {
            verify_open_stream(impl_.closed());
            return impl_.tell();
        }

        /**
         * Gets the characteristics of the audio in the output stream.
         *
         * @return
         *   The audio traits.
         */

        constexpr
        const audio_traits&
        traits() const noexcept
        {
            return impl_.traits();
        }

        /**
         * Writes audio samples in the given buffer to the stream.
         *
         * @param buffer
         *   The buffer containing the samples to write.  The size of the
         *   buffer must be a value such that `(size % channels) == 0` so that
         *   write operations result in complete frames and end on frame
         *   boundaries.
         */

        void
        write(std::span<const audio_sample> buffer)
        {
            verify_open_stream(impl_.closed());
            verify_audio_buffer(buffer, impl_.traits());
            impl_.write(buffer);
        }

    private:

        audio_output_stream() = delete;

        sndfile_output_stream impl_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::is_streamable()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Gets a boolean indicating whether or not the given `audio_traits` can be
     * used for audio I/O.
     *
     * @param traits
     *   The traits to check.
     *
     * @return
     *   The boolean indicator.
     */

    export
    inline
    bool
    is_streamable(const audio_traits& traits)
    {
        return is_sndfile_streamable(traits);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_copier
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    audio_frame_count
    flush_and_copy(
        audio_resampler& resampler,
        std::span<audio_sample> output_span,
        audio_output_stream& stream
    )
    {
        const auto channel_count = stream.traits().channel_count().value();
        audio_frame_count total_frames_written = 0;

        for (
            auto output_frames_generated = resampler.flush(output_span);
            output_frames_generated != 0;
            output_frames_generated = resampler.flush(output_span)
        ) {
            stream.write(
                output_span.first(output_frames_generated * channel_count));
            total_frames_written += output_frames_generated;
        }

        return total_frames_written;
    }

    audio_frame_count
    resample_and_copy(
        audio_resampler& resampler,
        std::span<const audio_sample> input_span,
        std::span<audio_sample> output_span,
        audio_output_stream& stream
    )
    {
        const auto channel_count = stream.traits().channel_count().value();
        auto input_frame_count = input_span.size() / channel_count;

        audio_frame_count total_frames_written = 0;

        for (;;) {
            const auto [input_frames_used, output_frames_generated] =
                resampler.resample(input_span, output_span);

            if (output_frames_generated != 0) [[likely]] {
                stream.write(
                    output_span.first(
                        output_frames_generated * channel_count));
                total_frames_written += output_frames_generated;
            }

            input_frame_count -= input_frames_used;
            if (input_frame_count == 0) [[likely]] {
                break;
            }

            input_span = input_span.subspan(input_frames_used * channel_count);
        }

        return total_frames_written;
    }

    /**
     * Copies audio from an `audio_input_stream` (at the input stream's current
     * position) to an `audio_output_stream` (at the output stream's current
     * position), resampling the audio if necessary.
     */

    export
    class audio_copier final: private noncopyable {

    public:

        /**
         * Constructs an `audio_copier` instance with an input buffer size
         * based on the OS page size.
         */

        inline
        audio_copier():
            input_buffer_(
                boost::interprocess::mapped_region::get_page_size() /
                sizeof(audio_sample))
        {
            // empty
        }

        /**
         * Copies at most `max_frames` frames of audio from the given
         * `audio_input_stream` to the given `audio_output_stream`.
         *
         * This method creates a coroutine that's fronted by the returned
         * generator.  The generator must be progressed for this method to
         * progress.
         *
         * @param input_stream
         *   The input stream to copy audio from.
         * @param output_stream
         *   The output stream to copy audio to.
         * @param max_frames
         *   The maximum amount of frames to copy.
         *
         * @return
         *   A generator object that yields pairs of `audio_frame_count`
         *   values, each pair containing, in order, the amount of frames read
         *   from the input stream and the amount of frames written to the
         *   output stream.
         */

        std::generator<std::pair<audio_frame_count, audio_frame_count>>
        copy(
            audio_input_stream& input_stream,
            audio_output_stream& output_stream,
            audio_frame_count max_frames =
                std::numeric_limits<audio_frame_count>::max()
        )
        {
            const auto& input_traits = input_stream.traits();
            const auto& output_traits = output_stream.traits();

            const auto channel_count = input_traits.channel_count();
            const auto output_channel_count = output_traits.channel_count();

            verify(
                channel_count == output_channel_count,
                "input stream channel count ({0}) does not match output "
                "stream channel count ({1})",
                channel_count, output_channel_count);

            if (max_frames == 0) [[unlikely]] {
                co_return;
            }

            std::size_t frames_per_read;
            std::size_t input_span_size;
            const auto channel_count_value = channel_count.value();

            const auto input_buffer_size = input_buffer_.size();
            if (input_buffer_size < channel_count_value) [[unlikely]] {
                input_buffer_.resize(channel_count_value);
                frames_per_read = 1;
                input_span_size = channel_count_value;
            } else {
                frames_per_read = input_buffer_size / channel_count_value;
                input_span_size = frames_per_read * channel_count_value;
            }
            auto input_span = std::span(
                input_buffer_.begin(), input_span_size);

            // If the sample rates are the same, then we don't need to bother
            // with resampling or using a separate output buffer.
            const auto input_sample_rate = input_traits.sample_rate();
            const auto output_sample_rate = output_traits.sample_rate();
            audio_frame_count frames_read;
            if (input_sample_rate == output_sample_rate) {
                for (
                    ;
                    max_frames >= frames_per_read;
                    max_frames -= frames_per_read
                ) [[likely]] {
                    frames_read = input_stream.read(input_span);
                    if (frames_read != frames_per_read) [[unlikely]] {
                        if (frames_read != 0) {
                            output_stream.write(
                                input_span.subspan(
                                    0, frames_read * channel_count_value));
                            co_yield {frames_read, frames_read};
                        }
                        co_return;
                    }
                    output_stream.write(input_span);
                    co_yield {frames_read, frames_read};
                }
                if (max_frames != 0) {
                    input_span = input_span.subspan(
                        0, max_frames * channel_count_value);
                    frames_read = input_stream.read(input_span);
                    if (frames_read != 0) {
                        output_stream.write(
                            input_span.subspan(
                                0, frames_read * channel_count_value));
                        co_yield {frames_read, frames_read};
                    }
                }
                co_return;
            }

            // Looks like we'll need to resample after all.  Hmmph.
            audio_resampler resampler(
                input_sample_rate, output_sample_rate, channel_count);

            // The minimum output buffer size is selected based on how many
            // samples we think we'll need when a full input buffer worth of
            // samples is resampled.
            const auto min_output_buffer_size = static_cast<std::size_t>(
                std::ceil(
                    static_cast<double>(frames_per_read) *
                    (
                        static_cast<double>(output_sample_rate.value()) /
                        static_cast<double>(input_sample_rate.value())
                    )
                ) *
                channel_count_value);
            const auto output_buffer_size = output_buffer_.size();
            std::size_t output_span_size;
            if (output_buffer_size < min_output_buffer_size) {
                output_buffer_.resize(min_output_buffer_size);
                output_span_size = min_output_buffer_size;
            } else {
                output_span_size = output_buffer_size -
                    (output_buffer_size % channel_count_value);
            }
            auto output_span = std::span(
                output_buffer_.begin(), output_span_size);

            audio_frame_count frames_written;
            for (
                ;
                max_frames >= frames_per_read;
                max_frames -= frames_per_read
            ) [[likely]] {

                frames_read = input_stream.read(input_span);

                if (frames_read != frames_per_read) [[unlikely]] {
                    if (frames_read != 0) {
                        frames_written = resample_and_copy(
                            resampler,
                            input_span.subspan(
                                0, frames_read * channel_count_value),
                            output_span, output_stream);
                    } else {
                        frames_written = 0;
                    }
                    frames_written += flush_and_copy(
                        resampler, output_span, output_stream);

                    co_yield {frames_read, frames_written};
                    co_return;
                }

                frames_written = resample_and_copy(
                    resampler, input_span, output_span, output_stream);

                co_yield {frames_read, frames_written};
            }

            if (max_frames == 0) {
                frames_read = 0;
                frames_written = flush_and_copy(
                    resampler, output_span, output_stream);
            } else {
                input_span = input_span.subspan(
                    0, max_frames * channel_count_value);
                frames_read = input_stream.read(input_span);
                if (frames_read != 0) {
                    frames_written = resample_and_copy(
                        resampler,
                        input_span.subspan(
                            0, frames_read * channel_count_value),
                        output_span, output_stream);
                } else {
                    frames_written = 0;
                }
                frames_written += flush_and_copy(
                    resampler, output_span, output_stream);
            }
            co_yield {frames_read, frames_written};
        }

    private:

        dynamic_buffer<audio_sample> input_buffer_;
        dynamic_buffer<audio_sample> output_buffer_;

    };

}
