/*
 * libsynthclone_portmedia - PortAudio/PortMIDI sampler plugin for `synthclone`
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

#include <QtCore/QDebug>

#include <synthclone/error.h>

#include "participant.h"

Participant::Participant(QObject *parent):
    synthclone::Participant(tr("Port Media"), 0, 0, 1, "Devin Anderson",
                            tr("Sampling via PortAudio/PortMIDI"), parent),
    addSamplerAction(tr("PortMedia Sampler"))
{
    connect(&addSamplerAction, SIGNAL(triggered()),
            SLOT(handleAddSamplerActionTrigger()));

    connect(&samplerView, SIGNAL(addRequest()),
            SLOT(handleAddSamplerRequest()));
    connect(&samplerView, SIGNAL(closeRequest()),
            SLOT(handleSamplerViewCloseRequest()));

    context = 0;
}

Participant::~Participant()
{
    // Empty
}

void
Participant::activate(synthclone::Context &context, const QVariant &/*state*/)
{
    synthclone::SampleChannelCount channels = context.getSampleChannelCount();
    sampler = new Sampler(tr("PortMedia Sampler"), channels,
                          context.getSampleRate(), this);
    context.addMenuAction(&addSamplerAction, synthclone::MENU_ADD_SAMPLER);

    samplerView.setName(sampler->getName());

    samplerView.clearAudioAPIs();
    int count = sampler->getAudioAPICount();
    for (int i = 0; i < count; i++) {
        samplerView.addAudioAPI(i, sampler->getAudioAPIName(i));
    }
    samplerView.setAudioAPI(sampler->getAudioAPIIndex());

    samplerView.clearAudioInputDevices();
    count = sampler->getAudioInputDeviceCount();
    for (int i = 0; i < count; i++) {
        samplerView.addAudioInputDevice
            (i, sampler->getAudioInputDeviceName(i));
    }
    samplerView.setAudioInputDevice(sampler->getAudioInputDeviceIndex());
    samplerView.setAudioInputDeviceChannelCount
        (sampler->getAudioInputDeviceChannelCount());

    samplerView.clearAudioOutputDevices();
    count = sampler->getAudioOutputDeviceCount();
    for (int i = 0; i < count; i++) {
        samplerView.addAudioOutputDevice
            (i, sampler->getAudioOutputDeviceName(i));
    }
    samplerView.setAudioOutputDevice(sampler->getAudioOutputDeviceIndex());
    samplerView.setAudioOutputDeviceChannelCount
        (sampler->getAudioOutputDeviceChannelCount());

    samplerView.clearMIDIDevices();
    count = sampler->getMIDIDeviceCount();
    for (int i = 0; i < count; i++) {
        samplerView.addMIDIDevice(i, sampler->getMIDIDeviceName(i));
    }
    samplerView.setMIDIDevice(sampler->getMIDIDeviceIndex());

    samplerView.setSampleChannelCount(channels);
    for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
        samplerView.setAudioInputChannel(i, sampler->getAudioInputChannel(i));
        samplerView.setAudioOutputChannel
            (i, sampler->getAudioOutputChannel(i));
    }

    connect(&samplerView, SIGNAL(audioAPIChangeRequest(int)),
            sampler, SLOT(setAudioAPIIndex(int)));
    connect(&samplerView,
            SIGNAL(audioInputChannelChangeRequest
                   (synthclone::SampleChannelCount,
                    synthclone::SampleChannelCount)),
            sampler,
            SLOT(setAudioInputChannel(synthclone::SampleChannelCount,
                                      synthclone::SampleChannelCount)));
    connect(&samplerView, SIGNAL(audioInputDeviceChangeRequest(int)),
            sampler, SLOT(setAudioInputDeviceIndex(int)));
    connect(&samplerView,
            SIGNAL(audioOutputChannelChangeRequest
                   (synthclone::SampleChannelCount,
                    synthclone::SampleChannelCount)),
            sampler,
            SLOT(setAudioOutputChannel(synthclone::SampleChannelCount,
                                       synthclone::SampleChannelCount)));
    connect(&samplerView, SIGNAL(audioOutputDeviceChangeRequest(int)),
            sampler, SLOT(setAudioOutputDeviceIndex(int)));
    connect(&samplerView, SIGNAL(midiDeviceChangeRequest(int)),
            sampler, SLOT(setMIDIDeviceIndex(int)));
    connect(&samplerView, SIGNAL(nameChangeRequest(const QString &)),
	    sampler, SLOT(setName(const QString &)));

    connect(sampler, SIGNAL(audioAPIIndexChanged(int)),
            &samplerView, SLOT(setAudioAPI(int)));
    connect(sampler,
            SIGNAL(audioInputChannelChanged
                   (synthclone::SampleChannelCount,
                    synthclone::SampleChannelCount)),
            &samplerView,
            SLOT(setAudioInputChannel(synthclone::SampleChannelCount,
                                      synthclone::SampleChannelCount)));
    connect(sampler, SIGNAL(audioInputDeviceAdded(int, const QString &)),
            &samplerView, SLOT(addAudioInputDevice(int, const QString &)));
    connect(sampler,
            SIGNAL(audioInputDeviceChannelCountChanged
                   (synthclone::SampleChannelCount)),
            &samplerView,
            SLOT(setAudioInputDeviceChannelCount
                 (synthclone::SampleChannelCount)));
    connect(sampler, SIGNAL(audioInputDeviceIndexChanged(int)),
            &samplerView, SLOT(setAudioInputDevice(int)));
    connect(sampler,
            SIGNAL(audioOutputChannelChanged
                   (synthclone::SampleChannelCount,
                    synthclone::SampleChannelCount)),
            &samplerView,
            SLOT(setAudioOutputChannel(synthclone::SampleChannelCount,
                                       synthclone::SampleChannelCount)));
    connect(sampler, SIGNAL(audioOutputDeviceAdded(int, const QString &)),
            &samplerView, SLOT(addAudioOutputDevice(int, const QString &)));
    connect(sampler,
            SIGNAL(audioOutputDeviceChannelCountChanged
                   (synthclone::SampleChannelCount)),
            &samplerView,
            SLOT(setAudioOutputDeviceChannelCount
                 (synthclone::SampleChannelCount)));
    connect(sampler, SIGNAL(audioOutputDeviceIndexChanged(int)),
            &samplerView, SLOT(setAudioOutputDevice(int)));
    connect(sampler, SIGNAL(channelsChanged(synthclone::SampleChannelCount)),
            &samplerView,
            SLOT(setSampleChannelCount(synthclone::SampleChannelCount)));
    connect(sampler, SIGNAL(midiDeviceIndexChanged(int)),
            &samplerView, SLOT(setMIDIDevice(int)));
    connect(sampler, SIGNAL(nameChanged(const QString &)),
            &samplerView, SLOT(setName(const QString &)));

    connect(sampler, SIGNAL(midiError(const QString &)),
            SLOT(handleMIDIError(const QString &)));

    connect(&context,
            SIGNAL(sampleChannelCountChanged(synthclone::SampleChannelCount)),
            SLOT(handleChannelCountChange(synthclone::SampleChannelCount)));
    connect(&context, SIGNAL(sampleRateChanged(synthclone::SampleRate)),
            SLOT(handleSampleRateChange(synthclone::SampleRate)));

    this->context = &context;
}

bool
Participant::addSampler()
{
    try {
        sampler->activate();
    } catch (synthclone::Error &e) {
        context->reportError(e.getMessage());
        return false;
    }
    addSamplerAction.setEnabled(false);
    const synthclone::Registration &registration =
        context->addSampler(sampler);
    connect(&registration, SIGNAL(unregistered(QObject *)),
            SLOT(handleSamplerUnregistration(QObject *)));
    return true;
}

void
Participant::deactivate(synthclone::Context &context)
{
    disconnect(&context,
               SIGNAL(sampleChannelCountChanged
                      (synthclone::SampleChannelCount)),
               this,
               SLOT(handleChannelCountChange(synthclone::SampleChannelCount)));
    disconnect(&context, SIGNAL(sampleRateChanged(synthclone::SampleRate)),
               this, SLOT(handleSampleRateChange(synthclone::SampleRate)));

    context.removeMenuAction(&addSamplerAction);

    if (sampler->isActive()) {
        context.removeSampler();
    }
    delete sampler;
    this->context = 0;
}

QVariant
Participant::getState(const synthclone::Sampler *sampler) const
{
    QVariantMap map;
    const Sampler *s = qobject_cast<const Sampler *>(sampler);
    assert(s);

    int index = s->getAudioAPIIndex();
    map["audioAPIIndex"] = index;
    map["audioAPIName"] = s->getAudioAPIName(index);

    index = s->getAudioInputDeviceIndex();
    map["audioInputDeviceIndex"] = index;
    map["audioInputDeviceName"] = s->getAudioInputDeviceName(index);

    index = s->getAudioOutputDeviceIndex();
    map["audioOutputDeviceIndex"] = index;
    map["audioOutputDeviceName"] = s->getAudioOutputDeviceName(index);

    synthclone::SampleChannelCount channels = s->getChannels();
    QVariantList inputChannels;
    QVariantList outputChannels;
    for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
        inputChannels.append(static_cast<int>(s->getAudioInputChannel(i)));
        outputChannels.append(static_cast<int>(s->getAudioOutputChannel(i)));
    }
    map["audioInputChannels"] = inputChannels;
    map["audioOutputChannels"] = outputChannels;

    index = s->getMIDIDeviceIndex();
    map["midiDeviceIndex"] = index;
    map["midiDeviceName"] = s->getMIDIDeviceName(index);

    return map;
}

void
Participant::handleAddSamplerActionTrigger()
{
    samplerView.setVisible(true);
}

void
Participant::handleAddSamplerRequest()
{
    if (addSampler()) {
        samplerView.setVisible(false);
    }
}

void
Participant::handleChannelCountChange(synthclone::SampleChannelCount count)
{
    if (sampler->isActive()) {
        context->removeSampler();
        context->reportError(tr("The PortMedia sampler was removed because "
                                "the session's sample channel count has been "
                                "changed."));
    }
    sampler->setChannels(count);
}

void
Participant::handleMIDIError(const QString &message)
{
    context->removeSampler();
    context->reportError(tr("The PortMedia sampler was removed due to a MIDI "
                            "error: %1").arg(message));
}

void
Participant::handleSampleRateChange(synthclone::SampleRate sampleRate)
{
    if (sampler->isActive()) {
        context->removeSampler();
        context->reportError(tr("The PortMedia sampler was removed because "
                                "the session's sample rate has been "
                                "changed."));
    }
    sampler->setSampleRate(sampleRate);
}

void
Participant::handleSamplerUnregistration(QObject *obj)
{
    qobject_cast<Sampler *>(obj)->deactivate();
    addSamplerAction.setEnabled(true);
}

void
Participant::handleSamplerViewCloseRequest()
{
    samplerView.setVisible(false);
}

void
Participant::restoreSampler(const QVariant &state)
{
    synthclone::SampleChannelCount channels = context->getSampleChannelCount();
    QVariantList channelMap;
    int count;
    synthclone::SampleChannelCount deviceChannelCount;
    QStringList errorMessages;
    int index;
    QLocale locale = QLocale::system();
    const QVariantMap map = state.toMap();
    QString name;
    bool success;
    QVariant value;

    // Find the audio API.
    value = map.value("audioAPIIndex", "");
    index = value.toInt(&success);
    if ((! success) || (index < 0)) {
        errorMessages.append(tr("non-integer API index '%1'").
                             arg(value.toString()));
        goto findMIDIDevice;
    }
    count = sampler->getAudioAPICount();
    name = map.value("audioAPIName", "").toString();
    if (index < count) {
        if (sampler->getAudioAPIName(index) == name) {
            sampler->setAudioAPIIndex(index);
            goto findAudioInputDevice;
        }
        qWarning() << tr("API name at saved API index '%1' doesn't match "
                         "saved PortAudio API name '%2'").
            arg(locale.toString(index), name);
    } else {
        qWarning() << tr("Saved API index '%1' is out of range").
            arg(locale.toString(index));
    }
    qDebug() << tr("Searching for PortAudio API '%1'").arg(name);
    for (int i = 0; i < count; i++) {
        if (sampler->getAudioAPIName(i) == name) {
            sampler->setAudioAPIIndex(i);
            goto findAudioInputDevice;
        }
    }
    errorMessages.append(tr("could not find PortAudio API '%1'").arg(name));
    goto findMIDIDevice;

findAudioInputDevice:
    value = map.value("audioInputDeviceIndex", "");
    index = value.toInt(&success);
    if ((! success) || (index < 0)) {
        errorMessages.append(tr("non-integer audio input device index '%1'").
                             arg(value.toString()));
        goto findAudioOutputDevice;
    }
    count = sampler->getAudioInputDeviceCount();
    name = map.value("audioInputDeviceName", "").toString();
    if (index < count) {
        if (sampler->getAudioInputDeviceName(index) == name) {
            sampler->setAudioInputDeviceIndex(index);
            goto loadAudioInputChannelMap;
        }
        qWarning() << tr("Audio input device name at saved index '%1' doesn't "
                         "match saved PortAudio device name '%2'").
            arg(locale.toString(index), name);
    } else {
        qWarning() << tr("Saved audio input device index '%1' is out of "
                         "range").arg(locale.toString(index));
    }
    qDebug() << tr("Searching for PortAudio input device '%1'").arg(name);
    for (int i = 0; i < count; i++) {
        if (sampler->getAudioInputDeviceName(i) == name) {
            sampler->setAudioInputDeviceIndex(i);
            goto loadAudioInputChannelMap;
        }
    }
    errorMessages.append(tr("could not find PortAudio input device '%1'").
                         arg(name));
    goto findAudioOutputDevice;

loadAudioInputChannelMap:
    channelMap = map.value("audioInputChannels", "").toList();
    count = channelMap.count();
    if (! count) {
        errorMessages.append(tr("audio input channel map not found"));
        goto findAudioOutputDevice;
    }
    if (count != channels) {
        errorMessages.append(tr("channel count does not match input channel "
                                "map length"));
        goto findAudioOutputDevice;
    }

    deviceChannelCount = sampler->getAudioInputDeviceChannelCount();
    for (int i = 0; i < count; i++) {
        value = channelMap[i];
        index = value.toInt(&success);
        if (! success) {
            errorMessages.append(tr("sample channel '%1' has non-integer "
                                    "input channel '%2'").
                                 arg(locale.toString(i), value.toString()));
            continue;
        }
        if ((index < 0) || (index >= deviceChannelCount)) {
            errorMessages.append(tr("sample channel '%1' has out of range "
                                    "input channel '%2'").
                                 arg(locale.toString(i),
                                     locale.toString(index)));
            continue;
        }
        sampler->setAudioInputChannel
            (static_cast<synthclone::SampleChannelCount>(i),
             static_cast<synthclone::SampleChannelCount>(index));
    }

findAudioOutputDevice:
    value = map.value("audioOutputDeviceIndex", "");
    index = value.toInt(&success);
    if ((! success) || (index < 0)) {
        errorMessages.append(tr("non-integer audio output device index '%1'").
                             arg(value.toString()));
        goto findMIDIDevice;
    }
    count = sampler->getAudioOutputDeviceCount();
    name = map.value("audioOutputDeviceName", "").toString();
    if (index < count) {
        if (sampler->getAudioOutputDeviceName(index) == name) {
            sampler->setAudioOutputDeviceIndex(index);
            goto loadAudioOutputChannelMap;
        }
        qWarning() << tr("Audio output device name at saved index '%1' "
                         "doesn't match saved PortAudio device name '%2'").
            arg(locale.toString(index), name);
    } else {
        qWarning() << tr("Saved audio output device index '%1' is out of "
                         "range").arg(locale.toString(index));
    }
    qDebug() << tr("Searching for PortAudio output device '%1'").arg(name);
    for (int i = 0; i < count; i++) {
        if (sampler->getAudioOutputDeviceName(i) == name) {
            sampler->setAudioOutputDeviceIndex(i);
            goto loadAudioOutputChannelMap;
        }
    }
    errorMessages.append(tr("could not find PortAudio output device '%1'").
                         arg(name));
    goto findMIDIDevice;

loadAudioOutputChannelMap:
    channelMap = map.value("audioOutputChannels", "").toList();
    count = channelMap.count();
    if (! count) {
        errorMessages.append(tr("audio output channel map not found"));
        goto findAudioOutputDevice;
    }
    if (count != channels) {
        errorMessages.append(tr("channel count does not match output channel "
                                "map length"));
        goto findAudioOutputDevice;
    }
    deviceChannelCount = sampler->getAudioOutputDeviceChannelCount();
    for (int i = 0; i < count; i++) {
        value = channelMap[i];
        index = value.toInt(&success);
        if (! success) {
            errorMessages.append(tr("sample channel '%1' has non-integer "
                                    "output channel '%2'").
                                 arg(locale.toString(i), value.toString()));
            continue;
        }
        if ((index < 0) || (index >= deviceChannelCount)) {
            errorMessages.append(tr("sample channel '%1' has out of range "
                                    "output channel '%2'").
                                 arg(locale.toString(i),
                                     locale.toString(index)));
            continue;
        }
        sampler->setAudioOutputChannel
            (static_cast<synthclone::SampleChannelCount>(i),
             static_cast<synthclone::SampleChannelCount>(index));
    }

findMIDIDevice:
    value = map.value("midiDeviceIndex", "");
    index = value.toInt(&success);
    if ((! success) || (index < 0)) {
        errorMessages.append(tr("non-integer MIDI device index '%1'").
                             arg(value.toString()));
        goto checkErrors;
    }
    count = sampler->getMIDIDeviceCount();
    name = map.value("midiDeviceName", "").toString();
    if (index < count) {
        if (sampler->getMIDIDeviceName(index) == name) {
            sampler->setMIDIDeviceIndex(index);
            goto checkErrors;
        }
        qWarning() << tr("MIDI device name at saved index '%1' doesn't match "
                         "saved PortMIDI device name '%2'").
            arg(locale.toString(index), name);
    } else {
        qWarning() << tr("Saved MIDI device index '%1' is out of range").
            arg(locale.toString(index));
    }
    qDebug() << tr("Searching for PortMIDI device '%1'").arg(name);
    for (int i = 0; i < count; i++) {
        if (sampler->getMIDIDeviceName(i) == name) {
            sampler->setMIDIDeviceIndex(i);
            goto checkErrors;
        }
    }
    errorMessages.append(tr("could not find PortMIDI device '%1'").arg(name));

checkErrors:
    count = errorMessages.count();
    if (! count) {
        addSampler();
    } else {
        samplerView.setVisible(true);
        for (int i = 0; i < count; i++) {
            context->reportError(errorMessages[i]);
        }
    }
}
