/*
 * libsynthclone - a plugin API for `synthclone`
 * Copyright (C) 2011 Devin Anderson
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

#ifndef __SYNTHCLONE_SAMPLEOUTPUTSTREAM_H__
#define __SYNTHCLONE_SAMPLEOUTPUTSTREAM_H__

#include <synthclone/samplestream.h>

namespace synthclone {

    /**
     * Used to write sample data to a sample file.  The object uses the
     * excellent libsndfile in its implementation, which can be found at
     * http://www.mega-nerd.com/libsndfile/.
     */

    class SampleOutputStream: public SampleStream {

        Q_OBJECT

    public:

        /**
         * Creates a sample output stream object that writes a .wav file with
         * float values.
         *
         * @param sample
         *   The sample to write to.
         *
         * @param sampleRate
         *   The sample rate to use.
         *
         * @param channels
         *   The channel count.
         *
         * @param parent
         *   The parent object of the new stream.
         */

        SampleOutputStream(Sample &sample, SampleRate sampleRate,
                           SampleChannelCount channels, QObject *parent=0);

        /**
         * Creates a sample output stream object.
         *
         * @param sample
         *   The sample to write to.
         *
         * @param sampleRate
         *   The sample rate to use.
         *
         * @param channels
         *   The channel count.
         *
         * @param type
         *   The format type.
         *
         * @param subType
         *   The format sub-type.
         *
         * @param endianType
         *   The format endian-type.
         *
         * @param parent
         *   The parent object of the new stream.
         */

        SampleOutputStream(Sample &sample, SampleRate sampleRate,
                           SampleChannelCount channels, Type type,
                           SubType subType,
                           EndianType endianType=ENDIANTYPE_FILE,
                           QObject *parent=0);

        /**
         * Destroys the stream.  This will close the stream if it isn't closed.
         */

        virtual
        ~SampleOutputStream();

        /**
         * Closes the sample stream, which causes any unwritten data to be
         * written to the sample.  This is automatically called by the
         * destructor.
         */

        void
        close();

        /**
         * Writes 'frames' data to the stream.  The data is contained in
         * 'buffer'.
         */

        void
        write(const float *buffer, SampleFrameCount frames);

    private:

        void
        initialize(SampleRate sampleRate, SampleChannelCount channels,
                   int format);

        bool framesWritten;
        Sample &sample;

    };

}

#endif
