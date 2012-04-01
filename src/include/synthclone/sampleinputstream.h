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

#ifndef __SYNTHCLONE_SAMPLEINPUTSTREAM_H__
#define __SYNTHCLONE_SAMPLEINPUTSTREAM_H__

#include <synthclone/sample.h>
#include <synthclone/samplestream.h>

namespace synthclone {

    /**
     * Used to read sample data from a sample file.  The object uses the
     * excellent libsndfile in its implementation, which can be found at
     * http://www.mega-nerd.com/libsndfile/.
     */

    class SampleInputStream: public SampleStream {

        Q_OBJECT

    public:

        /**
         * Constructs a new sample input stream.
         *
         * @param sample
         *   The sample to read data from.
         *
         * @param parent
         *   The parent object of the new stream object.
         */

        explicit
        SampleInputStream(const Sample &sample, QObject *parent=0);

        /**
         * Destroys the stream.
         */

        ~SampleInputStream();

        /**
         * Reads data from the stream.
         *
         * @param buffer
         *   A buffer to read data into.  The buffer's size should be greater
         *   than or equal to:
         *   @code frames * channels * sizeof(float) @endcode
         *
         * @param frames
         *   The maximum number of frames to read into the buffer.  If the
         *   stream is positioned less than the amount of frames from the end
         *   of the sample file, then less data will be read into the buffer.
         *
         * @returns
         *   The number of frames read into the buffer.
         */

        SampleFrameCount
        read(float *buffer, SampleFrameCount frames);

    };

}

#endif
