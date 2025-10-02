module;

#include <boost/test/unit_test.hpp>

export module synthclone.core.test:audio;

import std;

import synthclone.core;
import synthclone.test;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::audio_sample_buffer
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<std::size_t N>
    using audio_sample_buffer = fixed_buffer<audio_sample, N>;

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::load_audio()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    template<
        audio_format Format,
        audio_codec Codec,
        audio_endianness Endianness,
        std::size_t SampleRate,
        std::size_t ChannelCount,
        std::size_t FrameCount,
        std::size_t FrameOffset = 0,
        bool ExpectEos = true
    >
    audio_sample_buffer<FrameCount * ChannelCount>
    load_audio(const std::filesystem::path& path)
    {
        BOOST_TEST_INFO_SCOPE(make_test_info("synthclone::load_audio", path));

        constexpr audio_traits traits(
            Format, Codec, Endianness, SampleRate, ChannelCount);

        BOOST_REQUIRE(is_streamable(traits));

        audio_input_stream stream(path, traits);

        if constexpr(FrameOffset != 0) {
            audio_frame_count position;
            if (stream.seekable()) {
                position = stream.seek(
                    FrameOffset, audio_seek_origin::current);
            } else {
                std::array<audio_sample, FrameOffset * ChannelCount> tmp;
                position = stream.read(tmp);
            }

            BOOST_REQUIRE_MESSAGE(
                position == FrameOffset,
                std::format(
                    "expected to skip {0} frames, but found EOS at offset {1}",
                    FrameOffset, position));

        }

        audio_sample_buffer<FrameCount * ChannelCount> buffer;
        const auto frames_read = stream.read(buffer);

        BOOST_REQUIRE_MESSAGE(
            frames_read == FrameCount,
            std::format(
                "expected to read {0} frames, but only read {1} frames",
                FrameCount, frames_read));

        if constexpr(ExpectEos) {
            constexpr auto expected_end_position = FrameOffset + FrameCount;

            if (stream.seekable()) {
                const auto end_position = stream.seek(
                    0, audio_seek_origin::end);

                BOOST_REQUIRE_MESSAGE(
                    end_position == expected_end_position,
                    std::format(
                        "expected EOS at frame {0}, but EOS is at frame {1}",
                        expected_end_position, end_position));

            } else {
                std::array<audio_sample, ChannelCount> tmp;
                const auto n = stream.read(tmp);

                BOOST_REQUIRE_MESSAGE(
                    n == 0,
                    std::format(
                        "expected EOS at frame {0}, but there are more frames",
                        expected_end_position));

            }
        }

        return buffer;
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_audio_traits()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    void
    verify_audio_traits(
        const audio_traits& traits,
        audio_format expected_format,
        audio_codec expected_codec,
        audio_endianness expected_endianness,
        audio_sample_rate expected_sample_rate,
        audio_channel_count expected_channel_count
    )
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info(
                "synthclone::verify_audio_traits", traits,
                get_identifier(expected_format),
                get_identifier(expected_codec),
                get_identifier(expected_endianness), expected_sample_rate,
                expected_channel_count));

        verify_eq(traits.format(), expected_format);
        verify_eq(traits.codec(), expected_codec);
        verify_eq(traits.endianness(), expected_endianness);
        verify_eq(traits.sample_rate(), expected_sample_rate);
        verify_eq(traits.channel_count(), expected_channel_count);
    }

    export
    void
    verify_audio_traits(const audio_traits& lhs, const audio_traits& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_audio_traits", lhs, rhs));

        verify_audio_traits(
            lhs, rhs.format(), rhs.codec(), rhs.endianness(),
            rhs.sample_rate(), rhs.channel_count());

        verify_eq(lhs, rhs);
    }

}

///////////////////////////////////////////////////////////////////////////////
// synthclone::verify_audio_input_stream()
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    export
    void
    verify_audio_input_stream(audio_input_stream& lhs, audio_input_stream& rhs)
    {
        BOOST_TEST_INFO_SCOPE(
            make_test_info("synthclone::verify_audio_input_stream", lhs, rhs));

        verify_audio_traits(lhs.traits(), rhs.traits());

        auto closed = lhs.closed();
        verify_eq(closed, rhs.closed());

        if (closed) {
            return;
        }

        auto seekable = lhs.seekable();
        verify_eq(seekable, rhs.seekable());

        if (seekable) {
            verify_eq(lhs.tell(), rhs.tell());
        }

        std::array<audio_sample, 4096> lhs_buffer;
        std::array<audio_sample, 4096> rhs_buffer;

        for (;;) {

            const auto lhs_read_count = lhs.read(lhs_buffer);
            const auto rhs_read_count = rhs.read(rhs_buffer);
            verify_eq(lhs_read_count, rhs_read_count);

            if (! lhs_read_count) {
                break;
            }

            std::span lhs_span(lhs_buffer.data(), lhs_read_count);
            std::span rhs_span(rhs_buffer.data(), rhs_read_count);

            BOOST_CHECK_EQUAL_COLLECTIONS(
                lhs_span.begin(), lhs_span.end(), rhs_span.begin(),
                rhs_span.end());
        }
    }

}
