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

#include <cassert>

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>

#include <QtCore/QDebug>

#include "effect.h"

// Static functions

const void *
Effect::getPortValue(const char *symbol, void *effect, uint32_t *size,
                     uint32_t *type)
{
    return static_cast<Effect *>(effect)->getPortValue(symbol, size, type);
}

void
Effect::setPortValue(const char *symbol, void *effect, const void *value,
                     uint32_t size, uint32_t type)
{
    static_cast<Effect *>(effect)->setPortValue(symbol, value, size, type);
}

// Class definition

Effect::Effect(const LV2Plugin &plugin, LV2World &world,
               synthclone::SampleRate sampleRate,
               synthclone::SampleChannelCount channels, QObject *parent):
    synthclone::Effect(plugin.getName(), parent),
    plugin(plugin),
    world(world)
{
    audioInputChannelIndices = new int[channels];
    audioOutputChannelIndices = new int[channels];
    int audioInputPortCount = plugin.getAudioInputPortCount();
    int audioOutputPortCount = plugin.getAudioOutputPortCount();
    for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
        audioInputChannelIndices[i] = i % audioInputPortCount;
        audioOutputChannelIndices[i] = i % audioOutputPortCount;
    }
    this->channels = channels;
    int count = plugin.getControlInputPortCount();
    if (count) {
        controlInputPortValues = new float[count];
        for (int i = 0; i < count; i++) {
            const LV2Port &port = plugin.getControlInputPort(i);
            controlInputPortValues[i] = getDefaultControlPortValue(port);
        }
    } else {
        controlInputPortValues = 0;
    }
    count = plugin.getControlOutputPortCount();
    if (count) {
        controlOutputPortValues = new float[count];
        for (int i = 0; i < count; i++) {
            const LV2Port &port = plugin.getControlOutputPort(i);
            controlOutputPortValues[i] = getDefaultControlPortValue(port);
        }
    } else {
        controlOutputPortValues = 0;
    }
    this->sampleRate = sampleRate;
    addInstance();
}

Effect::~Effect()
{
    for (int i = instances.count() - 1; i >= 0; i--) {
        removeInstance();
    }
    delete[] audioInputChannelIndices;
    delete[] audioOutputChannelIndices;
    if (controlInputPortValues) {
        delete[] controlInputPortValues;
    }
    if (controlOutputPortValues) {
        delete[] controlOutputPortValues;
    }
}

void
Effect::addInstance()
{
    LV2Instance *instance = plugin.instantiate(static_cast<double>(sampleRate));
    int instanceCount = instances.count();
    if (instanceCount) {
        LV2State *state = instances[0]->getState();
        QScopedPointer<LV2State> statePtr(state);
        instance->setState(state);
    }
    instances.append(instance);
    emit instanceCountChanged(instanceCount + 1);
    float *audioBuffer;
    int i;
    int portIndex;
    int audioInputPortCount = plugin.getAudioInputPortCount();
    for (i = 0; i < audioInputPortCount; i++) {
        portIndex = (instanceCount * audioInputPortCount) + i;
        audioBuffer = new float[65536];
        instance->connectPort(plugin.getAudioInputPort(i).getIndex(),
                              audioBuffer);
        audioInputPortBuffers.append(audioBuffer);
        emit audioInputPortAdded(getAudioInputPortName(portIndex));
    }
    int audioOutputPortCount = plugin.getAudioOutputPortCount();
    for (i = 0; i < audioOutputPortCount; i++) {
        portIndex = (instanceCount * audioOutputPortCount) + i;
        audioBuffer = new float[65536];
        instance->connectPort(plugin.getAudioOutputPort(i).getIndex(),
                              audioBuffer);
        audioOutputPortBuffers.append(audioBuffer);
        emit audioOutputPortAdded(getAudioOutputPortName(portIndex));
    }
    for (i = plugin.getControlInputPortCount() - 1; i >= 0; i--) {
        instance->connectPort(plugin.getControlInputPort(i).getIndex(),
                              controlInputPortValues + i);
    }
    for (i = plugin.getControlOutputPortCount() - 1; i >= 0; i--) {
        instance->connectPort(plugin.getControlOutputPort(i).getIndex(),
                              controlOutputPortValues + i);
    }
}

int
Effect::getAudioInputChannel(synthclone::SampleChannelCount channel) const
{
    assert(channel < channels);
    return audioInputChannelIndices[channel];
}

int
Effect::getAudioInputPortCount() const
{
    return plugin.getAudioInputPortCount() * instances.count();
}

QString
Effect::getAudioInputPortName(int port) const
{
    int count = plugin.getAudioInputPortCount();
    assert((port >= 0) && (port < (count * instances.count())));
    return tr("%1 - %2").arg((port / count) + 1).
        arg(plugin.getAudioInputPort(port % count).getName());
}

int
Effect::getAudioOutputChannel(synthclone::SampleChannelCount channel) const
{
    assert(channel < channels);
    return audioOutputChannelIndices[channel];
}

int
Effect::getAudioOutputPortCount() const
{
    return plugin.getAudioOutputPortCount() * instances.count();
}

QString
Effect::getAudioOutputPortName(int port) const
{
    int count = plugin.getAudioOutputPortCount();
    assert((port >= 0) && (port < (count * instances.count())));
    return tr("%1 - %2").arg((port / count) + 1).
        arg(plugin.getAudioOutputPort(port % count).getName());
}

int
Effect::getControlInputPortCount() const
{
    return plugin.getControlInputPortCount();
}

uint32_t
Effect::getControlInputPortIndex(int index) const
{
    assert((index >= 0) && (index < plugin.getControlInputPortCount()));
    return plugin.getControlInputPort(index).getIndex();
}

QString
Effect::getControlInputPortSymbol(int index) const
{
    assert((index >= 0) && (index < plugin.getControlInputPortCount()));
    return plugin.getControlInputPort(index).getSymbol();
}

float
Effect::getControlInputPortValue(int index) const
{
    assert((index >= 0) && (index < plugin.getControlInputPortCount()));
    return controlInputPortValues[index];
}

int
Effect::getControlOutputPortCount() const
{
    return plugin.getControlOutputPortCount();
}

uint32_t
Effect::getControlOutputPortIndex(int index) const
{
    assert((index >= 0) && (index < plugin.getControlOutputPortCount()));
    return plugin.getControlOutputPort(index).getIndex();
}

QString
Effect::getControlOutputPortSymbol(int index) const
{
    assert((index >= 0) && (index < plugin.getControlOutputPortCount()));
    return plugin.getControlOutputPort(index).getSymbol();
}

float
Effect::getControlOutputPortValue(int index) const
{
    assert((index >= 0) && (index < plugin.getControlOutputPortCount()));
    return controlOutputPortValues[index];
}

float
Effect::getDefaultControlPortValue(const LV2Port &port) const
{
    float result;
    bool success;
    QVariant value = port.getDefaultValue();
    if (value.isValid()) {
        result = value.toFloat(&success);
        if (success) {
            return result;
        }
    }
    value = port.getMinimumValue();
    if (value.isValid()) {
        result = value.toFloat(&success);
        if (success) {
            return result;
        }
    }
    value = port.getMaximumValue();
    if (value.isValid()) {
        result = value.toFloat(&success);
        if (success) {
            return result;
        }
    }
    return 0.0;
}

int
Effect::getInstanceCount() const
{
    return instances.count();
}

const LV2Plugin &
Effect::getPlugin() const
{
    return plugin;
}

const void *
Effect::getPortValue(const char *symbol, uint32_t *size, uint32_t *type)
{
    const void *result;

    // Check input ports
    QString portSymbol(symbol);
    for (int i = getControlInputPortCount() - 1; i >= 0; i--) {
        if (portSymbol == getControlInputPortSymbol(i)) {
            result = controlInputPortValues + i;
            goto foundValue;
        }
    }

    // Check output ports
    for (int i = getControlOutputPortCount() - 1; i >= 0; i--) {
        if (portSymbol == getControlOutputPortSymbol(i)) {
            result = controlOutputPortValues + i;
            goto foundValue;
        }
    }

    // Uh oh ...
    qWarning() << tr("Effect::getPortValue - symbol '%1' is not associated "
                     "with a control port").arg(portSymbol);
    *size = 0;
    return 0;

 foundValue:
    *size = sizeof(float);
    *type = world.getURIMap().getId(LV2_ATOM__Float);
    return result;
}

QByteArray
Effect::getState() const
{
    LV2State *state = instances[0]->getState(Effect::getPortValue,
                                             const_cast<Effect *>(this));
    QScopedPointer<LV2State> statePtr(state);
    return state->getBytes();
}

void
Effect::process(const synthclone::Zone &/*zone*/,
                synthclone::SampleInputStream &inputStream,
                synthclone::SampleOutputStream &outputStream)
{
    assert(inputStream.getChannels() == channels);
    assert(inputStream.getSampleRate() == sampleRate);
    assert(outputStream.getChannels() == channels);
    assert(outputStream.getSampleRate() == sampleRate);

    emit statusChanged(tr("Initializing audio buffers ..."));
    float *sampleStreamData = new float[65536 * channels];
    QScopedArrayPointer<float> sampleStreamDataPtr(sampleStreamData);

    int instanceCount = instances.count();
    emit statusChanged(tr("Activating LV2 instance(s) ..."));
    for (int i = 0; i < instanceCount; i++) {
        instances[i]->activate();
    }

    emit statusChanged(tr("Processing audio data ..."));
    synthclone::SampleFrameCount totalFrames = inputStream.getFrames();
    if (totalFrames) {
        synthclone::SampleFrameCount framesProcessed = 0;
        for (; (totalFrames - framesProcessed) > 65536;
             framesProcessed += 65536) {
            emit progressChanged(static_cast<float>(framesProcessed) /
                                 static_cast<float>(totalFrames));
            runInstances(inputStream, outputStream, sampleStreamData, 65536);
        }
        assert(framesProcessed != totalFrames);
        emit progressChanged(static_cast<float>(framesProcessed) /
                             static_cast<float>(totalFrames));
        runInstances(inputStream, outputStream, sampleStreamData,
                     totalFrames - framesProcessed);
    }
    emit progressChanged(1.0);

    emit statusChanged(tr("Deactivating LV2 instances ..."));
    for (int i = 0; i < instanceCount; i++) {
        instances[i]->deactivate();
    }

    emit progressChanged(0.0);
    emit statusChanged("");
}

void
Effect::removeInstance()
{
    int i;
    int instanceCount = instances.count() - 1;
    LV2Instance *instance = instances[instanceCount];
    int j;
    int portIndex;
    int audioInputPortCount = plugin.getAudioInputPortCount();
    for (i = audioInputPortCount - 1; i >= 0; i--) {
        portIndex = (instanceCount * audioInputPortCount) + i;
        for (j = 0; j < channels; j++) {
            if (audioInputChannelIndices[j] == portIndex) {
                audioInputChannelIndices[j] = 0;
                emit audioInputChannelChanged(j, 0);
            }
        }
        instance->connectPort(plugin.getAudioInputPort(i).getIndex(), 0);
        delete[] audioInputPortBuffers.takeLast();
        emit audioInputPortRemoved();
    }
    int audioOutputPortCount = plugin.getAudioOutputPortCount();
    for (i = audioOutputPortCount - 1; i >= 0; i--) {
        portIndex = (instanceCount * audioOutputPortCount) + i;
        for (j = 0; j < channels; j++) {
            if (audioOutputChannelIndices[j] == portIndex) {
                audioOutputChannelIndices[j] = 0;
                emit audioOutputChannelChanged(j, 0);
            }
        }
        instance->connectPort(plugin.getAudioOutputPort(i).getIndex(), 0);
        delete[] audioOutputPortBuffers.takeLast();
        emit audioOutputPortRemoved();
    }
    for (i = plugin.getControlInputPortCount() - 1; i >= 0; i--) {
        instance->connectPort(plugin.getControlInputPort(i).getIndex(), 0);
    }
    for (i = plugin.getControlOutputPortCount() - 1; i >= 0; i--) {
        instance->connectPort(plugin.getControlOutputPort(i).getIndex(), 0);
    }
    delete instances.takeAt(instanceCount);
    emit instanceCountChanged(instanceCount);
}

void
Effect::runInstances(synthclone::SampleInputStream &inputStream,
                     synthclone::SampleOutputStream &outputStream,
                     float *sampleStreamData,
                     synthclone::SampleFrameCount frames)
{
    // Get data from input stream
    synthclone::SampleFrameCount framesRead =
        inputStream.read(sampleStreamData, frames);
    assert(framesRead == frames);

    // Prepare instance input data
    float *audioBuffer;
    for (int i = audioInputPortBuffers.count() - 1; i >= 0; i--) {
        audioBuffer = audioInputPortBuffers[i];
        for (int j = 0; j < 65536; j++) {
            audioBuffer[j] = 0.0;
        }
    }
    for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
        audioBuffer = audioInputPortBuffers[audioInputChannelIndices[i]];
        for (synthclone::SampleFrameCount j = 0; j < frames; j++) {
            audioBuffer[j] += sampleStreamData[(j * channels) + i];
        }
    }

    // Run instances
    for (int i = instances.count() - 1; i >= 0; i--) {
        instances[i]->run(frames);
    }

    // Prepare stream output data
    for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
        audioBuffer = audioOutputPortBuffers[audioOutputChannelIndices[i]];
        for (synthclone::SampleFrameCount j = 0; j < frames; j++) {
            sampleStreamData[(j * channels) + i] = audioBuffer[j];
        }
    }

    // Write output data
    outputStream.write(sampleStreamData, frames);
}

void
Effect::setAudioInputChannel(synthclone::SampleChannelCount channel,
                             int inputChannel)
{
    assert(channel < channels);
    assert((inputChannel >= 0) &&
           (inputChannel < (plugin.getAudioInputPortCount() *
                            instances.count())));
    if (audioInputChannelIndices[channel] != inputChannel) {
        audioInputChannelIndices[channel] = inputChannel;
        emit audioInputChannelChanged(channel, inputChannel);
    }
}

void
Effect::setAudioOutputChannel(synthclone::SampleChannelCount channel,
                              int outputChannel)
{
    assert(channel < channels);
    assert((outputChannel >= 0) &&
           (outputChannel < (plugin.getAudioOutputPortCount() *
                             instances.count())));
    if (audioOutputChannelIndices[channel] != outputChannel) {
        audioOutputChannelIndices[channel] = outputChannel;
        emit audioOutputChannelChanged(channel, outputChannel);
    }
}

void
Effect::setChannelCount(synthclone::SampleChannelCount channels)
{
    synthclone::SampleChannelCount oldChannels = this->channels;
    if (channels != oldChannels) {
        int *audioInputChannels = new int[channels];
        int *audioOutputChannels = new int[channels];
        size_t copySize;
        if (channels < oldChannels) {
            copySize = channels * sizeof(synthclone::SampleChannelCount);
            memcpy(audioInputChannels, audioInputChannelIndices, copySize);
            memcpy(audioOutputChannels, audioOutputChannelIndices, copySize);
        } else {
            copySize = oldChannels * sizeof(synthclone::SampleChannelCount);
            memcpy(audioInputChannels, audioInputChannelIndices, copySize);
            memcpy(audioOutputChannels, audioOutputChannelIndices, copySize);
            int audioInputPortCount = getAudioInputPortCount();
            int audioOutputPortCount = getAudioOutputPortCount();
            for (synthclone::SampleChannelCount i = oldChannels; i < channels;
                 i++) {
                audioInputChannels[i] = i % audioInputPortCount;
                audioOutputChannels[i] = i % audioOutputPortCount;
            }
        }
        delete[] audioInputChannelIndices;
        delete[] audioOutputChannelIndices;
        audioInputChannelIndices = audioInputChannels;
        audioOutputChannelIndices = audioOutputChannels;
        this->channels = channels;
        emit channelsChanged(channels);
        for (synthclone::SampleChannelCount i = oldChannels; i < channels;
             i++) {
            emit audioInputChannelChanged(i, audioInputChannels[i]);
            emit audioOutputChannelChanged(i, audioOutputChannels[i]);
        }
    }
}

void
Effect::setControlInputPortValue(int index, float value)
{
    assert((index >= 0) && (index < plugin.getControlInputPortCount()));
    if (controlInputPortValues[index] != value) {
        controlInputPortValues[index] = value;
        emit controlInputPortValueChanged(index, value);
    }
}

void
Effect::setControlOutputPortValue(int index, float value)
{
    assert((index >= 0) && (index < plugin.getControlOutputPortCount()));
    if (controlOutputPortValues[index] != value) {
        controlOutputPortValues[index] = value;
        emit controlOutputPortValueChanged(index, value);
    }
}

void
Effect::setInstanceCount(int count)
{
    assert(count > 0);
    int oldCount = instances.count();
    if (count != oldCount) {
        if (count < oldCount) {
            for (int i = oldCount; i > count; i--) {
                removeInstance();
            }
        } else {
            for (int i = oldCount; i < count; i++) {
                addInstance();
            }
        }
    }
}

void
Effect::setPortValue(const char *symbol, const void *value, uint32_t size,
                     uint32_t type)
{
    // Make sure we can handle the request.
    const char *typeURI = world.getURIMap().getURI(type);
    if (! typeURI) {
        qWarning() << tr("Effect::setPortValue - unmapped type '%1' requested "
                         "for port '%2'").arg(type).arg(symbol);
        return;
    }
    if (strcmp(typeURI, LV2_ATOM__Float)) {
        qWarning() << tr("Effect::setPortValue - can't handle type '%1'").
            arg(typeURI);
        return;
    }
    if (size != sizeof(float)) {
        qWarning() << tr("Effect::setPortValue - type is float, size is %1").
            arg(size);
        return;
    }

    // Check input ports
    QString portSymbol(symbol);
    for (int i = getControlInputPortCount() - 1; i >= 0; i--) {
        if (portSymbol == getControlInputPortSymbol(i)) {
            setControlInputPortValue(i, *(static_cast<const float *>(value)));
            return;
        }
    }

    // Check output ports
    for (int i = getControlOutputPortCount() - 1; i >= 0; i--) {
        if (portSymbol == getControlOutputPortSymbol(i)) {
            setControlOutputPortValue(i, *(static_cast<const float *>(value)));
            return;
        }
    }

    qWarning() << tr("Effect::setPortValue - symbol '%1' is not associated "
                     "with a control port").arg(portSymbol);
}

void
Effect::setSampleRate(synthclone::SampleRate sampleRate)
{
    assert(sampleRate);
    if (sampleRate != this->sampleRate) {
        LV2State *state = instances[0]->getState();
        QScopedPointer<LV2State> statePtr(state);
        for (int i = instances.count() - 1; i >= 0; i--) {
            delete instances[i];
            LV2Instance *instance =
                plugin.instantiate(static_cast<double>(sampleRate));
            instance->setState(state);
            instances[i] = instance;
        }
        emit sampleRateChanged(sampleRate);
    }
}

void
Effect::setState(const QByteArray &stateBytes)
{
    int count = instances.count();
    assert(count);
    LV2State *state = world.createState(stateBytes);
    QScopedPointer<LV2State> statePtr(state);
    instances[0]->setState(state, Effect::setPortValue, this);
    for (int i = instances.count() - 1; i > 0; i--) {
        instances[i]->setState(state);
    }
}
