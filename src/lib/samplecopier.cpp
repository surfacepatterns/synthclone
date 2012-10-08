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

#include <cassert>

#include <synthclone/samplecopier.h>
#include <synthclone/util.h>

using synthclone::SampleCopier;

SampleCopier::SampleCopier(QObject *parent):
    QObject(parent)
{
    for (int i = 0; i < 65536; i++) {
        buffer[i] = 0.0;
    }
}

SampleCopier::~SampleCopier()
{
    // Empty
}

synthclone::SampleFrameCount
SampleCopier::copy(SampleInputStream &inputStream,
                   SampleOutputStream &outputStream, SampleFrameCount frames)
{
    SampleChannelCount channels = inputStream.getChannels();

    CONFIRM(channels == outputStream.getChannels(),
            qApp->tr("the channel counts of the streams are not equal"));
    CONFIRM(inputStream.getSampleRate() == outputStream.getSampleRate(),
            qApp->tr("the sample rates of the streams are not equal"));
    CONFIRM(frames >= 0, tr("'%1': invalid frame count").arg(frames));

    SampleFrameCount framesRead;
    SampleFrameCount readSize = static_cast<SampleFrameCount>(65536 / channels);
    assert(readSize >= 1);
    SampleFrameCount framesLeft = frames;
    SampleFrameCount totalFramesProcessed = 0;
    for (; framesLeft > readSize; framesLeft -= readSize) {
        framesRead = inputStream.read(buffer, readSize);
        if (! framesRead) {
            return totalFramesProcessed;
        }
        outputStream.write(buffer, framesRead);
        totalFramesProcessed += framesRead;
        emit copyProgress(totalFramesProcessed, frames);
        if (framesRead != readSize) {
            return totalFramesProcessed;
        }
    }
    if (framesLeft) {
        framesRead = inputStream.read(buffer, framesLeft);
        if (framesRead) {
            outputStream.write(buffer, framesRead);
            totalFramesProcessed += framesRead;
            emit copyProgress(totalFramesProcessed, frames);
        }
    }
    return totalFramesProcessed;
}
