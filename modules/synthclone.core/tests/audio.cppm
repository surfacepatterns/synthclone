module;

#include <boost/test/unit_test.hpp>

export module synthclone.core.tests:audio;

import std;

import synthclone.core;
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
        BOOST_TEST_INFO_SCOPE(
            std::format(
                "synthclone::load_audio<"
                "{0}, {1}, {2}, {3}, {4}, {5}, {6}, {7}"
                ">({8:?})",
                get_identifier(Format), get_identifier(Codec),
                get_identifier(Endianness), SampleRate, ChannelCount,
                FrameCount, FrameOffset, ExpectEos, path.string()));

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
