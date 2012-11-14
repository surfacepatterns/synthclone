/*
 * libsynthclone_lv2 - LV2 effect plugin for `synthclone`
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

#ifndef __EFFECT_H__
#define __EFFECT_H__

#include <synthclone/effect.h>

#include "lv2world.h"

class Effect: public synthclone::Effect {

    Q_OBJECT

public:

    Effect(const LV2Plugin &plugin, LV2World &world,
           synthclone::SampleRate sampleRate,
           synthclone::SampleChannelCount channels, QObject *parent=0);

    ~Effect();

    int
    getAudioInputChannel(synthclone::SampleChannelCount channel) const;

    int
    getAudioInputPortCount() const;

    QString
    getAudioInputPortName(int port) const;

    int
    getAudioOutputChannel(synthclone::SampleChannelCount channel) const;

    int
    getAudioOutputPortCount() const;

    QString
    getAudioOutputPortName(int port) const;

    int
    getControlInputPortCount() const;

    uint32_t
    getControlInputPortIndex(int index) const;

    QString
    getControlInputPortSymbol(int index) const;

    float
    getControlInputPortValue(int index) const;

    int
    getControlOutputPortCount() const;

    uint32_t
    getControlOutputPortIndex(int index) const;

    QString
    getControlOutputPortSymbol(int index) const;

    float
    getControlOutputPortValue(int index) const;

    int
    getInstanceCount() const;

    const LV2Plugin &
    getPlugin() const;

    QByteArray
    getState() const;

    void
    process(const synthclone::Zone &zone,
            synthclone::SampleInputStream &inputStream,
            synthclone::SampleOutputStream &outputStream);

public slots:

    void
    setAudioInputChannel(synthclone::SampleChannelCount channel,
                         int inputChannel);

    void
    setAudioOutputChannel(synthclone::SampleChannelCount channel,
                          int outputChannel);

    void
    setChannelCount(synthclone::SampleChannelCount channels);

    void
    setControlInputPortValue(int index, float value);

    void
    setControlOutputPortValue(int index, float value);

    void
    setInstanceCount(int count);

    void
    setSampleRate(synthclone::SampleRate sampleRate);

    void
    setState(const QByteArray &stateBytes);

signals:

    void
    audioInputChannelChanged(synthclone::SampleChannelCount channel,
                             int inputChannel);

    void
    audioInputPortAdded(const QString &name);

    void
    audioInputPortRemoved();

    void
    audioOutputChannelChanged(synthclone::SampleChannelCount channel,
                              int outputChannel);

    void
    audioOutputPortAdded(const QString &name);

    void
    audioOutputPortRemoved();

    void
    channelsChanged(synthclone::SampleChannelCount channels);

    void
    controlInputPortValueChanged(int index, float value);

    void
    controlOutputPortValueChanged(int index, float value);

    void
    instanceCountChanged(int count);

    void
    sampleRateChanged(synthclone::SampleRate sampleRate);

private:

    static const void *
    getPortValue(const char *symbol, void *effect, uint32_t *size,
                 uint32_t *type);

    static void
    setPortValue(const char *symbol, void *effect, const void *value,
                 uint32_t size, uint32_t type);

    void
    addInstance();

    float
    getDefaultControlPortValue(const LV2Port &port) const;

    const void *
    getPortValue(const char *symbol, uint32_t *size, uint32_t *type);

    void
    removeInstance();

    void
    runInstances(synthclone::SampleInputStream &inputStream,
                 synthclone::SampleOutputStream &outputStream,
                 float *sampleStreamData, synthclone::SampleFrameCount frames);

    void
    setPortValue(const char *symbol, const void *value, uint32_t size,
                 uint32_t type);

    int *audioInputChannelIndices;
    QList<float *> audioInputPortBuffers;
    int *audioOutputChannelIndices;
    QList<float *> audioOutputPortBuffers;
    synthclone::SampleChannelCount channels;
    float *controlInputPortValues;
    float *controlOutputPortValues;
    QList<LV2Instance *> instances;
    const LV2Plugin &plugin;
    synthclone::SampleRate sampleRate;
    LV2World &world;

};

#endif
