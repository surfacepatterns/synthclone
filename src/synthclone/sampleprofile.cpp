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

#include <QtCore/QScopedArrayPointer>

#include <synthclone/sampleinputstream.h>

#include "sampleprofile.h"

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
    if (frames >= 1024) {
        for (int i = 0; i < 1024; i++) {
            float currentPosition = positionDiff * i;
            float nextPosition = positionDiff * (i + 1);
            synthclone::SampleFrameCount readFrameCount =
                static_cast<synthclone::SampleFrameCount>(nextPosition) -
                static_cast<synthclone::SampleFrameCount>(currentPosition);
            assert(readFrameCount >= 1);
            synthclone::SampleFrameCount readFrames =
                stream.read(buffer, readFrameCount);
            assert(readFrames == readFrameCount);
            float maximum = -1.0;
            float minimum = 1.0;
            quint64 sampleCount = readFrames * channels;
            for (quint64 j = 0; j < sampleCount; j++) {
                float n = buffer[j];
                if (maximum < n) {
                    maximum = n;
                }
                if (minimum > n) {
                    minimum = n;
                }
            }
            highPeaks[i] = maximum;
            lowPeaks[i] = minimum;
        }
    } else {
        int i;
        for (i = 0; i < frames; i++) {
            synthclone::SampleFrameCount readFrameCount =
                stream.read(buffer, 1);
            assert(readFrameCount == 1);
            float maximum = -1.0;
            float minimum = 1.0;
            for (synthclone::SampleChannelCount j = 0; j < channels; j++) {
                float n = buffer[j];
                if (maximum < n) {
                    maximum = n;
                }
                if (minimum > n) {
                    minimum = n;
                }
            }
            highPeaks[i] = maximum;
            lowPeaks[i] = minimum;
        }
        for (; i < 1024; i++) {
            highPeaks[i] = 0.0;
            lowPeaks[i] = 0.0;
        }
    }
    time = static_cast<float>(frames) / stream.getSampleRate();
}

SampleProfile::~SampleProfile()
{
    // Empty
}

const float *
SampleProfile::getHighPeaks() const
{
    return highPeaks;
}

const float *
SampleProfile::getLowPeaks() const
{
    return lowPeaks;
}

float
SampleProfile::getTime() const
{
    return time;
}
