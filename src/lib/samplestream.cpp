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

#include <QtCore/QDebug>

#include <synthclone/error.h>
#include <synthclone/samplestream.h>

using synthclone::SampleStream;

SampleStream::SampleStream(QObject *parent):
    QObject(parent)
{
    closed = false;
}

SampleStream::~SampleStream()
{
    if (! closed) {
        try {
            close();
        } catch (synthclone::Error &e) {
            qWarning() << e.getMessage();
        }
    }
}

void
SampleStream::close()
{
    int result = sf_close(handle);
    if (result) {
        throw synthclone::Error(sf_error_number(result));
    }
    closed = true;
}

synthclone::SampleChannelCount
SampleStream::getChannels() const
{
    return static_cast<SampleChannelCount>(info.channels);
}

SampleStream::EndianType
SampleStream::getEndianType() const
{
    return static_cast<EndianType>(info.format & SF_FORMAT_ENDMASK);
}

synthclone::SampleFrameCount
SampleStream::getFrames() const
{
    return static_cast<SampleFrameCount>(info.frames);
}

synthclone::SampleRate
SampleStream::getSampleRate() const
{
    return static_cast<SampleRate>(info.samplerate);
}

SampleStream::SubType
SampleStream::getSubType() const
{
    return static_cast<SubType>(info.format & SF_FORMAT_SUBMASK);
}

SampleStream::Type
SampleStream::getType() const
{
    return static_cast<Type>(info.format & SF_FORMAT_TYPEMASK);
}

bool
SampleStream::isClosed() const
{
    return closed;
}

synthclone::SampleFrameCount
SampleStream::seek(SampleFrameCount frames, Offset offset)
{
    int whence;
    switch (offset) {
    case OFFSET_CURRENT:
        whence = SEEK_CUR;
        break;
    case OFFSET_END:
        whence = SEEK_END;
        break;
    case OFFSET_START:
    default:
        whence = SEEK_SET;
    }
    sf_count_t newLocation =
        sf_seek(handle, static_cast<sf_count_t>(frames), whence);
    if (newLocation == -1) {
        throw Error(sf_strerror(handle));
    }
    return static_cast<SampleFrameCount>(newLocation);
}
