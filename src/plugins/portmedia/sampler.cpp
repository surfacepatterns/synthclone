/*
 * libsynthclone_portmedia - PortAudio/PortMIDI sampler plugin for `synthclone`
 * Copyright (C) 2012-2013 Devin Anderson
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>

#include <synthclone/error.h>
#include <synthclone/sampleinputstream.h>
#include <synthclone/sampleoutputstream.h>

#include "sampler.h"

static const char *ERROR_MIDI_BUFFER = "The MIDI ringbuffer is full";

// Callbacks

int
Sampler::handleProcessEvent(const void *input, void *output,
                            unsigned long frames,
                            const PaStreamCallbackTimeInfo */*timeInfo*/,
                            PaStreamCallbackFlags statusFlags, void *userData)
{
    Sampler *sampler = static_cast<Sampler *>(userData);
    assert(sampler);
    return sampler->handleProcessEvent(static_cast<const float *>(input),
                                       static_cast<float *>(output), frames,
                                       statusFlags);
}

// Class implementation

Sampler::Sampler(const QString &name, synthclone::SampleChannelCount channels,
                 synthclone::SampleRate sampleRate, QObject *parent):
    synthclone::Sampler(name, parent),
    commandBuffer(2),
    eventBuffer(64),
    eventThread(this),
    midiBuffer(64),
    midiThread(this)
{
    assert(channels >= synthclone::SAMPLE_CHANNEL_COUNT_MINIMUM);
    assert((sampleRate == synthclone::SAMPLE_RATE_NOT_SET) ||
           ((sampleRate >= synthclone::SAMPLE_RATE_MINIMUM) &&
            (sampleRate <= synthclone::SAMPLE_RATE_MAXIMUM)));
    PaError paError = Pa_Initialize();
    if (paError != paNoError) {
        throw synthclone::Error(Pa_GetErrorText(paError));
    }
    try {
        PmError pmError = Pm_Initialize();
        if (pmError != pmNoError) {
            throw synthclone::Error(Pm_GetErrorText(pmError));
        }
        try {

            // Enumerate and gather PortAudio device information.
            PaHostApiIndex audioAPICount = Pa_GetHostApiCount();
            if (! audioAPICount) {
                throw synthclone::Error(tr("There are no available PortAudio "
                                           "APIs"));
            }
            PaHostApiIndex defaultAudioAPIIndex = Pa_GetDefaultHostApi();
            audioAPIIndex = 0;
            audioInputDeviceIndex = 0;
            audioOutputDeviceIndex = 0;
            for (int i = 0; i < audioAPICount; i++) {
                PaHostApiIndex apiIndex = static_cast<PaHostApiIndex>(i);
                const PaHostApiInfo *apiInfo = Pa_GetHostApiInfo(apiIndex);
                assert(apiInfo);
                AudioAPIData data;
                int audioDeviceCount = apiInfo->deviceCount;
                PaDeviceIndex defaultInputDevice = apiInfo->defaultInputDevice;
                PaDeviceIndex defaultOutputDevice =
                    apiInfo->defaultOutputDevice;
                int potentialInputDevice = 0;
                int potentialOutputDevice = 0;
                for (int j = 0; j < audioDeviceCount; j++) {
                    PaDeviceIndex deviceIndex =
                        Pa_HostApiDeviceIndexToDeviceIndex(apiIndex, j);
                    const PaDeviceInfo *deviceInfo =
                        Pa_GetDeviceInfo(deviceIndex);
                    assert(deviceInfo);
                    AudioDeviceData deviceData;
                    deviceData.index = deviceIndex;
                    deviceData.info = deviceInfo;
                    if (deviceInfo->maxInputChannels) {
                        data.inputDevices.append(deviceData);
                        if (deviceIndex == defaultInputDevice) {
                            potentialInputDevice =
                                data.inputDevices.count() - 1;
                        }
                    }
                    if (deviceInfo->maxOutputChannels) {
                        data.outputDevices.append(deviceData);
                        if (deviceIndex == defaultOutputDevice) {
                            potentialOutputDevice =
                                data.outputDevices.count() - 1;
                        }
                    }
                }
                if (data.inputDevices.count() && data.outputDevices.count()) {
                    data.defaultInputDeviceIndex = potentialInputDevice;
                    data.defaultOutputDeviceIndex = potentialOutputDevice;
                    data.index = apiIndex;
                    data.info = apiInfo;
                    audioAPIs.append(data);
                    if (apiIndex == defaultAudioAPIIndex) {
                        audioAPIIndex = audioAPIs.count() - 1;
                        audioInputDeviceIndex = potentialInputDevice;
                        audioOutputDeviceIndex = potentialOutputDevice;
                    }
                }
            }
            if (! audioAPIs.count()) {
                throw synthclone::Error(tr("None of the available PortAudio "
                                           "APIs contains devices that "
                                           "support both audio input and "
                                           "audio output"));
            }

            // Enumerate and gather PortMidi device information.
            int midiDeviceCount = Pm_CountDevices();
            if (! midiDeviceCount) {
                throw synthclone::Error(tr("There are no available PortMIDI "
                                           "devices"));
            }
            PmDeviceID defaultDeviceId = Pm_GetDefaultOutputDeviceID();
            midiDeviceIndex = 0;
            for (int i = 0; i < midiDeviceCount; i++) {
                PmDeviceID deviceId = static_cast<PmDeviceID>(i);
                const PmDeviceInfo *info = Pm_GetDeviceInfo(deviceId);
                assert(info);
                if (info->output) {
                    MIDIDeviceData data;
                    data.id = deviceId;
                    data.info = info;
                    midiDevices.append(data);
                    if (deviceId == defaultDeviceId) {
                        midiDeviceIndex = midiDevices.count() - 1;
                    }
                }
            }
            if (! midiDevices.count()) {
                throw synthclone::Error(tr("None of the available PortMIDI "
                                           "devices supports MIDI output"));
            }

            active = false;
            audioInputChannelIndices =
                new synthclone::SampleChannelCount[channels];
            audioInputDeviceChannelCount =
                getAudioInputDeviceData().info->maxInputChannels;
	    audioOutputChannelIndices =
                new synthclone::SampleChannelCount[channels];
            audioOutputDeviceChannelCount =
                getAudioOutputDeviceData().info->maxOutputChannels;
            this->channels = channels;
            this->sampleRate = sampleRate;

            // Set default channel indices.
            synthclone::SampleChannelCount inputChannels =
                getAudioInputDeviceChannelCount();
            synthclone::SampleChannelCount outputChannels =
                getAudioOutputDeviceChannelCount();
            for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
                audioInputChannelIndices[i] = i % inputChannels;
                audioOutputChannelIndices[i] = i % outputChannels;
            }

        } catch (...) {
            pmError = Pm_Terminate();
            if (pmError != pmNoError) {
                qWarning() << tr("Error terminating PortMidi: %1").
                    arg(Pm_GetErrorText(pmError));
            }
            throw;
        }
    } catch (...) {
        paError = Pa_Terminate();
        if (paError != paNoError) {
            qWarning() << tr("Error terminating PortAudio: %1").
                arg(Pa_GetErrorText(paError));
        }
        throw;
    }

    aborted = false;
    audioStream = 0;
    command.job = 0;
    command.sampleBuffer = 0;
    command.stream = 0;
    command.totalReleaseFrames = 0;
    command.totalSampleFrames = 0;
    currentFrame = 0;
    errorMessage = 0;
    idle = true;
    midiStream = 0;
    progress = 0;
    state = STATE_IDLE;
}

Sampler::~Sampler()
{
    if (active) {
        deactivate();
    }

    delete[] audioInputChannelIndices;
    delete[] audioOutputChannelIndices;

    PmError pmError = Pm_Terminate();
    if (pmError != pmNoError) {
        qWarning() << tr("Error terminating PortMidi: %1").
            arg(Pm_GetErrorText(pmError));
    }
    PaError paError = Pa_Terminate();
    if (paError != paNoError) {
        qWarning() << tr("Error terminating PortAudio: %1").
            arg(Pa_GetErrorText(paError));
    }
}

void
Sampler::abortJob()
{
    assert(active);
    Command command;
    command.job = 0;
    emit statusChanged(tr("Aborting ..."));
    sendCommand(command);
}

void
Sampler::activate()
{
    assert(! active);
    eventBuffer.flush();
    midiBuffer.flush();

    PmError pmError = Pm_OpenOutput(&midiStream,
                                    midiDevices[midiDeviceIndex].id, 0, 0, 0,
                                    0, 0);
    if (pmError != pmNoError) {
        throw synthclone::Error(tr("failed to open MIDI stream: %1").
                                arg(Pm_GetErrorText(pmError)));
    }
    try {
        PaStreamParameters inputParameters;
        const AudioDeviceData &inputData = getAudioInputDeviceData();
        const PaDeviceInfo *info = inputData.info;
        int inputChannels = info->maxInputChannels;
        inputParameters.channelCount = inputChannels;
        inputParameters.device = inputData.index;
        inputParameters.hostApiSpecificStreamInfo = 0;
        inputParameters.sampleFormat = paFloat32;
        inputParameters.suggestedLatency = info->defaultHighInputLatency;

        // If the sample rate isn't set, then take the sample rate from the
        // given default sample rate for the input device.
        synthclone::SampleRate streamSampleRate = sampleRate;
        if (sampleRate == synthclone::SAMPLE_RATE_NOT_SET) {
            streamSampleRate = static_cast<synthclone::SampleRate>
                (info->defaultSampleRate);
        }

        PaStreamParameters outputParameters;
        const AudioDeviceData &outputData = getAudioOutputDeviceData();
        info = outputData.info;
        int outputChannels = info->maxOutputChannels;
        outputParameters.channelCount = outputChannels;
        outputParameters.device = outputData.index;
        outputParameters.hostApiSpecificStreamInfo = 0;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = info->defaultHighOutputLatency;

        PaError paError = Pa_OpenStream(&audioStream, &inputParameters,
                                        &outputParameters, streamSampleRate,
                                        paFramesPerBufferUnspecified, paNoFlag,
                                        handleProcessEvent, this);
        if (paError != paNoError) {
            throw synthclone::Error(tr("failed to open audio stream: %1").
                                    arg(Pa_GetErrorText(paError)));
        }
        try {
            idle = true;
            state = STATE_IDLE;
            paError = Pa_StartStream(audioStream);
            if (paError != paNoError) {
                throw synthclone::Error(tr("failed to start audio stream: %1").
                                        arg(Pa_GetErrorText(paError)));
            }

            if (streamSampleRate != sampleRate) {
                sampleRate = streamSampleRate;
                emit sampleRateChanged(sampleRate);
            }

            eventThread.start();
            midiThread.start();
            active = true;

        } catch (...) {
            Pa_CloseStream(audioStream);
            throw;
        }
    } catch (...) {
        Pm_Close(midiStream);
        throw;
    }
}

void
Sampler::copyData(const float *input, float *output, unsigned long totalFrames,
                  unsigned long startFrame)
{
    synthclone::SampleFrameCount total =
        static_cast<synthclone::SampleFrameCount>(totalFrames);
    for (synthclone::SampleFrameCount i = startFrame; i < total; i++) {
        synthclone::SampleFrameCount inputOffset =
            audioInputDeviceChannelCount * i;
        synthclone::SampleFrameCount outputOffset =
            audioOutputDeviceChannelCount * i;
        initializeOutputFrame(output, outputOffset);
        for (synthclone::SampleChannelCount j = 0; j < channels; j++) {
            output[outputOffset + audioOutputChannelIndices[j]] +=
                input[inputOffset + audioInputChannelIndices[j]];
        }
    }
}

void
Sampler::deactivate()
{
    // Stop audio processing
    Pa_StopStream(audioStream);
    Pa_CloseStream(audioStream);

    // Send signals to terminate threads
    eventSemaphore.post();
    midiSemaphore.post();

    // Wait until threads actually terminate
    eventThread.wait();
    midiThread.wait();

    // Now, the MIDI stream can be closed
    Pm_Close(midiStream);
    active = false;
}

int
Sampler::getAudioAPICount() const
{
    return audioAPIs.count();
}

int
Sampler::getAudioAPIIndex() const
{
    return audioAPIIndex;
}

QString
Sampler::getAudioAPIName(int index) const
{
    assert((index >= 0) && (index < audioAPIs.count()));
    return audioAPIs[index].info->name;
}

synthclone::SampleChannelCount
Sampler::getAudioInputChannel(synthclone::SampleChannelCount channel) const
{
    assert(channel < getAudioInputDeviceChannelCount());
    return audioInputChannelIndices[channel];
}

synthclone::SampleChannelCount
Sampler::getAudioInputDeviceChannelCount() const
{
    return getAudioInputDeviceChannelCount(audioInputDeviceIndex);
}

synthclone::SampleChannelCount
Sampler::getAudioInputDeviceChannelCount(int index) const
{
    return getAudioInputDeviceData(index).info->maxInputChannels;
}

int
Sampler::getAudioInputDeviceCount() const
{
    return audioAPIs[audioAPIIndex].inputDevices.count();
}

const Sampler::AudioDeviceData &
Sampler::getAudioInputDeviceData() const
{
    return getAudioInputDeviceData(audioInputDeviceIndex);
}

const Sampler::AudioDeviceData &
Sampler::getAudioInputDeviceData(int index) const
{
    const AudioDeviceDataList &inputDevices =
        audioAPIs[audioAPIIndex].inputDevices;
    assert((index >= 0) && (index < inputDevices.count()));
    return inputDevices[index];
}

int
Sampler::getAudioInputDeviceIndex() const
{
    return audioInputDeviceIndex;
}

QString
Sampler::getAudioInputDeviceName(int index) const
{
    return getAudioInputDeviceData(index).info->name;
}

synthclone::SampleChannelCount
Sampler::getAudioOutputChannel(synthclone::SampleChannelCount channel) const
{
    assert(channel < getAudioOutputDeviceChannelCount());
    return audioOutputChannelIndices[channel];
}

synthclone::SampleChannelCount
Sampler::getAudioOutputDeviceChannelCount() const
{
    return getAudioOutputDeviceChannelCount(audioOutputDeviceIndex);
}

synthclone::SampleChannelCount
Sampler::getAudioOutputDeviceChannelCount(int index) const
{
    return getAudioOutputDeviceData(index).info->maxOutputChannels;
}

int
Sampler::getAudioOutputDeviceCount() const
{
    return audioAPIs[audioAPIIndex].outputDevices.count();
}

const Sampler::AudioDeviceData &
Sampler::getAudioOutputDeviceData() const
{
    return getAudioOutputDeviceData(audioOutputDeviceIndex);
}

const Sampler::AudioDeviceData &
Sampler::getAudioOutputDeviceData(int index) const
{
    const AudioDeviceDataList &outputDevices =
        audioAPIs[audioAPIIndex].outputDevices;
    assert((index >= 0) && (index < outputDevices.count()));
    return outputDevices[index];
}

int
Sampler::getAudioOutputDeviceIndex() const
{
    return audioOutputDeviceIndex;
}

QString
Sampler::getAudioOutputDeviceName(int index) const
{
    return getAudioOutputDeviceData(index).info->name;
}

synthclone::SampleChannelCount
Sampler::getChannels() const
{
    return channels;
}

int
Sampler::getMIDIDeviceCount() const
{
    return midiDevices.count();
}

int
Sampler::getMIDIDeviceIndex() const
{
    return midiDeviceIndex;
}

QString
Sampler::getMIDIDeviceName(int index) const
{
    assert((index >= 0) && (index < midiDevices.count()));
    return midiDevices[index].info->name;
}

synthclone::SampleRate
Sampler::getSampleRate() const
{
    return sampleRate;
}

int
Sampler::handleProcessEvent(const float *input, float *output,
                            unsigned long frames,
                            PaStreamCallbackFlags statusFlags)
{
    bool genericCopy = true;
    const synthclone::SamplerJob *job;
    synthclone::MIDIData midiChannel;
    synthclone::SampleFrameCount processedFrames;
    const synthclone::Zone *zone;

    if (statusFlags & paInputOverflow) {
        sendSimpleEvent(Event::TYPE_INPUT_OVERFLOW);
    }
    if (statusFlags & paInputUnderflow) {
        sendSimpleEvent(Event::TYPE_INPUT_UNDERFLOW);
    }
    if (statusFlags & paOutputOverflow) {
        sendSimpleEvent(Event::TYPE_OUTPUT_OVERFLOW);
    }
    if (statusFlags & paOutputUnderflow) {
        sendSimpleEvent(Event::TYPE_OUTPUT_UNDERFLOW);
    }

    switch (state) {

    // Waiting for commands
    case STATE_IDLE:
    idle:
        while (commandBuffer.isReadable()) {
            commandBuffer.read(command);
            job = command.job;
            if (job) {
                aborted = false;
                currentFrame = 0;
                errorMessage = 0;
                if (job->getType() == synthclone::SamplerJob::TYPE_SAMPLE) {
                    state = STATE_SAMPLE_SEND_PRE_MIDI;
                    goto sampleSendPreMIDI;
                }
                state = STATE_PLAY;
                goto play;
            }
            // It's an abort command.  We can ignore it.
        }
        break;

    // Execute 'play' command
    case STATE_PLAY:
    play:
        updateCommandState();
        if (state == STATE_ABORT) {
            goto abort;
        }
        if (state == STATE_ERROR) {
            goto error;
        }
        genericCopy = false;
        processedFrames = playData(input, output, frames);
        if (static_cast<unsigned long>(processedFrames) < frames) {
            copyData(input, output, frames, frames - processedFrames);
            sendProgressEvent(1.0);
            state = STATE_COMPLETED;
            goto completed;
        }
        currentFrame += frames;
        sendProgressEvent(static_cast<float>(currentFrame) /
                          command.totalSampleFrames);
        break;

    // Four states for executing 'sample' command.
    case STATE_SAMPLE_SEND_PRE_MIDI:
    sampleSendPreMIDI:
        updateCommandState();
        if (state == STATE_ABORT) {
            goto abort;
        }
        if (state == STATE_ERROR) {
            goto error;
        }
        zone = command.job->getZone();

        {
            using synthclone::Zone;

            const Zone::ControlMap &controlValues = zone->getControlMap();
            Zone::ControlMap::const_iterator end = controlValues.end();
            midiChannel = zone->getChannel() - 1;
            for (Zone::ControlMap::const_iterator iter = controlValues.begin();
                 iter != end; iter++) {
                if (! sendMIDIMessage(0xb0 | midiChannel, iter.key(),
                                      iter.value())) {
                    goto error;
                }
            }
            synthclone::MIDIData note = zone->getNote();
            if (! sendMIDIMessage(0x90 | midiChannel, note,
                                  zone->getVelocity())) {
                goto error;
            }
            synthclone::MIDIData pressure = zone->getChannelPressure();
            if (pressure != synthclone::MIDI_VALUE_NOT_SET) {
                if (! sendMIDIMessage(0xb0 | midiChannel, pressure)) {
                    goto error;
                }
            }
            synthclone::MIDIData aftertouch = zone->getAftertouch();
            if (aftertouch != synthclone::MIDI_VALUE_NOT_SET) {
                if (! sendMIDIMessage(0xa0 | midiChannel, note, aftertouch)) {
                    goto error;
                }
            }
        }
        state = STATE_SAMPLE;
        break;

    case STATE_SAMPLE:
        updateCommandState();
        switch (state) {
        case STATE_ABORT:
            aborted = true;
            // Fallthrough on purpose.
        case STATE_ERROR:
            goto sampleSendNoteOff;
        default:
            ;
        }
        genericCopy = false;
        processedFrames = recordData(input, output, frames);
        if (static_cast<unsigned long>(processedFrames) >= frames) {
            currentFrame += frames;
            sendProgressEvent(static_cast<float>(currentFrame) /
                              (command.totalSampleFrames +
                               command.totalReleaseFrames));
            break;
        }
        copyData(input, output, frames, processedFrames);
        sendProgressEvent(static_cast<float>(command.totalSampleFrames) /
                          (command.totalSampleFrames +
                           command.totalReleaseFrames));
    sampleSendNoteOff:
        zone = command.job->getZone();
        if (! sendMIDIMessage(0x80 | (zone->getChannel() - 1), zone->getNote(),
                              zone->getVelocity())) {
            state = STATE_ERROR;
            goto error;
        }
        currentFrame = 0;
        state = STATE_SAMPLE_RELEASE;
        break;

    case STATE_SAMPLE_RELEASE:
        updateCommandState();

        // We still have to catch abort and error messages.  However, state
        // changes must wait until we've sent the post-MIDI messages.
        switch (state) {
        case STATE_ABORT:
            aborted = true;
            // Fallthrough on purpose.
        case STATE_ERROR:
            state = STATE_SAMPLE_RELEASE;
        default:
            ;
        }
        currentFrame += frames;
        if (currentFrame < command.totalReleaseFrames) {
            copyData(input, output, frames, 0);
            synthclone::SampleFrameCount totalSampleFrames =
                command.totalSampleFrames;
            sendProgressEvent((static_cast<float>(currentFrame) +
                               totalSampleFrames) /
                              (command.totalReleaseFrames +
                               totalSampleFrames));
            break;
        }
        sendProgressEvent(1.0);

        // Send MIDI messages to turn sound off and reset controllers.
        zone = command.job->getZone();
        midiChannel = zone->getChannel() - 1;
        if (! sendMIDIMessage(0xb0 | midiChannel, 0x78, 0)) {
            state = STATE_ERROR;
            goto error;
        }
        if (! sendMIDIMessage(0xb0 | midiChannel, 0x79, 0)) {
            state = STATE_ERROR;
            goto error;
        }

        if (errorMessage) {
            state = STATE_ERROR;
            goto error;
        }
        if (aborted) {
            state = STATE_ABORT;
            goto abort;
        }
        state = STATE_COMPLETED;
        // Fallthrough on purpose.

    // Operation finalization states.
    case STATE_COMPLETED:
    completed:
        if (sendJobFinalizationEvent(Event::TYPE_COMPLETE)) {
            goto idle;
        }
        break;

    case STATE_ABORT:
    abort:
        if (sendJobFinalizationEvent(Event::TYPE_ABORTED)) {
            goto idle;
        }
        break;

    case STATE_ERROR:
    error:
        if (sendErrorEvent()) {
            state = STATE_IDLE;
            goto idle;
        }
        break;
    }
    if (genericCopy) {
        copyData(input, output, frames, 0);
    }
    return paContinue;
}

void
Sampler::initializeOutputFrame(float *output,
                               synthclone::SampleFrameCount offset)
{
    for (synthclone::SampleChannelCount i = 0;
         i < audioOutputDeviceChannelCount; i++) {
        output[offset + i] = 0.0;
    }
}

bool
Sampler::isActive() const
{
    return active;
}

void
Sampler::monitorEvents()
{
    for (;;) {
        eventSemaphore.wait();
        if (! eventBuffer.isReadable()) {
            break;
        }
        Command *command;
        Event event;
        const synthclone::SamplerJob *job;
        eventBuffer.read(event);
        switch (event.type) {
        case Event::TYPE_ABORTED:
            idle = true;
            emit statusChanged(tr("Idle."));
            emit jobAborted();
            emit progressChanged(0.0);
            command = &(event.data.command);
            break;
        case Event::TYPE_COMPLETE:
            command = &(event.data.command);
            job = command->job;
            if (job->getType() == synthclone::SamplerJob::TYPE_SAMPLE) {
                synthclone::SampleOutputStream *stream =
                    qobject_cast<synthclone::SampleOutputStream *>
                    (command->stream);
                float *sampleBuffer = command->sampleBuffer;
                stream->write(sampleBuffer, command->totalSampleFrames);
            }
            idle = true;
            emit statusChanged(tr("Idle."));
            emit jobCompleted();
            emit progressChanged(0.0);
            break;
        case Event::TYPE_ERROR:
            idle = true;
            emit statusChanged(tr("Idle."));
            emit jobError(event.data.error.message);
            command = &(event.data.error.command);
            break;
        case Event::TYPE_INPUT_OVERFLOW:
            qWarning() << "PortMedia input overflow detected.";
            continue;
        case Event::TYPE_INPUT_UNDERFLOW:
            qWarning() << "PortMedia input underflow detected.";
            continue;
        case Event::TYPE_OUTPUT_OVERFLOW:
            qWarning() << "PortMedia output overflow detected.";
            continue;
        case Event::TYPE_OUTPUT_UNDERFLOW:
            qWarning() << "PortMedia output underflow detected.";
            continue;
        case Event::TYPE_PROGRESS:
            emit progressChanged(event.data.progress);
            continue;
        default:
            assert(false);
        }
        delete[] command->sampleBuffer;
    }
}

synthclone::SampleFrameCount
Sampler::playData(const float *input, float *output, unsigned long totalFrames)
{
    synthclone::SampleFrameCount nextFrame = currentFrame + totalFrames;
    float *sampleBuffer = command.sampleBuffer;
    synthclone::SampleFrameCount totalSampleFrames = command.totalSampleFrames;
    synthclone::SampleFrameCount copyFrames = nextFrame >= totalSampleFrames ?
        totalSampleFrames - currentFrame : totalFrames;
    for (synthclone::SampleFrameCount i = 0; i < copyFrames; i++) {
        synthclone::SampleFrameCount inputOffset =
            audioInputDeviceChannelCount * i;
        synthclone::SampleFrameCount outputOffset =
            audioOutputDeviceChannelCount * i;
        synthclone::SampleFrameCount sampleOffset =
            (currentFrame + i) * channels;
        initializeOutputFrame(output, outputOffset);
        for (synthclone::SampleChannelCount j = 0; j < channels; j++) {
            output[outputOffset + audioOutputChannelIndices[j]] +=
                input[inputOffset + audioInputChannelIndices[j]] +
                sampleBuffer[sampleOffset + j];
        }
    }
    return copyFrames;
}

synthclone::SampleFrameCount
Sampler::recordData(const float *input, float *output,
                    unsigned long totalFrames)
{
    synthclone::SampleFrameCount nextFrame = currentFrame + totalFrames;
    float *sampleBuffer = command.sampleBuffer;
    synthclone::SampleFrameCount totalSampleFrames = command.totalSampleFrames;
    synthclone::SampleFrameCount copyFrames = nextFrame >= totalSampleFrames ?
        totalSampleFrames - currentFrame : totalFrames;
    for (synthclone::SampleFrameCount i = 0; i < copyFrames; i++) {
        synthclone::SampleFrameCount inputOffset =
            audioInputDeviceChannelCount * i;
        synthclone::SampleFrameCount outputOffset =
            audioOutputDeviceChannelCount * i;
        synthclone::SampleFrameCount sampleOffset =
            (currentFrame + i) * channels;
        initializeOutputFrame(output, outputOffset);
        for (synthclone::SampleChannelCount j = 0; j < channels; j++) {
            float n = input[inputOffset + audioInputChannelIndices[j]];
            sampleBuffer[sampleOffset + j] = n;
            output[outputOffset + audioOutputChannelIndices[j]] += n;
        }
    }
    return copyFrames;
}

void
Sampler::runMIDI()
{

    // XXX: Acquire realtime priority.

    for (;;) {
        midiSemaphore.wait();
        if (! midiBuffer.isReadable()) {
            break;
        }
        PmMessage message;
        midiBuffer.read(message);
        PmError pmError = Pm_WriteShort(midiStream, 0, message);
        if (pmError != pmNoError) {
            QString s(Pm_GetErrorText(pmError));
            emit midiError(s);
        }
    }
}

void
Sampler::sendCommand(const Command &command)
{
    assert(commandBuffer.isWritable());
    commandBuffer.write(command);
}

bool
Sampler::sendErrorEvent()
{
    Event event;
    memcpy(static_cast<void *>(&(event.data.error.command)),
           static_cast<void *>(&command), sizeof(Command));
    event.data.error.message = errorMessage;
    event.type = Event::TYPE_ERROR;
    return sendEvent(event);
}

bool
Sampler::sendEvent(const Event &event)
{
    bool result = eventBuffer.write(event);
    if (result) {
        eventSemaphore.post();
    }
    return result;
}

bool
Sampler::sendJobFinalizationEvent(Event::Type type)
{
    Event event;
    memcpy(static_cast<void *>(&(event.data.command)),
           static_cast<void *>(&command), sizeof(Command));
    event.type = type;
    bool result = sendEvent(event);
    if (result) {
        state = STATE_IDLE;
    }
    return result;
}

bool
Sampler::sendMIDIMessage(synthclone::MIDIData status,
                         synthclone::MIDIData data1,
                         synthclone::MIDIData data2)
{
    if (! midiBuffer.isWritable()) {
        setErrorState(ERROR_MIDI_BUFFER);
        return false;
    }
    assert(data1 < 0x80);
    PmMessage message;
    if (data2 == synthclone::MIDI_VALUE_NOT_SET) {
        message = Pm_Message(status, data1, 0);
    } else {
        assert(data2 < 0x80);
        message = Pm_Message(status, data1, data2);
    }
    bool sent = midiBuffer.write(message);
    assert(sent);
    midiSemaphore.post();
    return true;
}

void
Sampler::sendProgressEvent(float progress)
{
    Event event;
    event.data.progress = progress;
    event.type = Event::TYPE_PROGRESS;
    sendEvent(event);
}

bool
Sampler::sendSimpleEvent(Event::Type type)
{
    Event event;
    event.type = type;
    return sendEvent(event);
}

void
Sampler::setAudioAPIIndex(int index)
{
    assert(! active);
    assert((index >= 0) && (index < audioAPIs.count()));
    if (index != audioAPIIndex) {
        audioAPIIndex = index;
        emit audioAPIIndexChanged(index);
        AudioAPIData &data = audioAPIs[index];
        AudioDeviceDataList &inputDevices = data.inputDevices;
        int deviceCount = inputDevices.count();
        for (int i = 0; i < deviceCount; i++) {
            emit audioInputDeviceAdded(i, inputDevices[i].info->name);
        }
        AudioDeviceDataList &outputDevices = data.outputDevices;
        deviceCount = outputDevices.count();
        for (int i = 0; i < deviceCount; i++) {
            emit audioOutputDeviceAdded(i, outputDevices[i].info->name);
        }
        audioInputDeviceIndex = -1;
        audioOutputDeviceIndex = -1;
        setAudioInputDeviceIndex(data.defaultInputDeviceIndex);
        setAudioOutputDeviceIndex(data.defaultOutputDeviceIndex);
    }
}

void
Sampler::setAudioInputChannel(synthclone::SampleChannelCount channel,
                              synthclone::SampleChannelCount deviceChannel)
{
    assert(! active);
    assert(channel < channels);
    assert(deviceChannel < audioInputDeviceChannelCount);
    if (audioInputChannelIndices[channel] != deviceChannel) {
        audioInputChannelIndices[channel] = deviceChannel;
        emit audioInputChannelChanged(channel, deviceChannel);
    }
}

void
Sampler::setAudioInputDeviceIndex(int index)
{
    assert(! active);
    AudioDeviceDataList &devices = audioAPIs[audioAPIIndex].inputDevices;
    assert((index >= 0) && (index < devices.count()));
    if (index != audioInputDeviceIndex) {
        audioInputDeviceIndex = index;
        audioInputDeviceChannelCount =
            static_cast<synthclone::SampleChannelCount>
            (devices[index].info->maxInputChannels);
        emit audioInputDeviceIndexChanged(index);
        emit audioInputDeviceChannelCountChanged(audioInputDeviceChannelCount);

        // Update input channel map
        for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
            synthclone::SampleChannelCount mappedChannel =
                audioInputChannelIndices[i];
            if (mappedChannel >= audioInputDeviceChannelCount) {
                mappedChannel = i % audioInputDeviceChannelCount;
                audioInputChannelIndices[i] = mappedChannel;
            }
            emit audioInputChannelChanged(i, mappedChannel);
        }
    }
}

void
Sampler::setAudioOutputChannel(synthclone::SampleChannelCount channel,
                               synthclone::SampleChannelCount deviceChannel)
{
    assert(! active);
    assert(channel < channels);
    assert(deviceChannel < audioOutputDeviceChannelCount);
    if (audioOutputChannelIndices[channel] != deviceChannel) {
        audioOutputChannelIndices[channel] = deviceChannel;
        emit audioOutputChannelChanged(channel, deviceChannel);
    }
}

void
Sampler::setAudioOutputDeviceIndex(int index)
{
    assert(! active);
    AudioDeviceDataList &devices = audioAPIs[audioAPIIndex].outputDevices;
    assert((index >= 0) && (index < devices.count()));
    if (index != audioOutputDeviceIndex) {
        audioOutputDeviceIndex = index;
        audioOutputDeviceChannelCount =
            static_cast<synthclone::SampleChannelCount>
            (devices[index].info->maxOutputChannels);
        emit audioOutputDeviceIndexChanged(index);
        emit audioOutputDeviceChannelCountChanged
            (audioOutputDeviceChannelCount);

        // Update output channel map
        for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
            synthclone::SampleChannelCount mappedChannel =
                audioOutputChannelIndices[i];
            if (mappedChannel >= audioOutputDeviceChannelCount) {
                mappedChannel = i % audioOutputDeviceChannelCount;
                audioOutputChannelIndices[i] = mappedChannel;
            }
            emit audioOutputChannelChanged(i, mappedChannel);
        }
    }
}

void
Sampler::setChannels(synthclone::SampleChannelCount channels)
{
    assert(! active);
    assert(channels >= synthclone::SAMPLE_CHANNEL_COUNT_MINIMUM);
    synthclone::SampleChannelCount oldChannels = this->channels;
    if (channels != oldChannels) {
        this->channels = channels;
        emit channelsChanged(channels);
        synthclone::SampleChannelCount *oldAudioInputChannelIndices =
            audioInputChannelIndices;
        synthclone::SampleChannelCount *oldAudioOutputChannelIndices =
            audioOutputChannelIndices;
        audioInputChannelIndices =
            new synthclone::SampleChannelCount[channels];
        audioOutputChannelIndices =
            new synthclone::SampleChannelCount[channels];
        synthclone::SampleChannelCount i;
        if (channels > oldChannels) {
            for (i = 0; i < oldChannels; i++) {
                audioInputChannelIndices[i] = oldAudioInputChannelIndices[i];
                audioOutputChannelIndices[i] = oldAudioOutputChannelIndices[i];
            }
            for (; i < channels; i++) {
                synthclone::SampleChannelCount c =
                    i % audioInputDeviceChannelCount;
                audioInputChannelIndices[i] = c;
                emit audioInputChannelChanged(i, c);
                c = i % audioOutputDeviceChannelCount;
                audioOutputChannelIndices[i] = c;
                emit audioOutputChannelChanged(i, c);
            }
        } else {
            for (i = 0; i < channels; i++) {
                audioInputChannelIndices[i] = oldAudioInputChannelIndices[i];
                audioOutputChannelIndices[i] = oldAudioOutputChannelIndices[i];
            }
        }
        delete[] oldAudioInputChannelIndices;
        delete[] oldAudioOutputChannelIndices;
    }
}

void
Sampler::setErrorState(const char *message)
{
    errorMessage = message;
    state = STATE_ERROR;
}

void
Sampler::setMIDIDeviceIndex(int index)
{
    assert(! active);
    assert((index >= 0) && (index < midiDevices.count()));
    if (index != midiDeviceIndex) {
        midiDeviceIndex = index;
        emit midiDeviceIndexChanged(index);
    }
}

void
Sampler::setSampleRate(synthclone::SampleRate sampleRate)
{
    assert(! active);
    assert((sampleRate >= synthclone::SAMPLE_RATE_MINIMUM) &&
           (sampleRate <= synthclone::SAMPLE_RATE_MAXIMUM));
    if (sampleRate != this->sampleRate) {
        this->sampleRate = sampleRate;
        emit sampleRateChanged(sampleRate);
    }
}

void
Sampler::startJob(const synthclone::SamplerJob &job,
                  synthclone::SampleStream &stream)
{
    assert(idle);
    assert(stream.getChannels() == channels);
    assert(stream.getSampleRate() == sampleRate);
    Command command;
    float *sampleBuffer;
    synthclone::SampleFrameCount sampleFrames;
    synthclone::SampleRate sampleRate = stream.getSampleRate();
    const synthclone::Zone *zone = job.getZone();
    if (job.getType() == synthclone::SamplerJob::TYPE_SAMPLE) {
        synthclone::SampleFrameCount releaseFrames =
            zone->getReleaseTime() * sampleRate;
        sampleFrames = zone->getSampleTime() * sampleRate;
        sampleBuffer = new float[channels * sampleFrames];
        emit statusChanged(tr("Sampling ..."));
        command.totalReleaseFrames = releaseFrames;
    } else {
        sampleFrames = stream.getFrames();
        sampleBuffer = new float[channels * sampleFrames];
        synthclone::SampleInputStream *inputStream =
            qobject_cast<synthclone::SampleInputStream *>(&stream);
        synthclone::SampleFrameCount count =
            inputStream->read(sampleBuffer, sampleFrames);
        assert(count == sampleFrames);
        emit statusChanged(tr("Playing sample ..."));
    }
    command.job = &job;
    command.sampleBuffer = sampleBuffer;
    command.stream = &stream;
    command.totalSampleFrames = sampleFrames;
    idle = false;
    sendCommand(command);
}

void
Sampler::updateCommandState()
{
    if (commandBuffer.isReadable()) {
        Command command;
        commandBuffer.read(command);
        assert(! command.job);
        if (! aborted) {
            state = STATE_ABORT;
        }
    } else {
        assert(command.stream->getSampleRate() == sampleRate);
    }
}
