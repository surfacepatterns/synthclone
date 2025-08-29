from collections.abc import Callable as _Callable
from pathlib import Path as _Path

from ..._code import (
    CodeComposer as _CodeComposer,
    make_bool_literal as _make_bool_literal,
    make_string_literal as _make_string_literal,
    write_compact_test_case as _write_compact_test_case,
    writing_test_suite as _writing_test_suite,
)
from ..._scoped import (
    get_scoped_enum_element_identifier as _get_scoped_enum_element_identifier
)
from ..._util import (
    apply_template as _apply_template,
    make_timestamp as _make_timestamp
)

from ._audio import (
    AUDIO_REFERENCE_WAVES as _AUDIO_REFERENCE_WAVES,
    COMMON_SAMPLE_RATES as _COMMON_SAMPLE_RATES,
    AudioCodec as _AudioCodec,
    AudioEndianness as _AudioEndianness,
    AudioFormat as _AudioFormat,
    AudioReferenceWave as _AudioReferenceWave,
    get_audio_format_extension as _get_audio_format_extension,
    get_audio_reference_wave_path as _get_audio_reference_wave_path,
    get_audio_verification_traits as _get_audio_verification_traits,
    is_audio_supported as _is_audio_supported
)
from ._layout import (
    CORE_MODULE_PATH as _CORE_MODULE_PATH,
    ENCODED_AUDIO_PATH as _ENCODED_AUDIO_PATH,
    TEMPLATES_PATH as _TEMPLATES_PATH,
    TESTS_PATH as _TESTS_PATH
)

###############################################################################
# EncodeCallable
###############################################################################

type EncodeCallable = _Callable[
    [_AudioFormat, _AudioCodec, _AudioEndianness, _AudioReferenceWave, _Path],
    bool
]

###############################################################################
# write_decoder_test_assets()
###############################################################################

# XXX: This code is specific to the `lib` component, but can be shared amongst
# multiple scripts that create audio decoder tests.  Perhaps this should be in
# a common space in the `lib` directory.

_AUDIO_DECODER_TEST_TEMPLATE_PATH = \
    _TEMPLATES_PATH / "tests" / "audio_io_decoder_gen.cpp.tmpl"

def _write_decoder_test_case_assets(
    composer: _CodeComposer,
    encoder_id: str,
    format: _AudioFormat,
    codec: _AudioCodec,
    endianness: _AudioEndianness,
    wave: _AudioReferenceWave,
    encode: EncodeCallable,
    /
) -> bool:
    channel_count = wave.channel_count
    sample_rate = wave.sample_rate
    if not _is_audio_supported(
        format, codec, endianness, sample_rate, channel_count
    ):
        return False

    codec_id = _get_scoped_enum_element_identifier(codec)
    endianness_id = _get_scoped_enum_element_identifier(endianness)
    format_id = _get_scoped_enum_element_identifier(format)

    path = _ENCODED_AUDIO_PATH / encoder_id / \
        f"{format_id}--{codec_id}" / \
        f"{sample_rate}Hz-{channel_count}ch-{codec_id}-{endianness_id[0]}e" \
        f"{_get_audio_format_extension(format)}"

    result = encode(format, codec, endianness, wave, path)
    if result:
        verification = _get_audio_verification_traits(codec, sample_rate)

        _write_compact_test_case(
            composer,

            f"test_{format_id}_{codec_id}_{endianness_id}_{sample_rate}Hz_"
            f"{channel_count}ch",

            "synthclone::verify_encoded_reference_audio<"
            f"synthclone::audio_format::{format_id},"
            f"synthclone::audio_codec::{codec_id},"
            f"synthclone::audio_endianness::{endianness_id},"
            f"{sample_rate},"
            f"{channel_count},"
            f"{verification.read_offset},"
            f"{_make_bool_literal(not verification.truncate)},"
            f"{verification.peak_power_threshold}"
            ">("

            f"{_make_string_literal(str(path.relative_to(_CORE_MODULE_PATH)))}"

            ");")

    return result

def write_decoder_test_assets(encoder_id: str, encode: EncodeCallable, /):
    if not all(
        _get_audio_reference_wave_path(wave).is_file()
        for wave in _AUDIO_REFERENCE_WAVES
    ):
        raise FileNotFoundError("one or more reference waves not found")

    composer = _CodeComposer()
    with _writing_test_suite(composer, f"audio_io_decoder_{encoder_id}_gen"):
        for format in _AudioFormat:
            for codec in _AudioCodec:
                for wave in _AUDIO_REFERENCE_WAVES:
                    endian_specific = _write_decoder_test_case_assets(
                        composer, encoder_id, format, codec,
                        _AudioEndianness.BIG, wave, encode)
                    endian_specific |= _write_decoder_test_case_assets(
                        composer, encoder_id, format, codec,
                        _AudioEndianness.LITTLE, wave, encode)
                    if not endian_specific:
                        _write_decoder_test_case_assets(
                            composer, encoder_id, format, codec,
                            _AudioEndianness.FILE, wave, encode)
    test_suite = str(composer).strip()

    _apply_template(
        _AUDIO_DECODER_TEST_TEMPLATE_PATH,
        _TESTS_PATH / f"audio_io_decoder_{encoder_id}_gen.cpp",
        test_suite=test_suite, timestamp=_make_timestamp())
