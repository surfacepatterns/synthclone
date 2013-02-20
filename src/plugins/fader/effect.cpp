/*
 * libsynthclone_fader - Fader effect plugin for `synthclone`
 * Copyright (C) 2013 Devin Anderson
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

#include <QtCore/QDebug>

#include <QtCore/QScopedArrayPointer>

#include <synthclone/samplecopier.h>

#include "effect.h"

Effect::Effect(const QString &name, QObject *parent):
    synthclone::Effect(name, parent)
{
    fadeInEnabled = true;
    fadeInStartVolume = -64.0;
    fadeInTime = 0.01;
    fadeOutEnabled = true;
    fadeOutEndVolume = -64.0;
    fadeOutTime = 0.01;
}

Effect::~Effect()
{
    // Empty
}

float
Effect::getAmplitude(float dBFS) const
{
    assert(dBFS <= 0.0);
    return std::pow(10, dBFS / 20.0);
}

float
Effect::getFadeInStartVolume() const
{
    return fadeInStartVolume;
}

float
Effect::getFadeInTime() const
{
    return fadeInTime;
}

float
Effect::getFadeOutEndVolume() const
{
    return fadeOutEndVolume;
}

float
Effect::getFadeOutTime() const
{
    return fadeOutTime;
}

void
Effect::handleCopyProgress(synthclone::SampleFrameCount current,
                           synthclone::SampleFrameCount /*total*/)
{
    emit progressChanged(static_cast<float>(copyStartFrame + current) /
                         static_cast<float>(copyTotalFrames));
}


bool
Effect::isFadeInEnabled() const
{
    return fadeInEnabled;
}

bool
Effect::isFadeOutEnabled() const
{
    return fadeOutEnabled;
}

void
Effect::process(const synthclone::Zone &/*zone*/,
                synthclone::SampleInputStream &inputStream,
                synthclone::SampleOutputStream &outputStream)
{
    synthclone::SampleFrameCount frames = inputStream.getFrames();
    float sampleRate = static_cast<float>(inputStream.getSampleRate());
    synthclone::SampleFrameCount fadeInFrames = fadeInEnabled ?
        static_cast<synthclone::SampleFrameCount>(fadeInTime * sampleRate) : 0;
    synthclone::SampleFrameCount fadeOutFrames = fadeOutEnabled ?
        static_cast<synthclone::SampleFrameCount>(fadeOutTime * sampleRate) : 0;
    synthclone::SampleFrameCount totalFadeFrames = fadeInFrames + fadeOutFrames;

    qDebug() << "\tfade in frames:" << fadeInFrames;
    qDebug() << "\t fade out frames:" << fadeOutFrames;

    // If the amount of frames spent fading is greater than the number of total
    // frames, then shorten the fades proportionally.  If anyone has a better
    // suggestion, I'm all ears.
    if (totalFadeFrames > frames) {

        qDebug() << "adjusting fade frames";

        fadeInFrames = static_cast<synthclone::SampleFrameCount>
            (static_cast<float>(fadeInFrames) *
             (static_cast<float>(frames) /
              static_cast<float>(totalFadeFrames)));
        fadeOutFrames = frames - fadeInFrames;

        qDebug() << "\tfade in frames:" << fadeInFrames;
        qDebug() << "\t fade out frames:" << fadeOutFrames;

    }

    synthclone::SampleChannelCount channels = inputStream.getChannels();
    QScopedArrayPointer<float> audioDataPtr(new float[channels]);
    float *audioData = audioDataPtr.data();
    synthclone::SampleFrameCount currentFrame = 0;
    synthclone::SampleFrameCount framesRead;
    float volume;
    if (fadeInFrames) {

        qDebug() << "\tapplying fade in ...";

        emit statusChanged(tr("Creating fade-in of sample ..."));
        for (; currentFrame < fadeInFrames; currentFrame++) {
            framesRead = inputStream.read(audioData, 1);
            assert(framesRead == 1);
            volume = getAmplitude(fadeInStartVolume *
                                  (1.0 - (static_cast<float>(currentFrame + 1) /
                                          static_cast<float>(fadeInFrames))));
            for (int i = 0; i < channels; i++) {
                audioData[i] *= volume;
            }
            outputStream.write(audioData, 1);
            emit progressChanged(static_cast<float>(currentFrame + 1) /
                                 static_cast<float>(frames));
        }
    }
    synthclone::SampleFrameCount fadeOutStartFrame = frames - fadeOutFrames;
    synthclone::SampleFrameCount copyFrames = fadeOutStartFrame - currentFrame;

    qDebug() << "\tcopy frames:" << copyFrames;
    qDebug() << "\tfade out start frame:" << fadeOutStartFrame;

    if (copyFrames) {

        qDebug() << "copying frames ...";

        emit statusChanged(tr("Writing sample ..."));
        copyStartFrame = currentFrame;
        copyTotalFrames = frames;
        synthclone::SampleCopier copier;
        connect(&copier,
                SIGNAL(copyProgress(synthclone::SampleFrameCount,
                                    synthclone::SampleFrameCount)),
                SLOT(handleCopyProgress(synthclone::SampleFrameCount,
                                        synthclone::SampleFrameCount)),
                Qt::DirectConnection);
        copier.copy(inputStream, outputStream, copyFrames);
    }
    currentFrame += copyFrames;
    if (fadeOutFrames) {

        qDebug() << "\tapplying fade out ...";

        emit statusChanged(tr("Creating fade-out of sample ..."));
        for (; currentFrame < frames; currentFrame++) {
            framesRead = inputStream.read(audioData, 1);
            assert(framesRead == 1);
            volume = getAmplitude(fadeOutEndVolume *
                                  (static_cast<float>(currentFrame + 1 -
                                                      fadeOutStartFrame) /
                                   static_cast<float>(fadeOutFrames)));
            for (int i = 0; i < channels; i++) {
                audioData[i] *= volume;
            }
            outputStream.write(audioData, 1);
            emit progressChanged(static_cast<float>(currentFrame + 1) /
                                 static_cast<float>(frames));
        }
    }
    emit progressChanged(0.0);
    emit statusChanged("");

    qDebug() << "/Effect::process";

}

void
Effect::setFadeInEnabled(bool enabled)
{
    if (enabled != fadeInEnabled) {
        fadeInEnabled = enabled;
        emit fadeInEnabledChanged(enabled);
    }
}

void
Effect::setFadeInStartVolume(float volume)
{
    assert(volume <= 0.0);
    if (volume != fadeInStartVolume) {
        fadeInStartVolume = volume;
        emit fadeInStartVolumeChanged(volume);
    }
}

void
Effect::setFadeInTime(float time)
{
    assert(time > 0.0);
    if (time != fadeInTime) {
        fadeInTime = time;
        emit fadeInTimeChanged(time);
    }
}

void
Effect::setFadeOutEnabled(bool enabled)
{
    if (enabled != fadeOutEnabled) {
        fadeOutEnabled = enabled;
        emit fadeOutEnabledChanged(enabled);
    }
}

void
Effect::setFadeOutEndVolume(float volume)
{
    assert(volume <= 0.0);
    if (volume != fadeOutEndVolume) {
        fadeOutEndVolume = volume;
        emit fadeOutEndVolumeChanged(volume);
    }
}

void
Effect::setFadeOutTime(float time)
{
    assert(time > 0.0);
    if (time != fadeOutTime) {
        fadeOutTime = time;
        emit fadeOutTimeChanged(time);
    }
}
