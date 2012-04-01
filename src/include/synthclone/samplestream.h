/*
 * libsynthclone - a plugin API for `synthclone`
 * Copyright (C) 2011-2012 Devin Anderson
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __SYNTHCLONE_SAMPLESTREAM_H__
#define __SYNTHCLONE_SAMPLESTREAM_H__

#include <QtCore/QObject>

#include <synthclone/types.h>

namespace synthclone {

    class SampleFile;

    /**
     * Base class for reading/writing Sample files.
     *
     * @sa
     *   SampleInputStream, SampleOutputStream
     */

    class SampleStream: public QObject {

        Q_OBJECT

    public:

        /**
         * Contains format endian types supported by sample streams.
         */

        enum EndianType {
            ENDIANTYPE_FILE = 0,
            ENDIANTYPE_LITTLE,
            ENDIANTYPE_BIG,
            ENDIANTYPE_CPU
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
         * Contains format sub-types supported by sample streams.
         */

        enum SubType {
            SUBTYPE_UNKNOWN = -1,
            SUBTYPE_PCM_S8 = 0,
            SUBTYPE_PCM_U8,
            SUBTYPE_PCM_16,
            SUBTYPE_PCM_24,
            SUBTYPE_PCM_32,
            SUBTYPE_FLOAT,
            SUBTYPE_DOUBLE,

            SUBTYPE_ULAW,
            SUBTYPE_ALAW,
            SUBTYPE_IMA_ADPCM,
            SUBTYPE_MS_ADPCM,

            SUBTYPE_GSM610,
            SUBTYPE_VOX_ADPCM,

            SUBTYPE_G721_32,
            SUBTYPE_G723_24,
            SUBTYPE_G723_40,

            SUBTYPE_DWVW_12,
            SUBTYPE_DWVW_16,
            SUBTYPE_DWVW_24,
            SUBTYPE_DWVW_N,

            SUBTYPE_DPCM_8,
            SUBTYPE_DPCM_16,

            SUBTYPE_VORBIS
        };

        /**
         * Contains format types supported by sample streams.
         */

        enum Type {
            TYPE_UNKNOWN = -1,
            TYPE_AIFF = 0,
            TYPE_AU,
            TYPE_AVR,
            TYPE_CAF,
            TYPE_FLAC,
            TYPE_HTK,
            TYPE_IRCAM,
            TYPE_MAT4,
            TYPE_MAT5,
            TYPE_MPC2K,
            TYPE_NIST,
            TYPE_OGG,
            TYPE_PAF,
            TYPE_PVF,
            TYPE_RAW,
            TYPE_RF64,
            TYPE_SD2,
            TYPE_SDS,
            TYPE_SVX,
            TYPE_VOC,
            TYPE_W64,
            TYPE_WAV,
            TYPE_WAVEX,
            TYPE_WVE,
            TYPE_XI
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
         * Gets a boolean indicating whether or not the stream is closed.
         *
         * @returns
         *   The boolean.
         */

        bool
        isClosed() const;

        /**
         * Sets the position of the stream in the sample file.
         *
         * @param frames
         *   The absolute frame count from the offset where the stream should
         *   be positioned.
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

        explicit
        SampleStream(QObject *parent=0);

        virtual
        ~SampleStream();

        SampleFile *file;

    };

}

#endif
