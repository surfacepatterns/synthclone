module;

#include <sndfile.h>

constexpr inline ::sf_count_t SF_COUNT_MAX_ = SF_COUNT_MAX;
#undef SF_COUNT_MAX

export module synthclone.external.sndfile;

export {

    constexpr inline ::sf_count_t SF_COUNT_MAX = SF_COUNT_MAX_;

    using ::SF_INFO;
    using ::SNDFILE;

    using ::sf_count_t;

    using ::SF_FORMAT_ENDMASK;
    using ::SF_FORMAT_SUBMASK;
    using ::SF_FORMAT_TYPEMASK;

    using ::SF_FORMAT_PCM_U8;
    using ::SF_FORMAT_PCM_S8;
    using ::SF_FORMAT_PCM_16;
    using ::SF_FORMAT_PCM_24;
    using ::SF_FORMAT_PCM_32;
    using ::SF_FORMAT_FLOAT;
    using ::SF_FORMAT_DOUBLE;
    using ::SF_FORMAT_ULAW;
    using ::SF_FORMAT_ALAW;
    using ::SF_FORMAT_IMA_ADPCM;
    using ::SF_FORMAT_MS_ADPCM;
    using ::SF_FORMAT_GSM610;
    using ::SF_FORMAT_VOX_ADPCM;
    using ::SF_FORMAT_VORBIS;

    using ::SF_ENDIAN_FILE;
    using ::SF_ENDIAN_BIG;
    using ::SF_ENDIAN_LITTLE;

    using ::SF_FORMAT_AIFF;
    using ::SF_FORMAT_AU;
    using ::SF_FORMAT_AVR;
    using ::SF_FORMAT_CAF;
    using ::SF_FORMAT_HTK;
    using ::SF_FORMAT_IRCAM;
    using ::SF_FORMAT_MAT4;
    using ::SF_FORMAT_MAT5;
    using ::SF_FORMAT_NIST;
    using ::SF_FORMAT_OGG;
    using ::SF_FORMAT_PAF;
    using ::SF_FORMAT_PVF;
    using ::SF_FORMAT_RAW;
    using ::SF_FORMAT_SVX;
    using ::SF_FORMAT_VOC;
    using ::SF_FORMAT_W64;
    using ::SF_FORMAT_WAV;
    using ::SF_FORMAT_WAVEX;
    using ::SF_FORMAT_WVE;

    using ::SF_ERR_NO_ERROR;

    using ::SFM_READ;
    using ::SFM_WRITE;

    using ::sf_close;
    using ::sf_error;
    using ::sf_format_check;
    using ::sf_open;
    using ::sf_read_float;
    using ::sf_seek;
    using ::sf_strerror;
    using ::sf_write_float;

}
