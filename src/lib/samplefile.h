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

#ifndef __SYNTHCLONE_SAMPLEFILE_H__
#define __SYNTHCLONE_SAMPLEFILE_H__

#include <sndfile.h>

#include <synthclone/samplestream.h>

namespace synthclone {

    class SampleFile: public QObject {

        Q_OBJECT

    public:

        explicit
        SampleFile(const QString &path, QObject *parent=0);

        SampleFile(const QString &path, SampleRate sampleRate,
                   SampleChannelCount channels, QObject *parent=0);

        SampleFile(const QString &path, SampleRate sampleRate,
                   SampleChannelCount channels, SampleStream::Type type,
                   SampleStream::SubType subType,
                   SampleStream::EndianType endianType, QObject *parent=0);

        ~SampleFile();

        virtual void
        close();

        SampleChannelCount
        getChannels() const;

        SampleStream::EndianType
        getEndianType() const;

        SampleFrameCount
        getFrames() const;

        SampleRate
        getSampleRate() const;

        SampleStream::SubType
        getSubType() const;

        SampleStream::Type
        getType() const;

        bool
        isClosed() const;

        SampleFrameCount
        read(float *buffer, SampleFrameCount frames);

        SampleFrameCount
        seek(SampleFrameCount frames, SampleStream::Offset offset);

        void
        write(const float *buffer, SampleFrameCount frames);

    private:

        void
        initializeWriteMode(const QString &path, SampleRate sampleRate,
                            SampleChannelCount channels,
                            SampleStream::Type type,
                            SampleStream::SubType subType,
                            SampleStream::EndianType endianType);

        bool closed;
        bool framesWritten;
        SNDFILE *handle;
        SF_INFO info;
        QString path;
        bool writeMode;

    };

}

#endif
