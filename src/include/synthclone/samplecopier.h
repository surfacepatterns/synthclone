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

#ifndef __SYNTHCLONE_SAMPLECOPIER_H__
#define __SYNTHCLONE_SAMPLECOPIER_H__

#include <synthclone/sampleinputstream.h>
#include <synthclone/sampleoutputstream.h>

namespace synthclone {

    /**
     * Utility class that copies Sample data from a SampleInputStream to a
     * SampleOutputStream, emitting SampleCopier::copyProgress events as it
     * goes along.
     */

    class SampleCopier: public QObject {

        Q_OBJECT

    public:

        /**
         * Constructs a new SampleCopier.
         *
         * @param parent
         *   The parent object of the new SampleCopier object.
         */

        explicit
        SampleCopier(QObject *parent=0);

        /**
         * Destroys the SampleCopier object.
         */

        ~SampleCopier();

        /**
         * Copies data from a SampleInputStream to a SampleOutputStream.
         *
         * @param inputStream
         *   The SampleInputStream.
         *
         * @param outputStream
         *   The SampleOutputStream.
         *
         * @param frames
         *   The maximum number of frames to copy.  If the end of the
         *   SampleInputStream is reached before this amount of frames is
         *   copied, then the return value will be different than this
         *   parameter.
         *
         * @returns
         *   The total number of frames copied.
         */

        SampleFrameCount
        copy(SampleInputStream &inputStream, SampleOutputStream &outputStream,
             SampleFrameCount frames);

    signals:

        /**
         * Emitted when frames are being copied.
         *
         * @param framesCopied
         *   The number of frames copied thus far.
         *
         * @param totalFrames
         *   The maximum amount of frames that will be copied.
         */

        void
        copyProgress(synthclone::SampleFrameCount framesCopied,
                     synthclone::SampleFrameCount totalFrames);

    private:

        float buffer[65536];

    };

}

#endif
