/*
 * synthclone - Synthesizer-cloning software
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
#include <cmath>
#include <limits>

#include <QtCore/QScopedArrayPointer>

#include <synthclone/sampleinputstream.h>

#include "sampleprofile.h"

static const float DBFS_MIN = -(std::numeric_limits<float>().max());

SampleProfile::SampleProfile(const synthclone::Sample &sample,
                             QObject *parent):
    QObject(parent)
{
    synthclone::SampleInputStream stream(sample);
    synthclone::SampleChannelCount channels = stream.getChannels();
    synthclone::SampleFrameCount frames = stream.getFrames();
    float positionDiff = frames / 1024.0;
    size_t bufferSize = static_cast<size_t>(ceil(positionDiff) * channels);
    float *buffer = new float[bufferSize];
    QScopedArrayPointer<float> bufferPtr(buffer);
    int i;
    float peak;
    synthclone::SampleFrameCount readFrameCount;
    if (frames >= 1024) {
        for (i = 0; i < 1024; i++) {
            float currentPosition = positionDiff * i;
            float nextPosition = positionDiff * (i + 1);
            readFrameCount =
                static_cast<synthclone::SampleFrameCount>(nextPosition) -
                static_cast<synthclone::SampleFrameCount>(currentPosition);
            assert(readFrameCount >= 1);
            synthclone::SampleFrameCount readFrames =
                stream.read(buffer, readFrameCount);
            assert(readFrames == readFrameCount);
            peak = 0.0;
            quint64 sampleCount = readFrames * channels;
            for (quint64 j = 0; j < sampleCount; j++) {
                float n = fabs(buffer[j]);
                if (n > peak) {
                    peak = n;
                }
            }
            peaks[i] = getDBFS(peak);
        }
    } else {
        for (i = 0; i < frames; i++) {
            readFrameCount = stream.read(buffer, 1);
            assert(readFrameCount == 1);
            peak = 0.0;
            for (synthclone::SampleChannelCount j = 0; j < channels; j++) {
                float n = fabs(buffer[j]);
                if (n > peak) {
                    peak = n;
                }
            }
            peaks[i] = getDBFS(peak);
        }
        for (; i < 1024; i++) {
            peaks[i] = DBFS_MIN;
        }
    }
    time = static_cast<float>(frames) / stream.getSampleRate();
}

SampleProfile::~SampleProfile()
{
    // Empty
}

float
SampleProfile::getDBFS(float sample) const
{
    return (sample == 0.0) ? DBFS_MIN : 20.0 * std::log10(std::fabs(sample));
}

const float *
SampleProfile::getPeaks() const
{
    return peaks;
}

float
SampleProfile::getTime() const
{
    return time;
}
