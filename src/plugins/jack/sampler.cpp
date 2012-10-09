/*
 * libsynthclone_jack - JACK Audio Connection Kit sampler plugin for
 * `synthclone`
 * Copyright (C) 2011-2012 Devin Anderson
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
#include <QtCore/QLocale>
#include <QtCore/QMutexLocker>
#include <QtCore/QScopedArrayPointer>
#include <QtCore/QScopedPointer>

#include <jack/midiport.h>

#include <synthclone/error.h>
#include <synthclone/sampleinputstream.h>
#include <synthclone/sampleoutputstream.h>

#include "sampler.h"

// Static data

static const char *ERROR_BACKEND =
    QT_TR_NOOP("A JACK server backend error has occurred");
static const char *ERROR_CLIENT =
    QT_TR_NOOP("Unable to initialize client");
static const char *ERROR_FAILURE =
    QT_TR_NOOP("The operation failed");
static const char *ERROR_INTERNAL_CLIENT =
    QT_TR_NOOP("Unable to load internal client");
static const char *ERROR_INVALID_OPTION =
    QT_TR_NOOP("Operation contained an invalid or unsupported option");
static const char *ERROR_MIDI_EVENT_RESERVE =
    QT_TR_NOOP("Failed to reserve event in JACK MIDI buffer");
static const char *ERROR_NAME_NOT_UNIQUE =
    QT_TR_NOOP("The given client name is not unique");
static const char *ERROR_NO_SUCH_CLIENT =
    QT_TR_NOOP("The requested client does not exist");
static const char *ERROR_SAMPLE_RATE =
    QT_TR_NOOP("JACK's sample rate differs from the sample's sample rate");
static const char *ERROR_SERVER_COMMUNICATION =
    QT_TR_NOOP("Communication error with JACK server");
static const char *ERROR_SERVER_FAILURE =
    QT_TR_NOOP("Unable to connect to the JACK server");
static const char *ERROR_SHM =
    QT_TR_NOOP("Unable to access shared memory");
static const char *ERROR_UNKNOWN =
    QT_TR_NOOP("An unspecified JACK server error has occurred");
static const char *ERROR_VERSION =
    QT_TR_NOOP("The JACK server protocol doesn't match the client protocol");
static const char *ERROR_ZOMBIE =
    QT_TR_NOOP("The JACK server has zombified this JACK client");

struct ClientDestructor {

    static void
    cleanup(jack_client_t *client)
    {
        if (client) {
            if (jack_client_close(client)) {
                qWarning() << "Failed to close JACK client";
            }
        }
    }

};

struct RingbufferDestructor {

    static void
    cleanup(jack_ringbuffer_t *ringbuffer)
    {
        if (ringbuffer) {
            jack_ringbuffer_free(ringbuffer);
        }
    }

};

int
Sampler::handleProcessEvent(jack_nframes_t frames, void *ptr)
{
    Sampler *sampler = static_cast<Sampler *>(ptr);
    assert(sampler);
    return sampler->handleProcessEvent(frames);
}

int
Sampler::handleSampleRateChangeEvent(jack_nframes_t frames, void *ptr)
{
    Sampler *sampler = static_cast<Sampler *>(ptr);
    assert(sampler);
    return sampler->handleSampleRateChangeEvent(frames);
}

void
Sampler::handleSessionEvent(jack_session_event_t *sessionEvent, void *ptr)
{
    Sampler *sampler = static_cast<Sampler *>(ptr);
    assert(sampler);
    return sampler->handleSessionEvent(sessionEvent);
}

void
Sampler::handleShutdownEvent(jack_status_t code, const char *reason, void *ptr)
{
    Sampler *sampler = static_cast<Sampler *>(ptr);
    assert(sampler);
    sampler->handleShutdownEvent(code, reason);
}

// Class definition

Sampler::Sampler(const QString &name, const char *sessionId, QObject *parent):
    synthclone::Sampler(name, parent),
    eventThread(this)
{
    QByteArray jackNameByteArray = tr("synthclone").toLocal8Bit();
    const char *jackName = jackNameByteArray.constData();
    jack_status_t status;
    client = sessionId ?
        jack_client_open(jackName, JackSessionID, &status, sessionId) :
        jack_client_open(jackName, JackNullOption, &status);
    if (! client) {
        throw synthclone::Error(tr(getErrorMessage(status)));
    }
    QScopedPointer<jack_client_t, ClientDestructor> clientPtr(client);

    // The command buffer needs to be large enough to hold a job command and an
    // abort command.
    commandBuffer = jack_ringbuffer_create((sizeof(Command) * 2) + 1);
    if (! commandBuffer) {
        throw std::bad_alloc();
    }
    QScopedPointer<jack_ringbuffer_t, RingbufferDestructor>
        commandBufferPtr(commandBuffer);

    // The priority event buffer needs to be large enough to hold a sampler
    // event, a session event, a shutdown event, and a terminate thread event.
    priorityEventBuffer =
        jack_ringbuffer_create((sizeof(PriorityEvent) * 4) + 1);
    if (! priorityEventBuffer) {
        throw std::bad_alloc();
    }
    QScopedPointer<jack_ringbuffer_t, RingbufferDestructor>
        priorityEventBufferPtr(priorityEventBuffer);

    // The process event buffer needs to be large enough to hold one event.
    processEventBuffer = jack_ringbuffer_create(sizeof(ProcessEvent) + 1);
    if (! processEventBuffer) {
        throw std::bad_alloc();
    }
    QScopedPointer<jack_ringbuffer_t, RingbufferDestructor>
        processEventBufferPtr(processEventBuffer);

    if (jack_set_process_callback(client, handleProcessEvent, this)) {
        throw synthclone::Error(tr("failed to set JACK process callback"));
    }
    if (jack_set_sample_rate_callback(client, handleSampleRateChangeEvent,
                                      this)) {
        throw synthclone::Error(tr("failed to set JACK sample rate callback"));
    }
    if (jack_set_session_callback(client, handleSessionEvent, this)) {
        throw synthclone::Error(tr("failed to set JACK session callback"));
    }
    jack_on_info_shutdown(client, handleShutdownEvent, this);

    active = false;
    clientPtr.take();
    commandBufferPtr.take();
    priorityEventBufferPtr.take();
    processEventBufferPtr.take();
}

Sampler::~Sampler()
{
    if (active) {
        QMutexLocker locker(&activeMutex);
        if (jack_client_close(client)) {
            qWarning() << "Failed to close JACK client";
        }
        active = false;
        clean();
    }

    // Terminate the event thread.
    PriorityEvent event;
    event.type = PriorityEvent::TYPE_TERMINATE;
    sendPriorityEvent(event);
    eventThread.wait();

    jack_ringbuffer_free(commandBuffer);
    jack_ringbuffer_free(priorityEventBuffer);
    jack_ringbuffer_free(processEventBuffer);
}

void
Sampler::abortJob()
{
    Command command;
    command.job = 0;
    emit statusChanged(tr("Aborting ..."));
    sendCommand(command);
}

void
Sampler::activate(synthclone::SampleChannelCount channels)
{
    try {
        inputPorts = initializeAudioPorts(tr("input"), JackPortIsInput,
                                          channels);
        QScopedArrayPointer<jack_port_t *> inputPortsPtr(inputPorts);

        monitorPorts = initializeAudioPorts(tr("monitor"), JackPortIsOutput,
                                            channels);
        QScopedArrayPointer<jack_port_t *> monitorPortsPtr(monitorPorts);

        outputPorts = initializeAudioPorts(tr("output"), JackPortIsOutput,
                                           channels);
        QScopedArrayPointer<jack_port_t *> outputPortsPtr(outputPorts);

        QByteArray midiPortName = tr("MIDI").toLocal8Bit();
        midiPort = openPort(midiPortName.constData(), JACK_DEFAULT_MIDI_TYPE,
                            JackPortIsOutput);

        active = true;
        this->channels = channels;
        idle = true;
        state = STATE_IDLE;
        if (jack_activate(client)) {
            active = false;
            throw synthclone::Error(tr("failed to activate JACK client"));
        }
        eventThread.start();

        inputPortsPtr.take();
        monitorPortsPtr.take();
        outputPortsPtr.take();
    } catch (...) {
        closePorts();
        throw;
    }
}

void
Sampler::clean()
{
    delete[] inputPorts;
    delete[] monitorPorts;
    delete[] outputPorts;
}

void
Sampler::closePort(jack_port_t *port)
{
    jack_port_unregister(client, port);
    registeredPorts.removeOne(port);
}

void
Sampler::closePorts()
{
    for (int i = registeredPorts.count() - 1; i >= 0; i--) {
        closePort(registeredPorts[i]);
    }
}

synthclone::SampleChannelCount
Sampler::getChannelCount() const
{
    return channels;
}

const char *
Sampler::getErrorMessage(jack_status_t status) const
{
    return (status & JackVersionError) ? ERROR_VERSION :
        (status & JackShmFailure) ? ERROR_SHM :
        (status & JackClientZombie) ? ERROR_ZOMBIE :
        (status & JackNoSuchClient) ? ERROR_NO_SUCH_CLIENT :
        (status & JackServerFailed) ? ERROR_SERVER_FAILURE :
        (status & JackServerError) ? ERROR_SERVER_COMMUNICATION :
        (status & JackLoadFailure) ? ERROR_INTERNAL_CLIENT :
        (status & JackInitFailure) ? ERROR_CLIENT :
        (status & JackNameNotUnique) ? ERROR_NAME_NOT_UNIQUE :
        (status & JackInvalidOption) ? ERROR_INVALID_OPTION :
        (status & JackBackendError) ? ERROR_BACKEND :
        (status & JackFailure) ? ERROR_FAILURE : ERROR_UNKNOWN;
}

synthclone::SampleRate
Sampler::getSampleRate() const
{
    return static_cast<synthclone::SampleRate>(jack_get_sample_rate(client));
}

int
Sampler::handleProcessEvent(jack_nframes_t frames)
{
    size_t copySize = frames * sizeof(jack_default_audio_sample_t);
    for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
        memcpy(jack_port_get_buffer(monitorPorts[i], frames),
               jack_port_get_buffer(inputPorts[i], frames), copySize);
    }
    void *midiBuffer = jack_port_get_buffer(midiPort, frames);
    jack_midi_clear_buffer(midiBuffer);

    jack_nframes_t copyFrames;
    const synthclone::SamplerJob *job;
    synthclone::MIDIData midiChannel;
    jack_nframes_t nextFrame;
    float **sampleBuffers;
    jack_nframes_t totalFrames;
    bool writeSilence = true;
    const synthclone::Zone *zone;

    switch (state) {

    // Waiting for commands
    case STATE_IDLE:
    idle:
        while (jack_ringbuffer_read_space(commandBuffer) >= sizeof(Command)) {
            jack_ringbuffer_read(commandBuffer, (char *) &command,
                                 sizeof(Command));
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
        nextFrame = currentFrame + frames;
        sampleBuffers = command.sampleBuffers;
        totalFrames = command.totalSampleFrames;
        writeSilence = false;
        if (nextFrame >= totalFrames) {
            copyFrames = totalFrames - currentFrame;
            copySize = copyFrames * sizeof(jack_default_audio_sample_t);
            for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
                jack_default_audio_sample_t *outputBuffer =
                    static_cast<jack_default_audio_sample_t *>
                    (jack_port_get_buffer(outputPorts[i], frames));
                memcpy(outputBuffer, sampleBuffers[i] + currentFrame, copySize);
                for (synthclone::SampleFrameCount j = copyFrames;
                     j < totalFrames; j++) {
                    outputBuffer[j] = 0.0;
                }
            }
            sendProgressEvent(1.0);
            state = STATE_COMPLETED;
            goto completed;
        }
        copySize = frames * sizeof(jack_default_audio_sample_t);
        for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
            memcpy(jack_port_get_buffer(outputPorts[i], frames),
                   sampleBuffers[i] + currentFrame, copySize);
        }
        currentFrame = nextFrame;
        sendProgressEvent(static_cast<float>(nextFrame) / totalFrames);
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
                if (! sendMIDIMessage(midiBuffer, 0xb0 | midiChannel,
                                      iter.key(), iter.value())) {
                    goto error;
                }
            }
            synthclone::MIDIData note = zone->getNote();
            if (! sendMIDIMessage(midiBuffer, 0x90 | midiChannel, note,
                                  zone->getVelocity())) {
                goto error;
            }
            synthclone::MIDIData pressure = zone->getChannelPressure();
            if (pressure != synthclone::MIDI_VALUE_NOT_SET) {
                if (! sendMIDIMessage(midiBuffer, 0xb0 | midiChannel,
                                      pressure)) {
                    goto error;
                }
            }
            synthclone::MIDIData aftertouch = zone->getAftertouch();
            if (aftertouch != synthclone::MIDI_VALUE_NOT_SET) {
                if (! sendMIDIMessage(midiBuffer, 0xa0 | midiChannel, note,
                                      aftertouch)) {
                    goto error;
                }
            }
        }
        // XXX: Skip frames if there's latency.  How do we determine that?
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
        nextFrame = currentFrame + frames;
        sampleBuffers = command.sampleBuffers;
        totalFrames = command.totalSampleFrames;
        if (nextFrame < totalFrames) {
            copySize = frames * sizeof(jack_default_audio_sample_t);
            for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
                memcpy(sampleBuffers[i] + currentFrame,
                       jack_port_get_buffer(inputPorts[i], frames), copySize);
            }
            currentFrame = nextFrame;
            sendProgressEvent(static_cast<float>(currentFrame) /
                              (totalFrames + command.totalReleaseFrames));
            break;
        }
        copyFrames = totalFrames - currentFrame;
        copySize = copyFrames * sizeof(jack_default_audio_sample_t);
        for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
            memcpy(sampleBuffers[i] + currentFrame,
                   jack_port_get_buffer(inputPorts[i], frames), copySize);
        }
        sendProgressEvent(static_cast<float>(totalFrames) /
                          (totalFrames + command.totalReleaseFrames));
    sampleSendNoteOff:
        zone = command.job->getZone();
        if (! sendMIDIMessage(midiBuffer, 0x80 | (zone->getChannel() - 1),
                              zone->getNote(), zone->getVelocity())) {
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
        totalFrames = command.totalReleaseFrames;
        if (currentFrame < totalFrames) {
            jack_nframes_t totalSampleFrames = command.totalSampleFrames;
            sendProgressEvent((static_cast<float>(currentFrame) +
                               totalSampleFrames) /
                              (totalFrames + totalSampleFrames));
            break;
        }
        sendProgressEvent(1.0);

        // Send MIDI messages to turn sound off and reset controllers.
        zone = command.job->getZone();
        midiChannel = zone->getChannel() - 1;
        if (! sendMIDIMessage(midiBuffer, 0xb0 | midiChannel, 0x78, 0)) {
            state = STATE_ERROR;
            goto error;
        }
        if (! sendMIDIMessage(midiBuffer, 0xb0 | midiChannel, 0x79, 0)) {
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
        if (sendJobFinalizationEvent(ProcessEvent::TYPE_COMPLETE)) {
            goto idle;
        }
        break;

    case STATE_ABORT:
    abort:
        if (sendJobFinalizationEvent(ProcessEvent::TYPE_ABORTED)) {
            goto idle;
        }
        break;

    case STATE_ERROR:
    error:
        if (sendProcessErrorEvent()) {
            state = STATE_IDLE;
            goto idle;
        }
        break;

    }
    if (writeSilence) {
        void *firstBuffer = jack_port_get_buffer(outputPorts[0], frames);
        jack_default_audio_sample_t *firstSampleBuffer =
            static_cast<jack_default_audio_sample_t *>(firstBuffer);
        for (jack_nframes_t i = 0; i < frames; i++) {
            firstSampleBuffer[i] = 0.0;
        }
        for (synthclone::SampleChannelCount i = 1; i < channels; i++) {
            memcpy(jack_port_get_buffer(outputPorts[i], frames), firstBuffer,
                   frames * sizeof(jack_default_audio_sample_t));
        }
    }
    return 0;
}

void
Sampler::handleSessionEvent(jack_session_event_t *sessionEvent)
{
    PriorityEvent event;
    event.data.session = sessionEvent;
    event.type = PriorityEvent::TYPE_SESSION;
    sendPriorityEvent(event);
}

int
Sampler::handleSampleRateChangeEvent(jack_nframes_t /*sampleRate*/)
{
    PriorityEvent event;
    event.type = PriorityEvent::TYPE_SAMPLE_RATE_CHANGE;
    sendPriorityEvent(event);
    return 0;
}

void
Sampler::handleShutdownEvent(jack_status_t status, const char */*reason*/)
{
    PriorityEvent event;
    event.data.message = getErrorMessage(status);
    event.type = PriorityEvent::TYPE_SHUTDOWN;
    sendPriorityEvent(event);
}

jack_port_t **
Sampler::initializeAudioPorts(const QString &prefix, JackPortFlags flags,
                              synthclone::SampleChannelCount channels)
{
    jack_port_t **ports = new jack_port_t *[channels];
    QScopedArrayPointer<jack_port_t *> portsPtr(ports);
    QString portNameTemplate = tr("%1-%2", "portNameTemplate");
    for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
        QByteArray portName =
            portNameTemplate.arg(prefix).arg(i + 1).toLocal8Bit();
        ports[i] = openPort(portName.constData(), JACK_DEFAULT_AUDIO_TYPE,
                            flags);
    }
    portsPtr.take();
    return ports;
}

void
Sampler::monitorEvents()
{
    Command *command;
    const synthclone::SamplerJob *job;
    for (;;) {
        eventSemaphore.wait();

        // Check for priority event first.
        if (jack_ringbuffer_read_space(priorityEventBuffer) >=
            sizeof(PriorityEvent)) {
            PriorityEvent event;
            jack_ringbuffer_read(priorityEventBuffer, (char *) &event,
                                 sizeof(PriorityEvent));
            switch (event.type) {
            case PriorityEvent::TYPE_SAMPLE_RATE_CHANGE:
                emit sampleRateChanged();
                continue;
            case PriorityEvent::TYPE_SESSION:
                emit sessionEvent(client, event.data.session);
                continue;
            case PriorityEvent::TYPE_SHUTDOWN:
                // We can't call jack_deactivate() if the server is shutdown
                // because it causes a segfault.  So, we do cleanup here.
                {
                    QMutexLocker locker(&activeMutex);
                    if (active) {
                        active = false;
                        clean();
                    }
                }
                emit fatalError(event.data.message);
                continue;
            case PriorityEvent::TYPE_TERMINATE:
                return;
            default:
                assert(false);
            }
        }

        // Handle process events.

        assert(jack_ringbuffer_read_space(processEventBuffer) >=
               sizeof(ProcessEvent));
        ProcessEvent event;
        jack_ringbuffer_read(processEventBuffer, (char *) &event,
                             sizeof(ProcessEvent));
        switch (event.type) {
        case ProcessEvent::TYPE_ABORTED:
            idle = true;
            emit statusChanged(tr("Idle."));
            emit jobAborted();
            command = &(event.data.command);
            break;
        case ProcessEvent::TYPE_COMPLETE:
            command = &(event.data.command);
            job = command->job;
            if (job->getType() == synthclone::SamplerJob::TYPE_SAMPLE) {
                synthclone::SampleOutputStream *stream =
                    qobject_cast<synthclone::SampleOutputStream *>
                    (command->stream);
                float **sampleBuffers = command->sampleBuffers;
                float *streamBuffer = new float[channels];
                for (jack_nframes_t i = 0; i < command->totalSampleFrames;
                     i++) {
                    for (synthclone::SampleChannelCount j = 0; j < channels;
                         j++) {
                        streamBuffer[j] = sampleBuffers[j][i];
                    }
                    stream->write(streamBuffer, 1);
                }
                delete[] streamBuffer;
            }
            idle = true;
            emit statusChanged(tr("Idle."));
            emit jobCompleted();
            emit progressChanged(0.0);
            break;
        case ProcessEvent::TYPE_ERROR:
            idle = true;
            emit statusChanged(tr("Idle."));
            emit jobError(event.data.error.message);
            command = &(event.data.error.command);
            break;
        case ProcessEvent::TYPE_PROGRESS:
            emit progressChanged(event.data.progress);
            continue;
        default:
            assert(false);
        }
        for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
            delete[] command->sampleBuffers[i];
        }
        delete[] command->sampleBuffers;
    }
}

jack_port_t *
Sampler::openPort(const char *name, const char *type, JackPortFlags flags)
{
    jack_port_t *port = jack_port_register(client, name, type, flags, 0);
    if (! port) {
        throw synthclone::Error(tr("failed to register JACK port '%1'").
                                arg(name));
    }
    registeredPorts.append(port);
    return port;
}

void
Sampler::sendCommand(const Command &command)
{
    size_t commandSize = sizeof(Command);
    assert(jack_ringbuffer_write_space(commandBuffer) >= commandSize);
    jack_ringbuffer_write(commandBuffer, (const char *) (&command),
                          commandSize);
}

bool
Sampler::sendJobFinalizationEvent(ProcessEvent::Type type)
{
    ProcessEvent event;
    memcpy(static_cast<void *>(&(event.data.command)),
           static_cast<void *>(&command), sizeof(Command));
    event.type = type;
    bool result = sendProcessEvent(event);
    if (result) {
        state = STATE_IDLE;
    }
    return result;
}

bool
Sampler::sendMIDIMessage(void *midiBuffer, synthclone::MIDIData status,
                         synthclone::MIDIData data1, synthclone::MIDIData data2)
{
    assert(data1 < 0x80);
    size_t size;
    if (data2 == synthclone::MIDI_VALUE_NOT_SET) {
        size = 2;
    } else {
        assert(data2 < 0x80);
        size = 3;
    }
    jack_midi_data_t *event = jack_midi_event_reserve(midiBuffer, 0, size);
    if (! event) {
        jack_midi_clear_buffer(midiBuffer);
        setProcessErrorState(ERROR_MIDI_EVENT_RESERVE);
        return false;
    }
    event[0] = status;
    event[1] = data1;
    if (size == 3) {
        event[2] = data2;
    }
    return true;
}

void
Sampler::sendPriorityEvent(const PriorityEvent &event)
{
    assert(jack_ringbuffer_write_space(priorityEventBuffer) >=
           sizeof(PriorityEvent));
    jack_ringbuffer_write(priorityEventBuffer, (const char *) (&event),
                          sizeof(PriorityEvent));
    eventSemaphore.post();
}

bool
Sampler::sendProcessErrorEvent()
{
    ProcessEvent event;
    memcpy(static_cast<void *>(&(event.data.error.command)),
           static_cast<void *>(&command), sizeof(Command));
    event.data.error.message = errorMessage;
    event.type = ProcessEvent::TYPE_ERROR;
    return sendProcessEvent(event);
}

bool
Sampler::sendProcessEvent(const ProcessEvent &event)
{
    bool result = jack_ringbuffer_write_space(processEventBuffer) >=
        sizeof(ProcessEvent);
    if (result) {
        jack_ringbuffer_write(processEventBuffer, (const char *) (&event),
                              sizeof(ProcessEvent));
        eventSemaphore.post();
    }
    return result;
}

void
Sampler::sendProgressEvent(float progress)
{
    ProcessEvent event;
    event.data.progress = progress;
    event.type = ProcessEvent::TYPE_PROGRESS;
    sendProcessEvent(event);
}

void
Sampler::setProcessErrorState(const char *message)
{
    errorMessage = message;
    state = STATE_ERROR;
}

void
Sampler::startJob(const synthclone::SamplerJob &job,
                  synthclone::SampleStream &stream)
{
    assert(idle);
    assert(stream.getChannels() == channels);
    assert(stream.getSampleRate() == getSampleRate());
    Command command;
    jack_default_audio_sample_t **sampleBuffers;
    jack_nframes_t sampleFrames;
    jack_nframes_t sampleRate = static_cast<jack_nframes_t>
        (stream.getSampleRate());
    const synthclone::Zone *zone = job.getZone();
    if (job.getType() == synthclone::SamplerJob::TYPE_SAMPLE) {
        jack_nframes_t releaseFrames = static_cast<jack_nframes_t>
            (zone->getReleaseTime() * sampleRate);
        sampleFrames = static_cast<jack_nframes_t>
            (zone->getSampleTime() * sampleRate);
        sampleBuffers = new jack_default_audio_sample_t *[channels];
        for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
            sampleBuffers[i] = new jack_default_audio_sample_t[sampleFrames];
        }
        emit statusChanged(tr("Sampling ..."));
        command.totalReleaseFrames = releaseFrames;
    } else {
        sampleFrames = static_cast<jack_nframes_t>(stream.getFrames());
        sampleBuffers = new jack_default_audio_sample_t *[channels];
        for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
            sampleBuffers[i] = new jack_default_audio_sample_t[sampleFrames];
        }
        synthclone::SampleInputStream *inputStream =
            qobject_cast<synthclone::SampleInputStream *>(&stream);
        float *buffer = new float[channels];
        for (synthclone::SampleFrameCount i = 0; i < sampleFrames; i++) {
            synthclone::SampleFrameCount count = inputStream->read(buffer, 1);
            assert(count == 1);
            for (synthclone::SampleChannelCount j = 0; j < channels; j++) {
                sampleBuffers[j][i] = buffer[j];
            }
        }
        delete[] buffer;
        emit statusChanged(tr("Playing sample ..."));
    }
    command.job = &job;
    command.sampleBuffers = sampleBuffers;
    command.stream = &stream;
    command.totalSampleFrames = sampleFrames;
    idle = false;
    sendCommand(command);
}

void
Sampler::updateCommandState()
{
    size_t commandSize = sizeof(Command);
    if (jack_ringbuffer_read_space(commandBuffer) >= commandSize) {
        Command command;
        jack_ringbuffer_read(commandBuffer, (char *) &command, commandSize);
        assert(! command.job);
        if (! aborted) {
            state = STATE_ABORT;
        }
    } else if (command.stream->getSampleRate() != getSampleRate()) {
        setProcessErrorState(ERROR_SAMPLE_RATE);
    }
}
