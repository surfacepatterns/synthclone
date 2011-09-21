/*
 * libsynthclone_trimmer - Sample trimmer effect plugin for `synthclone`
 * Copyright (C) 2011 Devin Anderson
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA.
 */

#include <cassert>
#include <cfloat>
#include <cmath>

#include <QtCore/QScopedArrayPointer>

#include <synthclone/samplecopier.h>

#include "effect.h"

Effect::Effect(const QString &name, QObject *parent):
    synthclone::Effect(name, parent)
{
    sampleFloor = -70.0;
    trimEnd = true;
    trimStart = true;
}

Effect::~Effect()
{
    // Empty
}

float
Effect::getDBFS(float sample) const
{
    return (sample == 0.0) ? -FLT_MAX : 20.0 * std::log10(std::fabs(sample));
}

float
Effect::getSampleFloor() const
{
    return sampleFloor;
}

bool
Effect::getTrimEnd() const
{
    return trimEnd;
}

bool
Effect::getTrimStart() const
{
    return trimStart;
}

void
Effect::handleCopyProgress(synthclone::SampleFrameCount current,
                           synthclone::SampleFrameCount total)
{
    emit progressChanged(static_cast<float>(current) / total);
}

void
Effect::process(const synthclone::Zone &/*zone*/,
                synthclone::SampleInputStream &inputStream,
                synthclone::SampleOutputStream &outputStream)
{
    synthclone::SampleChannelCount channels = inputStream.getChannels();
    QScopedArrayPointer<float> audioDataPtr(new float[channels]);
    float *audioData = audioDataPtr.data();
    synthclone::SampleFrameCount frames = inputStream.getFrames();
    synthclone::SampleFrameCount framesRead;
    synthclone::SampleFrameCount end = frames - 1;
    synthclone::SampleFrameCount start = 0;
    if (trimStart) {
        emit statusChanged(tr("Trimming start of sample ..."));
        for (; start < end; start++) {
            framesRead = inputStream.read(audioData, 1);
            assert(framesRead == 1);
            for (int i = 0; i < channels; i++) {
                if (getDBFS(audioData[i]) >= sampleFloor) {
                    goto findEnd;
                }
            }
        }
    }
 findEnd:
    if (trimEnd) {
        emit statusChanged(tr("Trimming end of sample ..."));
        for (; end >= start; end--) {
            inputStream.seek(end, synthclone::SampleStream::OFFSET_START);
            framesRead = inputStream.read(audioData, 1);
            assert(framesRead == 1);
            for (int i = 0; i < channels; i++) {
                if (getDBFS(audioData[i]) >= sampleFloor) {
                    goto writeSample;
                }
            }
        }
    }
 writeSample:
    inputStream.seek(start, synthclone::SampleStream::OFFSET_START);
    synthclone::SampleFrameCount newFrameCount = (end - start) + 1;
    emit progressChanged(0.0);
    emit statusChanged(tr("Writing sample ..."));
    synthclone::SampleCopier copier;
    connect(&copier,
            SIGNAL(copyProgress(synthclone::SampleFrameCount,
                                synthclone::SampleFrameCount)),
            SLOT(handleCopyProgress(synthclone::SampleFrameCount,
                                    synthclone::SampleFrameCount)),
            Qt::DirectConnection);
    copier.copy(inputStream, outputStream, newFrameCount);
    emit progressChanged(0.0);
    emit statusChanged("");
}

void
Effect::setSampleFloor(float sampleFloor)
{
    assert(sampleFloor <= 0.0);
    if (this->sampleFloor != sampleFloor) {
        this->sampleFloor = sampleFloor;
        emit sampleFloorChanged(sampleFloor);
    }
}

void
Effect::setTrimEnd(bool trimEnd)
{
    if (this->trimEnd != trimEnd) {
        this->trimEnd = trimEnd;
        emit trimEndChanged(trimEnd);
    }
}

void
Effect::setTrimStart(bool trimStart)
{
    if (this->trimStart != trimStart) {
        this->trimStart = trimStart;
        emit trimStartChanged(trimStart);
    }
}
