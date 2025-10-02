from collections.abc import Callable as _Callable, Iterable as _Iterable
from enum import Enum as _Enum
from itertools import chain as _chain
from math import log10 as _log10
from pathlib import Path as _Path
from typing import assert_never as _assert_never

from ..._enum import (
    EnumElementTraits as _EnumElementTraits,
    get_enum_element_identifier as _get_enum_element_identifier
)

from ._layout import REFERENCE_AUDIO_PATH as _REFERENCE_AUDIO_PATH

###############################################################################
# AudioEndiannessTraits
###############################################################################

class AudioEndiannessTraits(_EnumElementTraits):

    __slots__ = ()

###############################################################################
# AudioEndianness
###############################################################################

class AudioEndianness(_Enum):

    FILE = AudioEndiannessTraits("Default file-endianness.")

    BIG = AudioEndiannessTraits("Big endian.")

    LITTLE = AudioEndiannessTraits("Little endian.")

###############################################################################
# AudioVerificationTraits
###############################################################################

def _verify_audio_verification_read_offset(n: int) -> int:
    if n < 0:
        raise ValueError(f"{n}: invalid audio verification read offset")
    return n

def _verify_audio_verification_power_threshold(n: float) -> float:
    if not (0.0 <= n <= 1.0):
        raise ValueError(f"{n}: invalid audio verification threshold")
    return n

# XXX: Add cohesion.

class AudioVerificationTraits:

    __slots__ = ("__peak_power_threshold", "__read_offset", "__truncate")

    __peak_power_threshold: float
    __read_offset: int
    __truncate: bool

    def __init__(
        self,
        peak_power_threshold: float,
        /,
        *,
        read_offset: int = 0,
        truncate: bool = False
    ):
        self.__peak_power_threshold = \
            _verify_audio_verification_power_threshold(peak_power_threshold)
        self.__read_offset = _verify_audio_verification_read_offset(
            read_offset)
        self.__truncate = truncate

    @property
    def peak_power_threshold(self, /) -> float:
        return self.__peak_power_threshold

    @property
    def read_offset(self, /) -> int:
        return self.__read_offset

    @property
    def truncate(self, /) -> bool:
        return self.__truncate

###############################################################################
# AudioVerificationTraitsFactory
###############################################################################

type AudioVerificationTraitsFactory = _Callable[[int], AudioVerificationTraits]

###############################################################################
# AudioCodecFilterCallable
###############################################################################

type AudioCodecFilterCallable = _Callable[[AudioEndianness, int, int], bool]

###############################################################################
# AudioCodecTraits
###############################################################################

_FFT_ERROR = (1 / (2 ** 24))

def _make_simple_audio_verification_traits(
    significant_bits: int
) -> AudioVerificationTraits:
    # The amount of significant bits should include the number of bits of
    # precision and the sign bit.
    if significant_bits <= 1:
        raise ValueError(f"{significant_bits}: invalid significant bits value")

    # The minimum number of significant bits is capped at 24 for verification
    # purposes because the audio sample type used in `synthclone` is a 32-bit
    # floating point number.
    significant_bits = min(significant_bits, 24)

    # With simple (non-lossy) formats, the peak threshold must account for:
    #   * quantization error (1 LSB peak)
    #   * dither (disabled when possible)
    #   * error introduced by FFT 32-bit floating point calculations
    return AudioVerificationTraits(
        (1.0 / (2 ** (significant_bits - 1))) + _FFT_ERROR)

def _make_simple_audio_verification_traits_factory(
    *,
    default_traits: AudioVerificationTraits =
        AudioVerificationTraits(0.0, truncate=True),
    sample_rate_traits: _Iterable[tuple[int, AudioVerificationTraits]] = ()
) -> AudioVerificationTraitsFactory:
    traits_map = dict(sample_rate_traits)
    return lambda r: traits_map.get(r, default_traits)

class AudioCodecTraits(_EnumElementTraits):

    __slots__ = ("__bit_depth", "__filter", "__verification")

    __bit_depth: int | None
    __filter: AudioCodecFilterCallable | None
    __verification: AudioVerificationTraits | AudioVerificationTraitsFactory

    def __init__(
        self,
        verification: AudioVerificationTraits | AudioVerificationTraitsFactory,
        description: str,
        /,
        *,
        bit_depth: int | None = None,
        filter: AudioCodecFilterCallable | None = None,
        identifier: str | None = None
    ):
        super().__init__(description, identifier=identifier)
        self.__bit_depth = bit_depth
        self.__filter = filter
        self.__verification = verification

    @property
    def bit_depth(self, /) -> int | None:
        return self.__bit_depth

    @property
    def filter(self, /) -> AudioCodecFilterCallable | None:
        return self.__filter

    @property
    def verification(
        self,
        /
    ) -> AudioVerificationTraits | AudioVerificationTraitsFactory:
        return self.__verification

###############################################################################
# to_amplitude()
###############################################################################

def to_amplitude(dbfs: float, /) -> float:
    return 10.0 ** (dbfs / 20.0)

###############################################################################
# to_dbfs()
###############################################################################

def to_dbfs(amplitude: float, /) -> float:
    return 20.0 * _log10(abs(amplitude))

###############################################################################
# AudioCodec
###############################################################################

# In the case of lossless codecs, verification is based on quantization noise.
#
# In the case of lossy codecs, we either find the amount of maximum expected
# noise in the codec spec, or listen back to the audio, make sure it sounds as
# expected (given expectations for the codec), and determine the peak power
# threshold by calculating the peak power levels for the converted audio and
# adding 0.5 dBFS.
#
# I'm open to better ideas, especially ideas that don't involve arbitrary,
# hand-waving heuristics. :)

def _filter_non_endian_codec(
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    return endianness == AudioEndianness.FILE

def _generate_peak_threshold_heuristic(measurement: float) -> float:
    return to_amplitude(to_dbfs(measurement) + 0.5)

_GSM610_SAMPLE_RATE_TRAITS = (
    (
        8000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.02505148370936905),
            # XXX: This `truncate` flag is specific to non-raw GSM 6.10 encoded
            # data.  Raw GSM 6.10 doesn't require truncation.
            truncate=True)
    ),
    (
        44100,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.013034273521694506),
            truncate=True)
    ),
    (
        48000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.015819335965750116))
    ),
    (
        96000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.047135446035359685))
    ),
    (
        192000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.03947685754535875))
    )
)

_IMA_ADPCM_SAMPLE_RATE_TRAITS = (
    (
        8000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.006252335098007071),
            truncate=True)
    ),
    (
        44100,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.00048683486234998255),
            truncate=True)
    ),
    (
        48000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.0005508763178212414),
            truncate=True)
    ),
    (
        96000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.0004602904009630314),
            truncate=True)
    ),
    (
        192000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.00045198185179317187),
            truncate=True)
    )
)

_MS_ADPCM_SAMPLE_RATE_TRAITS = (
    (
        8000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.0052414875781024994),
            truncate=True)
    ),
    (
        44100,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.0011549059523958322),
            truncate=True)
    ),
    (
        48000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.0004158540339507244),
            truncate=True)
    ),
    (
        96000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.000279134191812469),
            truncate=True)
    ),
    (
        192000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.00015486653655982785),
            truncate=True)
    )
)

_VORBIS_SAMPLE_RATE_TRAITS = (
    (
        8000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.004740372587211187))
    ),
    (
        44100,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.005616179961156487))
    ),
    (
        48000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.006967447965442575))
    ),
    (
        96000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.0072736003032334295))
    ),
    (
        192000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.005363785322844539))
    )
)

_VOX_ADPCM_SAMPLE_RATE_TRAITS = (
    (
        8000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.004527997647256441))
    ),
    (
        44100,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.00041370846703035286))
    ),
    (
        48000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.0004498744674582031))
    ),
    (
        96000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(0.00021927359915857844))
    ),
    (
        192000,
        AudioVerificationTraits(
            _generate_peak_threshold_heuristic(7.91664771863067e-05))
    )
)

class AudioCodec(_Enum):

    PCM_S8 = AudioCodecTraits(
        _make_simple_audio_verification_traits(8),
        "Signed 8-bit integer PCM", bit_depth=8,
        filter=_filter_non_endian_codec)

    PCM_U8 = AudioCodecTraits(
        _make_simple_audio_verification_traits(8),
        "Unsigned 8-bit integer PCM", bit_depth=8,
        filter=_filter_non_endian_codec)

    PCM_S16 = AudioCodecTraits(
        _make_simple_audio_verification_traits(16),
        "Signed 16-bit integer PCM", bit_depth=16)

    PCM_S24 = AudioCodecTraits(
        _make_simple_audio_verification_traits(24),
        "Signed 24-bit integer PCM", bit_depth=24)

    PCM_S32 = AudioCodecTraits(
        _make_simple_audio_verification_traits(32),
        "Signed 32-bit integer PCM", bit_depth=32)

    PCM_F32 = AudioCodecTraits(
        _make_simple_audio_verification_traits(24),
        "32-bit floating point PCM", bit_depth=32)

    PCM_F64 = AudioCodecTraits(
        _make_simple_audio_verification_traits(53),
        "64-bit floating point PCM", bit_depth=64)

    # The number of bits of precision in ulaw vary, but peak quantization
    # happens at the highest values, wherein only the six most significant bits
    # are kept (not including the sign bit):
    #
    # https://www.sput.nl/internet/voip/aulaw.html#discbits
    ULAW = AudioCodecTraits(
        _make_simple_audio_verification_traits(7),
        "U-law encoded data", bit_depth=8, filter=_filter_non_endian_codec)

    # The number of bits of precision in alaw vary, but peak quantization
    # happens at the highest values, wherein only the six most significant bits
    # are kept (not including the sign bit):
    #
    # https://www.sput.nl/internet/voip/aulaw.html#discbits
    ALAW = AudioCodecTraits(
        _make_simple_audio_verification_traits(7),
        "A-law encoded data", bit_depth=8, filter=_filter_non_endian_codec)

    # Verification traits inferred by testing `sox` encoded data.
    IMA_ADPCM = AudioCodecTraits(
        _make_simple_audio_verification_traits_factory(
            sample_rate_traits=_IMA_ADPCM_SAMPLE_RATE_TRAITS),
        "IMA ADPCM encoded data", bit_depth=4)

    # Verification traits inferred by testing `sox` encoded data.
    MS_ADPCM = AudioCodecTraits(
        _make_simple_audio_verification_traits_factory(
            sample_rate_traits=_MS_ADPCM_SAMPLE_RATE_TRAITS),
        "Microsoft ADPCM encoded data", bit_depth=4)

    # Raw GSM 6.10 encoded data is typically written in 160 sample chunks,
    # meaning that any sample rate that isn't evenly divisible by 160 is going
    # to have a tail of samples at the end (hence the variable truncation
    # behavior in verification traits).
    #
    # Wav-ish GSM 6.10 encoded data is typically written in 320 sample chunks,
    # and the number of samples must be even, resulting in padding when one of
    # the conditions doesn't hold.
    #
    # Verification traits inferred by testing `sox` encoded data.
    GSM610 = AudioCodecTraits(
        _make_simple_audio_verification_traits_factory(
            sample_rate_traits=_GSM610_SAMPLE_RATE_TRAITS),
        "GSM 6.10 encoded data")

    # Verification traits inferred by testing `sox` encoded data.
    VOX_ADPCM = AudioCodecTraits(
        _make_simple_audio_verification_traits_factory(
            sample_rate_traits=_VOX_ADPCM_SAMPLE_RATE_TRAITS),
        "Oki Dialogic ADPCM encoded data", bit_depth=4)

    # Verification traits inferred by testing `sox` encoded data.
    #
    # Note that we don't attempt to set the bitrate, so encoded files are based
    # on whatever the default settings are for the codec in `sox`.
    VORBIS = AudioCodecTraits(
        _make_simple_audio_verification_traits_factory(
            sample_rate_traits=_VORBIS_SAMPLE_RATE_TRAITS),
        "Xiph Vorbis encoded data")



    # The commented out audio codecs below this line are codecs we could
    # *potentially* support using `libsndfile`, but aren't tested against
    # encoded data.



    # NMS_ADPCM_16 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "16kbs NMS G721-variant encoded data", bit_depth=2)

    # NMS_ADPCM_24 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "24kbs NMS G721-variant encoded data", bit_depth=3)

    # NMS_ADPCM_32 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "32kbs NMS G721-variant encoded data", bit_depth=4)

    # G721_32 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "32 kbps G721 ADPCM encoded data", bit_depth=4)

    # G723_24 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "24 kbps G723 ADPCM encoded data", bit_depth=3)

    # G723_40 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "40 kbps G723 ADPCM encoded data", bit_depth=5)

    # DWVW_12 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "12-bit delta width variable word encoded data", bit_depth=12)

    # DWVW_16 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "16-bit delta width variable word encoded data", bit_depth=16)

    # DWVW_24 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "24-bit delta width variable word encoded data", bit_depth=24)

    # DWVW_N = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "N-bit delta width variable word encoded data")

    # DPCM_8 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits(8),
    #     "8-bit differential PCM", bit_depth=8)

    # DPCM_16 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits(8),
    #     "16-bit differential PCM", bit_depth=16)

    # OPUS = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "Xiph/Skype Opus encoded data")

    # ALAC_16 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits(16),
    #     "Apple Lossless Audio Codec (16 bit)", bit_depth=16)

    # ALAC_20 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits(20),
    #     "Apple Lossless Audio Codec (20 bit)", bit_depth=20)

    # ALAC_24 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits(24),
    #     "Apple Lossless Audio Codec (24 bit)", bit_depth=24)

    # ALAC_32 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits(32),
    #     "Apple Lossless Audio Codec (32 bit)", bit_depth=32)

    # MPEG_LAYER_1 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "MPEG-1 Audio Layer I")

    # MPEG_LAYER_2 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "MPEG-1 Audio Layer II")

    # MPEG_LAYER_3 = AudioCodecTraits(
    #     _make_simple_audio_verification_traits_factory(),
    #     "MPEG-2 Audio Layer III")

###############################################################################
# get_audio_verification_traits()
###############################################################################

def get_audio_verification_traits(
    codec: AudioCodec,
    sample_rate: int,
    /
) -> AudioVerificationTraits:
    v = codec.value.verification
    match v:
        case AudioVerificationTraits():
            return v
        # We'd like to be able to create a `case` statement that matches
        # against `AudioVerificationTraitsFactory`, but that's not currently
        # possible.
        case f if callable(f):
            return v(sample_rate)
        case _:
            _assert_never(v)

###############################################################################
# AudioFormatFilterCallable
###############################################################################

type AudioFormatFilterCallable = \
    Callable[[AudioCodec, AudioEndianness, int, int], bool]

###############################################################################
# AudioFormatTraits
###############################################################################

class AudioFormatTraits(_EnumElementTraits):

    __slots__ = ("__extension", "__filter")

    __extension: str | None
    __filter: AudioFormatFilterCallable

    def __init__(
        self,
        description: str,
        filter: AudioFormatFilterCallable,
        /,
        *,
        extension: str | None = None,
        identifier: str | None = None
    ):
        super().__init__(description, identifier=identifier)
        self.__extension = extension
        self.__filter = filter

    @property
    def extension(self, /) -> str | None:
        return self.__extension

    @property
    def filter(self, /) -> AudioFormatFilterCallable:
        return self.__filter

###############################################################################
# AudioFormat
###############################################################################

# Filter functions *mostly* just mirror `libsndfile` for the time being.

def _filter_aiff(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case AudioCodec.PCM_S16 | AudioCodec.PCM_S24 | AudioCodec.PCM_S32:
            return True
        case (
            AudioCodec.PCM_U8 |
            AudioCodec.PCM_S8 |
            AudioCodec.PCM_F32 |
            AudioCodec.PCM_F64 |
            AudioCodec.ULAW |
            AudioCodec.ALAW
        ):
            return endianness == AudioEndianness.FILE
        case (
            #AudioCodec.DWVW_12 |
            #AudioCodec.DWVW_16 |
            #AudioCodec.DWVW_24 |
            AudioCodec.GSM610
        ):
            return (endianness == AudioEndianness.FILE) and \
                (channel_count == 1)
        case AudioCodec.IMA_ADPCM:
            return (endianness == AudioEndianness.FILE) and \
                (channel_count <= 2)
        case _:
            return False

def _filter_au(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            AudioCodec.PCM_S8 |
            AudioCodec.PCM_S16 |
            AudioCodec.PCM_S24 |
            AudioCodec.PCM_S32 |
            AudioCodec.PCM_F32 |
            AudioCodec.PCM_F64 |
            AudioCodec.ULAW |
            AudioCodec.ALAW
        ):
            return True
        #case AudioCodec.G721_32 | AudioCodec.G723_24 | AudioCodec.G723_40:
        #    return channel_count == 1
        case _:
            return False

def _filter_avr(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case AudioCodec.PCM_U8 | AudioCodec.PCM_S8 | AudioCodec.PCM_S16:
            return (endianness != AudioEndianness.LITTLE) and \
                (channel_count <= 2)
        case _:
            return False

def _filter_caf(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            AudioCodec.PCM_S8 |
            AudioCodec.PCM_S16 |
            AudioCodec.PCM_S24 |
            AudioCodec.PCM_S32 |
            AudioCodec.PCM_F32 |
            AudioCodec.PCM_F64 |
            AudioCodec.ULAW |
            AudioCodec.ALAW #|
            #AudioCodec.ALAC_16 |
            #AudioCodec.ALAC_20 |
            #AudioCodec.ALAC_24 |
            #AudioCodec.ALAC_32
        ):
            return True
        case _:
            return False

def _filter_flac(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case AudioCodec.PCM_S8 | AudioCodec.PCM_S16 | AudioCodec.PCM_S24:
            return (channel_count <= 8) and \
                (endianness == AudioEndianness.FILE)
        case _:
            return False

def _filter_htk(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        # HTK encodes the sample rate as the number of 100 nanosecond periods
        # per sample, meaning there's not enough precision to represent certain
        # sample rates, especially larger sample rates.
        case AudioCodec.PCM_S16:
            return (channel_count == 1) and \
                (endianness != AudioEndianness.LITTLE) and \
                (int(10000000 / int(10000000 / sample_rate)) == sample_rate)
        case _:
            return False

def _filter_ircam(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            AudioCodec.PCM_S16 |
            AudioCodec.PCM_S32 |
            AudioCodec.PCM_F32 |
            AudioCodec.ULAW |
            AudioCodec.ALAW
        ):
            return channel_count <= 256
        case _:
            return False

def _filter_mat4(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            AudioCodec.PCM_S16 |
            AudioCodec.PCM_S32 |
            AudioCodec.PCM_F32 |
            AudioCodec.PCM_F64
        ):
            return True
        case _:
            return False

def _filter_mat5(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            AudioCodec.PCM_U8 |
            AudioCodec.PCM_S16 |
            AudioCodec.PCM_S32 |
            AudioCodec.PCM_F32 |
            AudioCodec.PCM_F64
        ):
            return True
        case _:
            return False

def _filter_mpc2k(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case AudioCodec.PCM_S16:
            return (channel_count <= 2) and (endianness != AudioEndianness.BIG)
        case _:
            return False

def _filter_mpeg(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        #case (
        #    AudioCodec.MPEG_LAYER_1 |
        #    AudioCodec.MPEG_LAYER_2 |
        #    AudioCodec.MPEG_LAYER_3
        #):
        #    return (channel_count <= 2) and \
        #        (endianness == AudioEndianness.FILE)
        case _:
            return False

def _filter_nist(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            AudioCodec.PCM_S8 |
            AudioCodec.PCM_S16 |
            AudioCodec.PCM_S24 |
            AudioCodec.PCM_S32 |
            AudioCodec.ULAW |
            AudioCodec.ALAW
        ):
            return True
        case _:
            return False

def _filter_ogg(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            #AudioCodec.OPUS |
            AudioCodec.VORBIS
        ):
            return endianness == AudioEndianness.FILE
        case _:
            return False

def _filter_paf(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case AudioCodec.PCM_S8 | AudioCodec.PCM_S16 | AudioCodec.PCM_S24:
            return True
        case _:
            return False

def _filter_pvf(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case AudioCodec.PCM_S8 | AudioCodec.PCM_S16 | AudioCodec.PCM_S32:
            return True
        case _:
            return False

def _filter_raw(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            AudioCodec.PCM_U8 |
            AudioCodec.PCM_S8 |
            AudioCodec.PCM_S16 |
            AudioCodec.PCM_S24 |
            AudioCodec.PCM_S32 |
            AudioCodec.PCM_F32 |
            AudioCodec.PCM_F64 |
            AudioCodec.ULAW |
            AudioCodec.ALAW
        ):
            return True
        case (
            #AudioCodec.DWVW_12 |
            #AudioCodec.DWVW_16 |
            #AudioCodec.DWVW_24 |
            AudioCodec.GSM610 |
            AudioCodec.VOX_ADPCM #|
            #AudioCodec.NMS_ADPCM_16 |
            #AudioCodec.NMS_ADPCM_24 |
            #AudioCodec.NMS_ADPCM_32
        ):
            return channel_count == 1
        case _:
            return False

def _filter_rf64(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            AudioCodec.PCM_U8 |
            AudioCodec.PCM_S16 |
            AudioCodec.PCM_S24 |
            AudioCodec.PCM_S32 |
            AudioCodec.PCM_F32 |
            AudioCodec.PCM_F64 |
            AudioCodec.ULAW |
            AudioCodec.ALAW
        ):
            return endianness != AudioEndianness.BIG
        case _:
            return False

# def _filter_sd2(
#     codec: AudioCodec,
#     endianness: AudioEndianness,
#     sample_rate: int,
#     channel_count: int,
#     /
# ) -> bool:
#     match codec:
#         case (
#             AudioCodec.PCM_S8 |
#             AudioCodec.PCM_S16 |
#             AudioCodec.PCM_S24 |
#             AudioCodec.PCM_S32
#         ):
#             return endianness != AudioEndianness.LITTLE
#         case _:
#             return False

def _filter_sds(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        # SDS encodes the sample rate as the number of nanoseconds per sample,
        # meaning there's not enough precision to represent certain sample
        # rates, especially larger sample rates.
        case AudioCodec.PCM_S8 | AudioCodec.PCM_S16 | AudioCodec.PCM_S24:
            return (channel_count == 1) and \
                (endianness != AudioEndianness.LITTLE) and \
                (
                    int(1000000000 / int(1000000000 / sample_rate)) ==
                    sample_rate
                )
        case _:
            return False

def _filter_svx(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case AudioCodec.PCM_S8 | AudioCodec.PCM_S16:
            return (channel_count == 1) and \
                (endianness != AudioEndianness.LITTLE) and \
                (sample_rate <= 65535)
        case _:
            return False

def _filter_voc(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    # `libsndfile` attempts to generate the oldest format it can for VOC files.
    # 8-bit mono files use the basic format.  8-bit stereo files use the
    # extended header.  16-bit files use a newer header that can hold any
    # 32-bit sample rate.
    #
    # The basic and extended headers can only accurately represent certain
    # sample rates (as detailed below).  `libsndfile` will silently change an
    # unsupported sample rate to the nearest supported sample rate.
    #
    # With u-law and a-law encodings, `libsndfile` appears to append one
    # additional frame to the output.
    #
    # XXX: File an upstream bug.
    match codec:
        case AudioCodec.PCM_U8:
            if channel_count == 1:
                m = 256
                n = 1000000
            else:
                m = 65536
                n = 12800000
            time_constant = int(m - (n / sample_rate))
            computed_sample_rate = int((-n) / (time_constant - m))
            if sample_rate != computed_sample_rate:
                return False
            return (sample_rate == computed_sample_rate) and \
                (channel_count <= 2) and (endianness != AudioEndianness.BIG)
        case AudioCodec.PCM_S16: #| AudioCodec.ULAW | AudioCodec.ALAW:
            return (channel_count <= 2) and (endianness != AudioEndianness.BIG)
        case _:
            return False

def _filter_w64(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            AudioCodec.PCM_U8 |
            AudioCodec.PCM_S16 |
            AudioCodec.PCM_S24 |
            AudioCodec.PCM_S32 |
            AudioCodec.PCM_F32 |
            AudioCodec.PCM_F64 |
            AudioCodec.ULAW |
            AudioCodec.ALAW
        ):
            return endianness != AudioEndianness.BIG
        case AudioCodec.IMA_ADPCM | AudioCodec.MS_ADPCM:
            return (channel_count <= 2) and (endianness != AudioEndianness.BIG)
        case AudioCodec.GSM610:
            return (channel_count == 1) and (endianness != AudioEndianness.BIG)
        case _:
            return False

def _filter_wav(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            AudioCodec.PCM_U8 |
            AudioCodec.PCM_S16 |
            AudioCodec.PCM_S24 |
            AudioCodec.PCM_S32 |
            AudioCodec.PCM_F32 |
            AudioCodec.PCM_F64 |
            AudioCodec.ULAW |
            AudioCodec.ALAW
        ):
            return True
        case (
            AudioCodec.IMA_ADPCM |
            AudioCodec.MS_ADPCM #|
            #AudioCodec.MPEG_LAYER_3
        ):
            return (channel_count <= 2)
        case (
            AudioCodec.GSM610 #|
            #AudioCodec.G721_32 |
            #AudioCodec.NMS_ADPCM_16 |
            #AudioCodec.NMS_ADPCM_24 |
            #AudioCodec.NMS_ADPCM_32
        ):
            return (channel_count == 1)
        case _:
            return False

def _filter_wavex(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case (
            AudioCodec.PCM_U8 |
            AudioCodec.PCM_S16 |
            AudioCodec.PCM_S24 |
            AudioCodec.PCM_S32 |
            AudioCodec.PCM_F32 |
            AudioCodec.PCM_F64 |
            AudioCodec.ULAW |
            AudioCodec.ALAW
        ):
            return endianness != AudioEndianness.BIG
        case _:
            return False

def _filter_wve(
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    match codec:
        case AudioCodec.ALAW:
            # XXX: File an upstream bug.
            #
            # `libsndfile` will silently write 8000 Hz a-law data regardless of
            # the sample rate that you request.
            return (channel_count == 1) and \
                (endianness == AudioEndianness.FILE) and \
                (sample_rate == 8000)
        case _:
            return False

# def _filter_xi(
#     codec: AudioCodec,
#     endianness: AudioEndianness,
#     sample_rate: int,
#     channel_count: int,
#     /
# ) -> bool:
#     match codec:
#         #case AudioCodec.DPCM_8 | AudioCodec.DPCM_16:
#         #    return channel_count == 1
#         case _:
#             return False

class AudioFormat(_Enum):

    AIFF = AudioFormatTraits("Apple/SGI AIFF format", _filter_aiff)

    AU = AudioFormatTraits("Sun/NeXT AU format", _filter_au)

    AVR = AudioFormatTraits("Audio Visual Research format", _filter_avr)

    CAF = AudioFormatTraits("Core Audio File format", _filter_caf)

    #FLAC = AudioFormatTraits("FLAC lossless file format", _filter_flac)

    HTK = AudioFormatTraits("HMM Tool Kit format", _filter_htk)

    IRCAM = AudioFormatTraits(
        "Berkeley/IRCAM/CARL format", _filter_ircam, extension=".sf")

    MAT4 = AudioFormatTraits(
        "Matlab 4.2 / GNU Octave 2.0 format", _filter_mat4, extension=".mat")

    MAT5 = AudioFormatTraits(
        "Matlab 5.0 / GNU Octave 2.1 format", _filter_mat5, extension=".mat")

    #MPC2K = AudioFormatTraits(
    #    "Akai MPC 2000 sampler format", _filter_mpc2k, extension=".snd")

    #MPEG = AudioFormatTraits("MPEG 1/2 audio stream format", _filter_mpeg)

    NIST = AudioFormatTraits(
        "Sphere NIST format", _filter_nist, extension=".wav")

    OGG = AudioFormatTraits("Xiph OGG container format", _filter_ogg)

    PAF = AudioFormatTraits("Ensoniq PARIS file format", _filter_paf)

    PVF = AudioFormatTraits("Portable Voice Format", _filter_pvf)

    RAW = AudioFormatTraits("Raw data", _filter_raw)

    #RF64 = AudioFormatTraits("RF64 WAV file format", _filter_rf64)

    # SD2 = AudioFormatTraits("Sound Designer 2 format", _filter_sd2)

    # XXX: Open upstream bug.
    #
    # `libsndfile` sometimes writes 10-20 zero value samples at the end of a
    # sample instead of the samples that should be written.
    #SDS = AudioFormatTraits("Midi Sample Dump Standard format", _filter_sds)

    SVX = AudioFormatTraits(
        "Amiga IFF / SVX8 / SV16 format", _filter_svx, extension=".iff")

    VOC = AudioFormatTraits("VOC file format", _filter_voc)

    W64 = AudioFormatTraits(
        "Sonic Foundry’s 64 bit RIFF/WAV format", _filter_w64)

    WAV = AudioFormatTraits("Microsoft WAV format", _filter_wav)

    WAVEX = AudioFormatTraits(
        "MS WAVE with WAVEFORMATEX", _filter_wavex, extension=".wav")

    WVE = AudioFormatTraits("Psion WVE format", _filter_wve)

    #XI = AudioFormatTraits(
    #    "Fasttracker 2 Extended Instrument format", _filter_xi)

###############################################################################
# get_audio_format_extension()
###############################################################################

def get_audio_format_extension(format: AudioFormat, /) -> str:
    extension = format.value.extension
    if extension is None:
        extension = f".{format.name.lower()}"
    return extension

###############################################################################
# is_audio_supported()
###############################################################################

def is_audio_supported(
    format: AudioFormat,
    codec: AudioCodec,
    endianness: AudioEndianness,
    sample_rate: int,
    channel_count: int,
    /
) -> bool:
    if not format.value.filter(codec, endianness, sample_rate, channel_count):
        return False
    codec_filter = codec.value.filter
    return (codec_filter is None) or \
        codec_filter(endianness, sample_rate, channel_count)

###############################################################################
# AudioSeekOriginTraits
###############################################################################

class AudioSeekOriginTraits(_EnumElementTraits):

    __slots__ = ()

###############################################################################
# AudioSeekOrigin
###############################################################################

class AudioSeekOrigin(_Enum):

    CURRENT = AudioSeekOriginTraits(
        "Seek from the current frame in the audio stream.")

    START = AudioSeekOriginTraits(
        "Seek from the first frame in the audio stream.")

    END = AudioSeekOriginTraits(
        "Seek from just after the last frame in the audio stream.")

###############################################################################
# COMMON_SAMPLE_RATES
###############################################################################

# We don't want to add *too* many sample rates here, as each new sample rate
# results in a slew of tests, reference audio, and encoded audio, but we do
# want to cover a wide spectrum (pun intended) of sample rates and provide
# adequate testing for audio formats, codecs, etc.
#
# The sample rates we test with must be even so that the sample counts play
# nicely with `kissfft`.

COMMON_SAMPLE_RATES = (8000, 44100, 48000, 96000, 192000)

###############################################################################
# COMMON_CHANNEL_COUNTS
###############################################################################

COMMON_CHANNEL_COUNTS = (1, 2)

###############################################################################
# AudioReferenceWave
###############################################################################

def _verify_channel_count(n: int, /) -> int:
    if not (1 <= n <= 1024):
        raise ValueError(f"{n}: invalid channel count")
    return n

def _verify_sample_rate(n: int, /) -> int:
    if not (1 <= n <= 0xffffff):
        raise ValueError(f"{n}: invalid sample rate")
    return n

class AudioReferenceWave:

    __slots__ = ("__channel_count", "__sample_rate")

    __channel_count: int
    __sample_rate: int

    def __init__(self, sample_rate, channel_count, /):
        self.__channel_count = _verify_channel_count(channel_count)
        self.__sample_rate = _verify_sample_rate(sample_rate)

    @property
    def channel_count(self, /) -> int:
        return self.__channel_count

    @property
    def sample_rate(self, /) -> int:
        return self.__sample_rate

###############################################################################
# get_audio_reference_wave_path()
###############################################################################

_AUDIO_REFERENCE_CODEC_ID = _get_enum_element_identifier(AudioCodec.PCM_F64)
_AUDIO_REFERENCE_ENDIANNESS_PREFIX = _get_enum_element_identifier(
    AudioEndianness.LITTLE)[0]
_AUDIO_REFERENCE_FORMAT_EXTENSION = get_audio_format_extension(AudioFormat.RAW)

def get_audio_reference_wave_path(wave: AudioReferenceWave, /) -> _Path:
    return _REFERENCE_AUDIO_PATH / \
        f"{wave.sample_rate}Hz-{wave.channel_count}ch-" \
        f"{_AUDIO_REFERENCE_CODEC_ID}-{_AUDIO_REFERENCE_ENDIANNESS_PREFIX}e" \
        f"{_AUDIO_REFERENCE_FORMAT_EXTENSION}"

###############################################################################
# AUDIO_REFERENCE_WAVES
###############################################################################

AUDIO_REFERENCE_WAVES = tuple(
    AudioReferenceWave(s, n)
    for s in COMMON_SAMPLE_RATES
    for n in COMMON_CHANNEL_COUNTS)
