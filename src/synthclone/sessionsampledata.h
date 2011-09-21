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

#ifndef __SESSIONSAMPLEDATA_H__
#define __SESSIONSAMPLEDATA_H__

#include <QtCore/QDir>

#include <synthclone/sample.h>
#include <synthclone/types.h>

class SessionSampleData: public QObject {

    Q_OBJECT

public:

    explicit
    SessionSampleData(QObject *parent=0);

    ~SessionSampleData();

    synthclone::SampleChannelCount
    getSampleChannelCount() const;

    const QDir *
    getSampleDirectory() const;

    synthclone::SampleRate
    getSampleRate() const;

public slots:

    void
    setSampleChannelCount(synthclone::SampleChannelCount count);

    void
    setSampleDirectory(const QDir *directory);

    void
    setSampleRate(synthclone::SampleRate sampleRate);

    synthclone::Sample *
    updateSample(synthclone::Sample &sample, QObject *parent=0);

signals:

    void
    sampleChannelCountChanged(synthclone::SampleChannelCount count);

    void
    sampleDirectoryChanged(const QDir *directory);

    void
    sampleRateChanged(synthclone::SampleRate sampleRate);

private:

    enum ChannelConvertAlgorithm {
        CHANNELCONVERTALGORITHM_NONE,
        CHANNELCONVERTALGORITHM_FROM_MONO,
        CHANNELCONVERTALGORITHM_TO_MONO,
    };

    synthclone::SampleChannelCount sampleChannelCount;
    QDir *sampleDirectory;
    synthclone::SampleRate sampleRate;

};

#endif
