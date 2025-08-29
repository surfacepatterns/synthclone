/**
 * @file
 *
 * Contains support for `libsndfile` audio I/O.
 */

module;

#include <cerrno>

#include <boost/scope/scope_exit.hpp>

#include <sndfile.h>

#include <synthclone/config.h>

export module synthclone.core:sndfile_io;

import std;

import synthclone.util;

import :audio_core;
import :sndfile_core;
import :sndfile_gen;

///////////////////////////////////////////////////////////////////////////////
// synthclone::sndfile_input_stream
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    audio_codec
    extract_audio_codec(int sndfile_format)
    {
        auto n = sndfile_format & ::SF_FORMAT_SUBMASK;
        auto result = sndfile_to_audio_codec_table[
            to_unsigned(n >> sndfile_codec_shift)];
        assume(result != -1, "{0}: unsupported sndfile codec constant", n);
        return static_cast<audio_codec>(result);
    }

    audio_endianness
    extract_audio_endianness(int sndfile_format)
    {
        auto n = sndfile_format & ::SF_FORMAT_ENDMASK;
        auto result = sndfile_to_audio_endianness_table[
            to_unsigned(n >> sndfile_endianness_shift)];
        assume(
            result != -1, "{0}: unsupported sndfile endianness constant", n);
        return static_cast<audio_endianness>(result);
    }

    audio_format
    extract_audio_format(int sndfile_format)
    {
        auto n = sndfile_format & ::SF_FORMAT_TYPEMASK;
        auto result = sndfile_to_audio_format_table[
            to_unsigned(n >> sndfile_format_shift)];
        assume(result != -1, "{0}: unsupported sndfile format constant", n);
        return static_cast<audio_format>(result);
    }

    int
    to_sndfile_codec(audio_codec codec)
    {
        auto result = audio_to_sndfile_codec_table[
            static_cast<std::size_t>(codec)];
        verify(
            result != -1,
            "{0}: no `sndfile` binding available", get_identifier(codec));
        return result;
    }

    int
    to_sndfile_endianness(audio_endianness endianness)
    {
        return audio_to_sndfile_endianness_table[
            static_cast<std::size_t>(endianness)];
    }

    int
    to_sndfile_seek_origin(audio_seek_origin origin)
    {
        auto result = audio_to_sndfile_seek_origin_table[
            static_cast<std::size_t>(origin)];
        verify(
            result != -1,
            "{0}: no `sndfile` binding available", get_identifier(origin));
        return result;
    }

    ::SF_INFO
    make_raw_sndfile_info(
        audio_codec codec,
        audio_endianness endianness,
        audio_sample_rate sample_rate,
        audio_channel_count channel_count
    )
    {
        return ::SF_INFO {
            .frames = 0,

            .samplerate = static_cast<int>(sample_rate),
            .channels = static_cast<int>(channel_count),
            .format = ::SF_FORMAT_RAW | to_sndfile_codec(codec) |
                to_sndfile_endianness(endianness),

            .sections = 0,
            .seekable = 0
        };
    }

    void
    close_sndfile_handle(::SNDFILE* handle)
    {

        assume(handle != nullptr, "handle is set to NULL");

        // Some notes:
        //
        // * A return value of 0 does not mean the call succeeded.
        //   `VALIDATE_SNDFILE_AND_ASSIGN_PSF`, a macro used in many API entry
        //   points for `libsndfile`, will return 0 on errors and store the
        //   error code in the handle's private data; unfortunately, we can't
        //   distinguish this from a successful close wherein the handle has
        //   been freed, so checking for this error isn't practical.
        // * The current implementation of `::sf_close()` ignores errors in the
        //   custom codec and container close implementations.
        // * When `sf_close()` returns a non-zero value, the value appears to
        //   come from calling `::close()` on the file descriptor.  Under most
        //   circumstances, it seems like `errno` is preserved, but it may not
        //   be preserved if `libsndfile` does resource forking because, in
        //   that case, it seems like `libsndfile` closes another file
        //   descriptor (and ignores the result), which could affect `errno`.
        //   We use `errno` anyway and hope that it provides a useful error
        //   code.
        errno = 0;
        if (::sf_close(handle) != 0) [[unlikely]] {
            throw audio_error(
                std::format(
                    "::sf_close({0:x}): failed to close audio stream: {1}",
                    reinterpret_cast<std::uintptr_t>(handle),
                    std::generic_category().message(errno)));
        }
    }

    struct sndfile_destroyer final {

        void
        operator()(::SNDFILE* handle)
        {
            close_sndfile_handle(handle);
        }

    };

    using sndfile_ptr = std::unique_ptr<::SNDFILE, sndfile_destroyer>;

    constexpr
    ::SF_INFO sndfile_infer_info { .format = 0 };

    const char*
    check_sndfile_error_message(sndfile_ptr& handle)
    {
        auto* p = handle.get();
        return ::sf_error(p) == ::SF_ERR_NO_ERROR ? nullptr : ::sf_strerror(p);
    }

    const char*
    expect_sndfile_error_message(sndfile_ptr& handle)
    {
        auto* p = handle.get();
        assume(
            ::sf_error(p) != ::SF_ERR_NO_ERROR,
            "attempted to extract `libsndfile` error message when error isn't "
            "set");
        return ::sf_strerror(p);
    }

    sndfile_ptr
    open_sndfile_stream(
        const std::filesystem::path& path,
        ::SF_INFO& info,
        int mode
    )
    {
        errno = 0;
        sndfile_ptr handle(::sf_open(path.c_str(), mode, &info));
        if (handle == nullptr) [[unlikely]] {
            // XXX: We use std::filesystem::path::string() to format the path.
            // When C++26 comes around, we should use the path directly.
            throw audio_error(
                std::format(
                    "::sf_open({0:?}, {1}, {{ .samplerate={2}, .channels={3}, "
                    ".format={4} }}): failed to open audio stream: {5}",
                    path.string(), mode, info.samplerate, info.channels,
                    info.format, expect_sndfile_error_message(handle)));
        }
        return handle;
    }

    class sndfile_stream: private noncopyable {

    public:

        inline
        void
        close()
        {
            boost::scope::scope_exit release_guard(
                [this]() { handle_.release(); });
            close_sndfile_handle(handle_.get());
        }

        inline
        bool
        closed() const noexcept
        {
            return handle_ == nullptr;
        }

        audio_frame_count
        seek(audio_seek_offset frames, audio_seek_origin origin)
        {
            errno = 0;
            auto result = ::sf_seek(
                handle_.get(), static_cast<::sf_count_t>(frames),
                to_sndfile_seek_origin(origin));

            const char* message;
            switch (result) {

            case 0:
                // 0 *can* mean an error.
                {
                    message = check_sndfile_error_message(handle_);
                    if (message == nullptr) [[likely]] {
                        return 0;
                    }
                }
                break;

            [[unlikely]] case -1:
                message = expect_sndfile_error_message(handle_);
                break;

            default:

                assume(
                    result > 0,
                    "::sf_seek({0:x}, {1}, {2}): unexpected seek result: {3}",
                    reinterpret_cast<std::uintptr_t>(handle_.get()), frames,
                    get_identifier(origin), result);

                return result;
            }

            throw audio_error(
                std::format(
                    "::sf_seek({0:x}, {1}, {2}): seek operation failed: {3}",
                    reinterpret_cast<std::uintptr_t>(handle_.get()), frames,
                    get_identifier(origin), message));
        }

        inline
        bool
        seekable() const noexcept
        {
            return seekable_;
        }

        inline
        audio_frame_count
        tell()
        {
            return seek(0, audio_seek_origin::current);
        }

        inline
        const audio_traits&
        traits() const noexcept
        {
            return traits_;
        }

    protected:

        sndfile_stream(sndfile_stream&& stream) noexcept:
            traits_(stream.traits_),
            handle_(std::move(stream.handle_)),
            seekable_(stream.seekable_)
        {
            // empty
        }

        sndfile_stream(
            sndfile_ptr&& handle,
            const audio_traits& traits,
            bool seekable
        ) noexcept:
            traits_(traits),
            handle_(std::move(handle)),
            seekable_(seekable)
        {
            // empty
        }

        ~sndfile_stream() = default;

        sndfile_stream&
        operator=(sndfile_stream&& stream) noexcept
        {
            traits_ = stream.traits_;
            handle_ = std::move(stream.handle_);
            seekable_ = stream.seekable_;
            return *this;
        }

        audio_traits traits_;
        sndfile_ptr handle_;
        bool seekable_;

    };

    audio_traits
    make_sndfile_audio_traits(const ::SF_INFO& info)
    {
        auto sndfile_format = info.format;
        return audio_traits(
            extract_audio_format(sndfile_format),
            extract_audio_codec(sndfile_format),
            extract_audio_endianness(sndfile_format), info.samplerate,
            info.channels);
    }

    audio_traits
    make_sndfile_audio_traits(
        const ::SF_INFO& info,
        const audio_traits& expected_traits
    )
    {
        auto actual_traits = make_sndfile_audio_traits(info);

        auto expected_format = expected_traits.format();
        auto actual_format = actual_traits.format();
        if (expected_format != actual_format) [[unlikely]] {
            throw audio_error(
                std::format(
                    "audio format {0:?} does not match expected audio format "
                    "{1:?}",
                    get_identifier(actual_format),
                    get_identifier(expected_format)));
        }

        auto expected_codec = expected_traits.codec();
        auto actual_codec = actual_traits.codec();
        if (expected_codec != actual_codec) [[unlikely]] {
            throw audio_error(
                std::format(
                    "audio codec {0:?} does not match expected audio codec "
                    "{1:?}",
                    get_identifier(actual_codec),
                    get_identifier(expected_codec)));
        }

        auto expected_endianness = expected_traits.endianness();
        if (expected_endianness != audio_endianness::file) {
            auto actual_endianness = actual_traits.endianness();
            if (
                (actual_endianness != audio_endianness::file) &&
                (expected_endianness != actual_endianness)
            ) [[unlikely]] {
                throw audio_error(
                    std::format(
                        "audio endianness {0:?} does not match expected audio "
                        "endianness {1:?}",
                        get_identifier(actual_endianness),
                        get_identifier(expected_endianness)));
            }
        }

        auto expected_sample_rate = expected_traits.sample_rate();
        auto actual_sample_rate = actual_traits.sample_rate();
        if (expected_sample_rate != actual_sample_rate.value()) [[unlikely]] {
            throw audio_error(
                std::format(
                    "sample rate {0} does not match expected sample rate {1}",
                    actual_sample_rate, expected_sample_rate));
        }

        auto expected_channel_count = expected_traits.channel_count();
        auto actual_channel_count = actual_traits.channel_count();
        if (expected_channel_count != actual_channel_count) [[unlikely]] {
            throw audio_error(
                std::format(
                    "channel count {0} does not match expected channel count "
                    "{1}",
                    actual_channel_count, expected_channel_count));
        }

        return actual_traits;
    }

    export
    class sndfile_input_stream final: public sndfile_stream {

    public:

        inline explicit
        sndfile_input_stream(const std::filesystem::path& path):
            sndfile_input_stream(path, sndfile_infer_info)
        {
            // empty
        }

        inline
        sndfile_input_stream(
            const std::filesystem::path& path,
            const audio_traits& traits
        ):
            sndfile_input_stream(
                path,
                traits.format() == audio_format::raw ?
                    make_raw_sndfile_info(
                        traits.codec(), traits.endianness(),
                        traits.sample_rate(), traits.channel_count()) :
                    sndfile_infer_info,
                traits)
        {
            // empty
        }

        audio_frame_count
        read(std::span<audio_sample> buffer)
        {
            auto size = buffer.size();
            errno = 0;
            auto result = ::sf_read_float(handle_.get(), buffer.data(), size);

            assume(
                (result >= 0) && (result <= size) &&
                ((result % traits().channel_count().value()) == 0),
                "::sf_read_float({0:x}, {1:x}, {2}): unexpected result: {3}",
                reinterpret_cast<std::uintptr_t>(handle_.get()),
                reinterpret_cast<std::uintptr_t>(buffer.data()),
                size, result);

            // A partial read *probably* means EOF, but it *might* mean an
            // error occurred.
            if (result != size) [[unlikely]] {
                const auto* message = check_sndfile_error_message(handle_);
                if (message != nullptr) [[unlikely]] {
                    throw audio_error(
                        std::format(
                            "::sf_read_float({0:x}, {1:x}, {2}): failed to "
                            "read all samples into buffer from file ({3} "
                            "samples were read): {4}",
                            reinterpret_cast<std::uintptr_t>(handle_.get()),
                            reinterpret_cast<std::uintptr_t>(buffer.data()),
                            size, result, message));
                }
            }

            return static_cast<audio_frame_count>(
                result / traits().channel_count().value());
        }

    private:

        sndfile_input_stream(sndfile_ptr&& handle, ::SF_INFO& info):
            sndfile_stream(
                std::move(handle), make_sndfile_audio_traits(info),
                info.seekable)
        {
            // empty
        }

        sndfile_input_stream(
            sndfile_ptr&& handle,
            ::SF_INFO& info,
            const audio_traits& traits
        ):
            sndfile_stream(
                std::move(handle), make_sndfile_audio_traits(info, traits),
                info.seekable)
        {
            // empty
        }

        sndfile_input_stream(
            const std::filesystem::path& path,
            ::SF_INFO info
        ):
            sndfile_input_stream(
                open_sndfile_stream(path, info, ::SFM_READ), info)
        {
            // empty
        }

        sndfile_input_stream(
            const std::filesystem::path& path,
            ::SF_INFO info,
            const audio_traits& traits
        ):
            sndfile_input_stream(
                open_sndfile_stream(path, info, ::SFM_READ), info, traits)
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::sndfile_output_stream
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    int
    to_sndfile_format(audio_format format)
    {
        auto result = audio_to_sndfile_format_table.at(
            static_cast<std::size_t>(format));
        verify(
            result != -1,
            "{0}: no `sndfile` binding available", get_identifier(format));
        return result;
    }

    ::SF_INFO
    make_sndfile_info(const audio_traits& traits)
    {
        return ::SF_INFO {
            .frames = 0,

            .samplerate = static_cast<int>(traits.sample_rate()),
            .channels = static_cast<int>(traits.channel_count()),
            .format = to_sndfile_format(traits.format()) |
                to_sndfile_codec(traits.codec()) |
                to_sndfile_endianness(traits.endianness()),

            .sections = 0,
            .seekable = 0
        };
    }

    constexpr
    char sndfile_error_unknown[] = "reason unknown";

    export
    class sndfile_output_stream final: public sndfile_stream {

    public:

        inline
        sndfile_output_stream(
            std::filesystem::path path,
            const audio_traits& traits
        ):
            sndfile_output_stream(path, make_sndfile_info(traits))
        {
            // empty
        }

        void
        write(std::span<const audio_sample> buffer)
        {
            auto size = buffer.size();
            auto result = ::sf_write_float(handle_.get(), buffer.data(), size);

            assume(
                (result >= 0) && (result <= size),
                "::sf_write_float({0:x}, {1:x}, {2}): unexpected result: {3}",
                reinterpret_cast<std::uintptr_t>(handle_.get()),
                reinterpret_cast<std::uintptr_t>(buffer.data()),
                buffer.size(), result);

            // A partial write generally means an error; however, there are
            // circumstances in which a partial write succeeds, but `errno` is
            // not set.  For more information, see the documentation for the
            // `write()` system call:
            //
            //     https://man7.org/linux/man-pages/man2/write.2.html
            if (result != size) [[unlikely]] {
                const auto* message = check_sndfile_error_message(handle_);
                if (message == nullptr) {
                    message = sndfile_error_unknown;
                }
                throw audio_error(
                    std::format(
                        "::sf_write_float({0:x}, {1:x}, {2}): failed to write "
                        "all samples in buffer to file ({3} samples were "
                        "written): {4}",
                        reinterpret_cast<std::uintptr_t>(handle_.get()),
                        reinterpret_cast<std::uintptr_t>(buffer.data()),
                        buffer.size(), result, message));
            }
        }

    private:

        sndfile_output_stream(sndfile_ptr&& handle, ::SF_INFO& info):
            sndfile_stream(
                std::move(handle), make_sndfile_audio_traits(info),
                info.seekable)
        {
            // empty
        }

        sndfile_output_stream(
            const std::filesystem::path& path,
            ::SF_INFO info
        ):
            sndfile_output_stream(
                open_sndfile_stream(path, info, ::SFM_WRITE), info)
        {
            // empty
        }

    };

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::is_sndfile_streamable()
///////////////////////////////////////////////////////////////////////////////

namespace SYNTHCLONE_LIB_NAMESPACE {

    export
    inline
    bool
    is_sndfile_streamable(const audio_traits& traits)
    {
        // XXX: This works for now, but won't work if and when `synthclone`
        // supports other audio libraries.
        const auto info = make_sndfile_info(traits);
        return ::sf_format_check(&info);
    }

}
