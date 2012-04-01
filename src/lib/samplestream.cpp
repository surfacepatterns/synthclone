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

#include <synthclone/samplestream.h>

#include "samplefile.h"

using synthclone::SampleStream;

SampleStream::SampleStream(QObject *parent):
    QObject(parent)
{
    // Empty
}

SampleStream::~SampleStream()
{
    // Empty
}

void
SampleStream::close()
{
    file->close();
}

synthclone::SampleChannelCount
SampleStream::getChannels() const
{
    return file->getChannels();
}

SampleStream::EndianType
SampleStream::getEndianType() const
{
    return file->getEndianType();
}

synthclone::SampleFrameCount
SampleStream::getFrames() const
{
    return file->getFrames();
}

synthclone::SampleRate
SampleStream::getSampleRate() const
{
    return file->getSampleRate();
}

SampleStream::SubType
SampleStream::getSubType() const
{
    return file->getSubType();
}

SampleStream::Type
SampleStream::getType() const
{
    return file->getType();
}

bool
SampleStream::isClosed() const
{
    return file->isClosed();
}

synthclone::SampleFrameCount
SampleStream::seek(SampleFrameCount frames, Offset offset)
{
    return file->seek(frames, offset);
}
