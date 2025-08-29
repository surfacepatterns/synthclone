/**
 * @file
 *
 * Contains audio type definitions and I/O facilities.
 */

module;

#include <stdfloat>

#include <samplerate.h>

#include <synthclone/config.h>

export module synthclone.core:audio_core;

import std;

import synthclone.util;

import :audio_gen;

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_error
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Identifies an error specific to audio functionality.
     */

    export class audio_error: public std::runtime_error {

    public:

        using std::runtime_error::runtime_error;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_duration
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr std::uint_least64_t maximum_audio_duration_n =
        300 * static_cast<std::uint_least64_t>(std::nano::den);

    constexpr
    std::uint_least64_t
    verify_audio_duration(const std::uint_least64_t n)
    {
        verify(
            n <= maximum_audio_duration_n, "{0}: invalid audio duration", n);
        return n;
    }

    /**
     * Represents a "valid" audio duration in nanoseconds (within the context
     * of `synthclone`).
     */

    export
    class audio_duration final: public uint_least64_proxy<audio_duration> {

    public:

        /**
         * Constucts an `audio_duration` instance.
         *
         * @param n
         *   The duration (in nanoseconds).
         */

        constexpr
        audio_duration(const std::uint_least64_t n):
            uint_least64_proxy<audio_duration>(verify_audio_duration(n))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_sample
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

#ifdef __STDCPP_FLOAT32_T__

    /**
     * The audio sample type used in `synthclone`.
     */

    export
    using audio_sample = std::float32_t;

#else

    static_assert(
        sizeof(float) == 4, "sizeof(float) is not 4 bytes on this platform");

    /**
     * The audio sample type used in `synthclone`.
     */

    export
    using audio_sample = float;

#endif

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_channel_count
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    // 1024 is the maximum audio channel count supported by `libsndfile`.  128
    // is the maximum audio channel count supported by `libsamplerate`.
    constexpr std::uint_least16_t maximum_audio_channel_count_n = 128;

    constexpr
    std::uint_least16_t
    verify_audio_channel_count(const std::uint_least16_t n)
    {
        verify(
            (n != 0) && (n <= maximum_audio_channel_count_n),
            "{0}: invalid audio channel count", n);
        return n;
    }

    /**
     * Holds an audio channel count.
     */

    export
    class audio_channel_count final:
        public uint_least16_proxy<audio_channel_count> {

    public:

        /**
         * Constructs an `audio_channel_count` instance.
         *
         * @param n
         *   The audio channel count.
         */

        constexpr
        audio_channel_count(const std::uint_least16_t n):
            uint_least16_proxy<audio_channel_count>(
                verify_audio_channel_count(n))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_sample_rate
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    // The resampler supports ratios between 1/256 and 256.  For now, we make
    // sure to only support a sample rate range that would generate ratios
    // within the supported resampler range.
    //
    // We can always /decrease/increase these later.
    constexpr std::uint_least32_t maximum_audio_sample_rate_n = 1024000;
    constexpr std::uint_least32_t minimum_audio_sample_rate_n = 4000;

    constexpr
    std::uint_least32_t
    verify_audio_sample_rate(const std::uint_least32_t n)
    {
        verify(
            (n >= minimum_audio_sample_rate_n) &&
            (n <= maximum_audio_sample_rate_n),
            "{0}: unsupported audio sample rate", n);
        return n;
    }

    /**
     * Holds an audio sample rate.
     */

    export
    class audio_sample_rate final:
        public uint_least32_proxy<audio_sample_rate> {

    public:

        /**
         * Constructs an `audio_sample_rate` instance.
         *
         * @param n
         *   The audio sample rate.
         */

        constexpr
        audio_sample_rate(const std::uint_least32_t n):
            uint_least32_proxy<audio_sample_rate>(verify_audio_sample_rate(n))
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_traits
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    constexpr
    audio_codec
    verify_audio_codec(const audio_codec codec)
    {
        const auto n = std::to_underlying(codec);
        verify(n < audio_codec_count, "{0}: invalid audio codec constant", n);
        return codec;
    }

    constexpr
    audio_endianness
    verify_audio_endianness(const audio_endianness endianness)
    {
        const auto n = std::to_underlying(endianness);
        verify(
            n < audio_endianness_count,
            "{0}: invalid audio endianness constant", n);
        return endianness;
    }

    constexpr
    audio_format
    verify_audio_format(const audio_format format)
    {
        const auto n = std::to_underlying(format);
        verify(
            n < audio_format_count, "{0}: invalid audio format constant", n);
        return format;
    }

    /**
     * Contains audio characteristics for `audio_input_stream` and
     * `audio_output_stream` instances.
     */

    export
    class audio_traits final {

    public:

        /**
         * Constructs an `audio_traits` instance using the given arguments.
         *
         * @param format
         *   The audio format.
         * @param codec
         *   The audio codec.
         * @param endianness
         *   The audio endianness.
         * @param sample_rate
         *   The audio sample rate.
         * @param channel_count
         *   The audio channel count.
         */

        constexpr
        audio_traits(
            const audio_format format,
            const audio_codec codec,
            const audio_endianness endianness,
            const audio_sample_rate sample_rate,
            const audio_channel_count channel_count
        ):
            sample_rate_(sample_rate),
            channel_count_(channel_count),
            format_(verify_audio_format(format)),
            codec_(verify_audio_codec(codec)),
            endianness_(verify_audio_endianness(endianness))
        {
            // empty
        }

        /**
         * Constructs an `audio_traits` instance using the given arguments.
         *
         * @param format
         *   The audio format.
         * @param codec
         *   The audio codec.
         * @param sample_rate
         *   The audio sample rate.
         * @param channel_count
         *   The audio channel count.
         */

        constexpr
        audio_traits(
            const audio_format format,
            const audio_codec codec,
            const audio_sample_rate sample_rate,
            const audio_channel_count channel_count
        ):
            audio_traits(
                format, codec, audio_endianness::file, sample_rate,
                channel_count)
        {
            // empty
        }


        /**
         * Gets the channel count.
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
         * Gets the codec.
         *
         * @return
         *   The codec.
         */

        constexpr
        audio_codec
        codec() const noexcept
        {
            return codec_;
        }

        /**
         * Gets the audio endianness.
         *
         * @return
         *   The endianness.
         */

        constexpr
        audio_endianness
        endianness() const noexcept
        {
            return endianness_;
        }

        /**
         * Gets the format.
         *
         * @return
         *   The format.
         */

        constexpr
        audio_format
        format() const noexcept
        {
            return format_;
        }

        /**
         * Gets the sample rate.
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

    private:

        audio_traits() = delete;

        audio_sample_rate sample_rate_;
        audio_channel_count channel_count_;
        audio_format format_;
        audio_codec codec_;
        audio_endianness endianness_;

    };

    /**
     * Gets a boolean indicating whether or not the two `audio_traits`
     * instances compare equal.
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
    operator==(const audio_traits& lhs, const audio_traits& rhs) noexcept
    {
        return (lhs.format() == rhs.format()) &&
            (lhs.codec() == rhs.codec()) &&
            (lhs.endianness() == rhs.endianness()) &&
            (lhs.sample_rate() == rhs.sample_rate()) &&
            (lhs.channel_count() == rhs.channel_count());
    }

    /**
     * Gets a boolean indicating whether or not the two `audio_traits`
     * instances compare inequal.
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
    operator!=(const audio_traits& lhs, const audio_traits& rhs) noexcept
    {
        return ! (lhs == rhs);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_seek_offset
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * The integral type used to represent audio stream seek offsets.
     */

    export
    using audio_seek_offset = std::streamoff;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_frame_count
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * The audio frame count type used in `synthclone`.
     */

    export
    using audio_frame_count = std::make_unsigned_t<audio_seek_offset>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_resampler
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    // Both `libsamplerate` and `synthclone` use `float` for samples.  If that
    // changes, this implementation will need to change too.
    static_assert(std::is_same_v<audio_sample, float>);

    struct src_state_destroyer {

        void
        operator()(::SRC_STATE* handle) noexcept
        {
            assume(handle != nullptr, "handle is set to NULL");
            ::src_delete(handle);
        }

    };

    using src_state_ptr = std::unique_ptr<::SRC_STATE, src_state_destroyer>;

    constexpr const std::array<float, 1> junk_buffer { 0.0 };

    std::string_view
    extract_src_error_message(const int code)
    {
        assume(code != 0, "code is set to 0");

        const auto* message = ::src_strerror(code);
        assume(message != nullptr, "message is set to NULL");

        return std::string_view(message);
    }

    src_state_ptr
    make_src_state_handle(const audio_channel_count channel_count)
    {
        int error_code;
        src_state_ptr handle(
            ::src_new(
                ::SRC_SINC_BEST_QUALITY,
                static_cast<long>(channel_count.value()), &error_code));
        if (handle == nullptr) [[unlikely]] {
            throw audio_error(
                std::format(
                    "::src_new(::SRC_SINC_BEST_QUALITY, {0}, {1:x}): "
                    "failed to create resampler state: {2}",
                    channel_count,
                    reinterpret_cast<std::uintptr_t>(&error_code),
                    extract_src_error_message(error_code)));
        }
        return handle;
    }

    double
    make_src_ratio(
        const audio_sample_rate input_sample_rate,
        const audio_sample_rate output_sample_rate
    )
    {
        const auto ratio = static_cast<double>(output_sample_rate.value()) /
            static_cast<double>(input_sample_rate.value());

        assume(
            ::src_is_valid_ratio(ratio),
            "the sample rate conversion ratio ({0}), used to convert from {1} "
            "Hz to {2} Hz, is not valid",
            ratio, input_sample_rate, output_sample_rate);

        return ratio;
    }

    void
    resample(src_state_ptr& handle, ::SRC_DATA& data)
    {
        assume(handle != nullptr, "handle is set to NULL");
        assume(
            (
                (data.data_in == junk_buffer.data()) ==
                (data.input_frames == 0)
            ) &&
            (data.data_out != nullptr) && (data.input_frames >= 0) &&
            (data.output_frames > 0) && (data.src_ratio > 0.0),
            "{{ data_in={0:x}, data_out={1:x}, input_frames={2}, "
            "output_frames={3}, end_of_input={4}, src_ratio={5} }}: one or "
            "`::SRC_DATA` preconditions not satisfied",
            reinterpret_cast<std::uintptr_t>(data.data_in),
            reinterpret_cast<std::uintptr_t>(data.data_out),
            data.input_frames, data.output_frames, data.end_of_input,
            data.src_ratio);

        const auto error_code = ::src_process(handle.get(), &data);
        if (error_code != 0) [[unlikely]] {
            throw audio_error(
                std::format(
                    "::src_process({0:x}, {{ data_in={1:x}, data_out={2:x}, "
                    "input_frames={3}, output_frames={4}, "
                    "end_of_input={5}, src_ratio={6} }}): failed to "
                    "process data: {7}",
                    reinterpret_cast<std::uintptr_t>(handle.get()),
                    reinterpret_cast<std::uintptr_t>(data.data_in),
                    reinterpret_cast<std::uintptr_t>(data.data_out),
                    data.input_frames, data.output_frames, data.end_of_input,
                    data.src_ratio, extract_src_error_message(error_code)));
        }

        assume(
            (data.input_frames_used >= 0) &&
            (data.input_frames_used <= data.input_frames) &&
            (data.output_frames_gen >= 0) &&
            (data.output_frames_gen <= data.output_frames) &&
            (
                (data.input_frames == 0) ||
                (
                    ! (
                        (data.input_frames_used == 0) &&
                        (data.output_frames_gen == 0)
                    )
                )
            ),
            "{{ data_in={0:x}, data_out={1:x}, input_frames={2}, "
            "output_frames={3}, input_frames_used={4}, output_frames_gen={5}, "
            "end_of_input={6}, src_ratio={7} }}: one or more `::SRC_DATA` "
            "postconditions not satisfied",
            reinterpret_cast<std::uintptr_t>(data.data_in),
            reinterpret_cast<std::uintptr_t>(data.data_out),
            data.input_frames, data.output_frames, data.input_frames_used,
            data.output_frames_gen, data.end_of_input, data.src_ratio);
    }

    /**
     * Resamples audio data.
     */

    export
    class audio_resampler final: private noncopyable {

    public:

        /**
         * Constructs an `audio_resampler` instance.
         *
         * @param input_sample_rate
         *   The sample rate of the data that will be fed to the
         *   `audio_resampler` instance.
         * @param output_sample_rate
         *   The sample rate of the data that will be emitted by the
         *   `audio_resampler` instance.
         * @param channel_count
         *   The number of audio channels in the audio data.
         */

        audio_resampler(
            const audio_sample_rate input_sample_rate,
            const audio_sample_rate output_sample_rate,
            const audio_channel_count channel_count
        ):
            handle_(make_src_state_handle(channel_count)),
            ratio_(make_src_ratio(input_sample_rate, output_sample_rate)),
            channel_count_(channel_count),
            flush_started_(false)
        {
            // empty
        }

        /**
         * Flushes remaining data from the internal buffers of the
         * `audio_resampler` instance.
         *
         * When flushing data, you shouldn't assume all of the data is flushed
         * until this function returns 0.
         *
         * @param output_buffer
         *   The buffer to write to.
         *
         * @return
         *   The number of frames written to the given output buffer (zero if
         *   all of data in the internal buffers has been flushed).
         */

        audio_frame_count
        flush(std::span<audio_sample> output_buffer)
        {
            verify(handle_ != nullptr, "handle is set to NULL");

            const auto channel_count = channel_count_.value();
            auto* output_data = output_buffer.data();
            const auto output_size = output_buffer.size();

            verify(output_data != nullptr, "output buffer is set to NULL");
            verify(output_size != 0, "output buffer size is set to 0");
            verify(
                (output_size % channel_count) == 0,
                "output buffer size {0} is not evenly divisible by channel "
                "count {1}",
                output_size, channel_count);

            // If we set `.data_in` to `nullptr`, then `libsamplerate` doesn't
            // flush all of the data.
            //
            // XXX: Open an issue with `libsamplerate`.
            ::SRC_DATA data {
                .data_in = junk_buffer.data() /*nullptr*/,
                .data_out = output_data,
                .input_frames = 0,
                .output_frames = static_cast<long>(
                    output_size / channel_count),
                .end_of_input = 1,
                .src_ratio = ratio_
            };

            flush_started_ = true;

            synthclone::resample(handle_, data);

            return static_cast<audio_frame_count>(data.output_frames_gen);
        }

        /**
         * Reads data from the given input buffer and writes it to internal
         * buffers and writes any available resampled data to the given output
         * buffer.
         *
         * Note that you cannot call this method on an `audio_resampler`
         * instance after calling `flush()` for the same instance because, once
         * `flush()` is called, the `audio_resampler` instance assumes that the
         * end of input has been reached.  This limitation is lifted if you
         * call `reset()` on the `audio_resampler` instance.
         *
         * @param input_buffer
         *   Data to read into the `audio_resampler` instance's intneral
         *   buffers.
         * @param output_buffer
         *   Buffer to write resampled data to.
         *
         * @return
         *   A pair of `audio_frame_count` elements.  The first element
         *   is the total number of frames read from the input buffer, and the
         *   second element is the total number of frames written to the output
         *   buffer.
         */

        std::pair<audio_frame_count, audio_frame_count>
        resample(
            std::span<const audio_sample> input_buffer,
            std::span<audio_sample> output_buffer
        )
        {
            verify(handle_ != nullptr, "handle is set to NULL");
            verify(
                ! flush_started_,
                "cannot resample more data after starting resampler flush");

            const auto channel_count = channel_count_.value();
            const auto input_data = input_buffer.data();
            const auto input_size = input_buffer.size();
            auto* output_data = output_buffer.data();
            const auto output_size = output_buffer.size();

            verify(input_data != nullptr, "input buffer is set to NULL");
            verify(input_size != 0, "input buffer size is set to 0");
            verify(
                (input_size % channel_count) == 0,
                "input buffer size {0} is not evenly divisible by channel "
                "count {1}",
                input_size, channel_count);

            verify(output_data != nullptr, "output buffer is set to NULL");
            verify(output_size != 0, "output buffer size is set to 0");
            verify(
                (output_size % channel_count) == 0,
                "output buffer size {0} is not evenly divisible by channel "
                "count {1}",
                output_size, channel_count);

            const auto input_frame_count = input_size / channel_count;
            ::SRC_DATA data {
                .data_in = input_data,
                .data_out = output_data,
                .input_frames = static_cast<long>(input_frame_count),
                .output_frames = static_cast<long>(
                    output_size / channel_count),
                .end_of_input = 0,
                .src_ratio = ratio_
            };

            synthclone::resample(handle_, data);

            return {
                static_cast<audio_frame_count>(data.input_frames_used),
                static_cast<audio_frame_count>(data.output_frames_gen)
            };
        }

        /**
         * Resets the internal state of the `audio_resampler` instance.
         */

        void
        reset()
        {
            verify(handle_ != nullptr, "handle is set to NULL");

            const auto result = ::src_reset(handle_.get());
            if (result != 0) [[unlikely]] {
                throw audio_error(
                    std::format(
                        "::src_reset({0:x}): failed to reset resampler: {1}",
                        reinterpret_cast<std::uintptr_t>(handle_.get()),
                        extract_src_error_message(result)));
            }

            flush_started_ = false;
        }

    private:

        src_state_ptr handle_;
        double ratio_;
        audio_channel_count channel_count_;
        bool flush_started_;

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::get_extension()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Gets the default extension corresponding to a given format.
     *
     * @param format
     *   The audio format.
     *
     * @return
     *   The default extension.
     */

    export
    constexpr
    std::string_view
    get_extension(const audio_format format)
    {
        return audio_format_extension_table.at(
            static_cast<std::size_t>(format));
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::to_dbfs()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    /**
     * Gets the relative dBFS value for a floating point amplitude value.
     *
     * @param n
     *   The amplitude value.
     *
     * @return
     *   The relative volume in dBFS.
     */

    export
    template<std::floating_point T>
    T
    to_dbfs(const T n)
    {
        return static_cast<T>(20.0) * std::log10(std::abs(n));
    }

}
