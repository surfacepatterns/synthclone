/*
 * libsynthclone_jack - JACK Audio Connection Kit sampler plugin for
 * `synthclone`
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

#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include <jack/jack.h>
#include <jack/ringbuffer.h>
#include <jack/session.h>

#include <QtCore/QDir>
#include <QtCore/QMutex>

#include <synthclone/sampler.h>

#include "eventthread.h"
#include "semaphore.h"

class Sampler: public synthclone::Sampler {

    Q_OBJECT

    friend class EventThread;

public:

    explicit
    Sampler(const QString &name, const char *sessionId=0, QObject *parent=0);

    ~Sampler();

    void
    abortJob();

    void
    activate(synthclone::SampleChannelCount channels);

    void
    deactivate();

    synthclone::SampleChannelCount
    getChannelCount() const;

    synthclone::SampleRate
    getSampleRate() const;

    void
    startJob(const synthclone::SamplerJob &job,
             synthclone::SampleStream &stream);

signals:

    void
    fatalError(const QString &message);

    void
    sampleRateChanged();

    void
    sessionEvent(jack_client_t *client, jack_session_event_t *event);

private:

    struct Command {
        const synthclone::SamplerJob *job;
        jack_default_audio_sample_t **sampleBuffers;
        synthclone::SampleStream *stream;
        jack_nframes_t totalReleaseFrames;
        jack_nframes_t totalSampleFrames;
    };

    struct PriorityEvent {

        enum Type {
            TYPE_SAMPLE_RATE_CHANGE,
            TYPE_SESSION,
            TYPE_SHUTDOWN,
            TYPE_TERMINATE
        };

        Type type;

        union {
            const char *message;
            jack_session_event_t *session;
        } data;

    };

    struct ProcessEvent {

        enum Type {
            TYPE_ABORTED,
            TYPE_COMPLETE,
            TYPE_ERROR,
            TYPE_PROGRESS
        };

        Type type;

        union {
            struct {
                Command command;
                const char *message;
            } error;
            Command command;
            float progress;
        } data;
    };

    enum State {
        STATE_ABORT,
        STATE_COMPLETED,
        STATE_ERROR,
        STATE_IDLE,
        STATE_PLAY,
        STATE_SAMPLE,
        STATE_SAMPLE_SEND_PRE_MIDI,
        STATE_SAMPLE_RELEASE
    };

    // Static wrappers

    static int
    handleProcessEvent(jack_nframes_t frames, void *ptr);

    static int
    handleSampleRateChangeEvent(jack_nframes_t frames, void *ptr);

    static void
    handleSessionEvent(jack_session_event_t *sessionEvent, void *ptr);

    static void
    handleShutdownEvent(jack_status_t code, const char *reason, void *ptr);

    // Members

    void
    clean();

    void
    closePort(jack_port_t *port);

    void
    closePorts();

    const char *
    getErrorMessage(jack_status_t status) const;

    int
    handleProcessEvent(jack_nframes_t frames);

    int
    handleSampleRateChangeEvent(jack_nframes_t frames);

    void
    handleSessionEvent(jack_session_event_t *sessionEvent);

    void
    handleShutdownEvent(jack_status_t code, const char *reason);

    jack_port_t **
    initializeAudioPorts(const QString &prefix, JackPortFlags flags,
                         synthclone::SampleChannelCount channels);

    bool
    isCommandAborted();

    void
    monitorEvents();

    jack_port_t *
    openPort(const char *name, const char *type, JackPortFlags flags);

    void
    sendCommand(const Command &command);

    bool
    sendJobFinalizationEvent(ProcessEvent::Type type);

    bool
    sendMIDIMessage(void *midiBuffer, synthclone::MIDIData status,
                    synthclone::MIDIData data1,
                    synthclone::MIDIData data2=synthclone::MIDI_VALUE_NOT_SET);

    void
    sendPriorityEvent(const PriorityEvent &event);

    bool
    sendProcessErrorEvent();

    bool
    sendProcessEvent(const ProcessEvent &event);

    void
    sendProgressEvent(float progress);

    void
    setProcessErrorState(const char *message);

    void
    updateCommandState();

    bool aborted;
    volatile bool active;
    QMutex activeMutex;
    synthclone::SampleChannelCount channels;
    jack_client_t *client;
    Command command;
    jack_ringbuffer_t *commandBuffer;
    jack_nframes_t currentFrame;
    const char *errorMessage;
    Semaphore eventSemaphore;
    EventThread eventThread;
    bool idle;
    jack_port_t **inputPorts;
    jack_port_t *midiPort;
    jack_port_t **monitorPorts;
    jack_port_t **outputPorts;
    jack_ringbuffer_t *priorityEventBuffer;
    jack_ringbuffer_t *processEventBuffer;
    int progress;
    QList<jack_port_t *> registeredPorts;
    State state;

};

#endif
