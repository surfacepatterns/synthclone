/*
 * libsynthclone_reverser - `synthclone` effect plugin that renders a sample
 * stream in reverse
 * Copyright (C) 2012 Devin Anderson
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

#include <QtCore/QScopedArrayPointer>

#include "effect.h"

Effect::Effect(const QString &name, QObject *parent):
    synthclone::Effect(name, parent)
{
    // Empty
}

Effect::~Effect()
{
    // Empty
}

void
Effect::process(const synthclone::Zone &/*zone*/,
                synthclone::SampleInputStream &inputStream,
                synthclone::SampleOutputStream &outputStream)
{
    QScopedArrayPointer<float> dataPtr(new float[inputStream.getChannels()]);
    float *data = dataPtr.data();
    synthclone::SampleFrameCount totalFrames = inputStream.getFrames();
    emit progressChanged(0.0);
    emit statusChanged(tr("Reversing sample ..."));
    for (synthclone::SampleFrameCount i = totalFrames - 1; i >= 0; i--) {
        inputStream.seek(i, synthclone::SampleStream::OFFSET_START);
        synthclone::SampleFrameCount n = inputStream.read(data, 1);
        assert(n == 1);
        outputStream.write(data, 1);
        emit progressChanged(static_cast<float>(totalFrames - i) /
                             static_cast<float>(totalFrames));
    }
    emit progressChanged(0.0);
    emit statusChanged("");
}
