#ifndef __SYNTHCLONE_SAMPLESTREAM_H__
#define __SYNTHCLONE_SAMPLESTREAM_H__

#include <sndfile.h>

#include <synthclone/sample.h>
#include <synthclone/types.h>

namespace synthclone {

    /**
     * Base class for reading/writing Sample files.  The object uses the
     * excellent libsndfile in its implementation, which can be found at
     * http://www.mega-nerd.com/libsndfile/.
     *
     * @sa
     *   SampleInputStream, SampleOutputStream
     */

    class SampleStream: public QObject {

        Q_OBJECT

    public:

        /**
         * Contains format endian types supported by sample streams.  Adding
         * format endian types supported by 'libsndfile' is a trivial task.  If
         * you have a format endian type that you'd like to see supported, then
         * let me know.
         */

        enum EndianType {
            ENDIANTYPE_FILE = SF_ENDIAN_FILE,
            ENDIANTYPE_LITTLE = SF_ENDIAN_LITTLE,
            ENDIANTYPE_BIG = SF_ENDIAN_BIG,
            ENDIANTYPE_CPU = SF_ENDIAN_CPU
        };

        /**
         * Offsets are used to set the position of the stream using the seek()
         * call.
         */

        enum Offset {
            OFFSET_CURRENT = 0,
            OFFSET_END = 1,
            OFFSET_START = 2
        };

        /**
         * Contains format sub-types supported by sample streams.  Adding format
         * sub-types supported by 'libsndfile' is a trivial task.  If you have a
         * format sub-type that you'd like to see supported, then let me know.
         */

        enum SubType {
            SUBTYPE_PCM_S8 = SF_FORMAT_PCM_S8,
            SUBTYPE_PCM_U8 = SF_FORMAT_PCM_U8,
            SUBTYPE_PCM_16 = SF_FORMAT_PCM_16,
            SUBTYPE_PCM_24 = SF_FORMAT_PCM_24,
            SUBTYPE_PCM_32 = SF_FORMAT_PCM_32,
            SUBTYPE_FLOAT = SF_FORMAT_FLOAT,
            SUBTYPE_DOUBLE = SF_FORMAT_DOUBLE,

            SUBTYPE_ULAW = SF_FORMAT_ULAW,
            SUBTYPE_ALAW = SF_FORMAT_ALAW,
            SUBTYPE_IMA_ADPCM = SF_FORMAT_IMA_ADPCM,
            SUBTYPE_MS_ADPCM = SF_FORMAT_MS_ADPCM,

            SUBTYPE_GSM610 = SF_FORMAT_GSM610,
            SUBTYPE_VOX_ADPCM = SF_FORMAT_VOX_ADPCM,

            SUBTYPE_G721_32 = SF_FORMAT_G721_32,
            SUBTYPE_G723_24 = SF_FORMAT_G723_24,
            SUBTYPE_G723_40 = SF_FORMAT_G723_40,

            SUBTYPE_DWVW_12 = SF_FORMAT_DWVW_12,
            SUBTYPE_DWVW_16 = SF_FORMAT_DWVW_16,
            SUBTYPE_DWVW_24 = SF_FORMAT_DWVW_24,
            SUBTYPE_DWVW_N = SF_FORMAT_DWVW_N,

            SUBTYPE_DPCM_8 = SF_FORMAT_DPCM_8,
            SUBTYPE_DPCM_16 = SF_FORMAT_DPCM_16,

            SUBTYPE_VORBIS = SF_FORMAT_VORBIS
        };

        /**
         * Contains format types supported by sample streams.  Adding formats
         * supported by 'libsndfile' is a trivial task.  If you have a format
         * that you'd like to see supported, then let me know.
         */

        enum Type {
            TYPE_AIFF = SF_FORMAT_AIFF,
            TYPE_AU = SF_FORMAT_AU,
            TYPE_AVR = SF_FORMAT_AVR,
            TYPE_CAF = SF_FORMAT_CAF,
            TYPE_FLAC = SF_FORMAT_FLAC,
            TYPE_HTK = SF_FORMAT_HTK,
            TYPE_IRCAM = SF_FORMAT_IRCAM,
            TYPE_MAT4 = SF_FORMAT_MAT4,
            TYPE_MAT5 = SF_FORMAT_MAT5,
            TYPE_MPC2K = SF_FORMAT_MPC2K,
            TYPE_NIST = SF_FORMAT_NIST,
            TYPE_OGG = SF_FORMAT_OGG,
            TYPE_PAF = SF_FORMAT_PAF,
            TYPE_PVF = SF_FORMAT_PVF,
            TYPE_RAW = SF_FORMAT_RAW,
            TYPE_RF64 = SF_FORMAT_RF64,
            TYPE_SD2 = SF_FORMAT_SD2,
            TYPE_SDS = SF_FORMAT_SDS,
            TYPE_SVX = SF_FORMAT_SVX,
            TYPE_VOC = SF_FORMAT_VOC,
            TYPE_W64 = SF_FORMAT_W64,
            TYPE_WAV = SF_FORMAT_WAV,
            TYPE_WAVEX = SF_FORMAT_WAVEX,
            TYPE_WVE = SF_FORMAT_WVE,
            TYPE_XI = SF_FORMAT_XI
        };

        /**
         * Closes the sample stream, which causes any unwritten data to be
         * written to the sample.  This is automatically called by the
         * destructor.
         */

        void
        close();

        /**
         * Gets the channel count for the sample.
         *
         * @returns
         *   The channel count.
         */

        SampleChannelCount
        getChannels() const;

        /**
         * Gets the format endian type of the underlying sample.
         *
         * @returns
         *   The format endian type.
         */

        EndianType
        getEndianType() const;

        /**
         * Gets the total number of frames in this sample.
         *
         * @returns
         *   The frame count.
         */

        SampleFrameCount
        getFrames() const;

        /**
         * Gets the sample rate for this sample.
         *
         * @returns
         *   The sample rate.
         */

        SampleRate
        getSampleRate() const;

        /**
         * Gets the format sub-type of the underlying sample.
         *
         * @returns
         *   The format sub-type.
         */

        SubType
        getSubType() const;

        /**
         * Gets the format type of the underlying sample.
         *
         * @returns
         *   The format type.
         */

        Type
        getType() const;

        /**
         * Sets the position of the stream in the sample file.
         *
         * @param frames
         *   The absolute frame count from the offset where the stream should be
         *   positioned.
         *
         * @param offset
         *   The offset for stream positioning.
         *
         * @returns
         *   The new position.
         */

        SampleFrameCount
        seek(SampleFrameCount frames, Offset offset);

    protected:

        /**
         * Constructs a new SampleStream.  This constructor should not be called
         * directly; instead, subclasses of SampleStream should be used to
         * create stream objects.
         *
         * @param parent
         *   The parent object of the new stream object.
         *
         * @sa
         *   SampleInputStream, SampleOutputStream
         */

        explicit
        SampleStream(QObject *parent=0);

        virtual
        ~SampleStream();

        SNDFILE *handle;
        SF_INFO info;

    private:

        bool closed;

    };

}

#endif
