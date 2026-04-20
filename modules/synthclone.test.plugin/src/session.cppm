export module synthclone.test.plugin:session;

import synthclone.core;

namespace synthclone {

    export
    constexpr
    session_info
    make_test_session_info(
        audio_sample_rate sample_rate,
        audio_channel_count channels
    ) noexcept
    {
        return session_info(
            audio_traits(
                audio_format::raw, audio_codec::pcm_f32,
                audio_endianness::little, sample_rate, channels));
    }

}
