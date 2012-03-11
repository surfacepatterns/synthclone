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

#ifndef __SAMPLER_H__
#define __SAMPLER_H__

#include <portaudio.h>
#include <portmidi.h>

#include <QtCore/QList>

#include <synthclone/sampler.h>
#include <synthclone/semaphore.h>

#include "eventthread.h"
#include "midithread.h"
#include "ringbuffer.h"

class Sampler: public synthclone::Sampler {

    Q_OBJECT

    friend class EventThread;
    friend class MIDIThread;

public:

    explicit
    Sampler(const QString &name, synthclone::SampleChannelCount channels,
            synthclone::SampleRate sampleRate, QObject *parent=0);

    ~Sampler();

    void
    abortJob();

    void
    activate();

    void
    deactivate();

    int
    getAudioAPICount() const;

    int
    getAudioAPIIndex() const;

    QString
    getAudioAPIName(int index) const;

    synthclone::SampleChannelCount
    getAudioInputChannel(synthclone::SampleChannelCount channel) const;

    synthclone::SampleChannelCount
    getAudioInputDeviceChannelCount() const;

    synthclone::SampleChannelCount
    getAudioInputDeviceChannelCount(int index) const;

    int
    getAudioInputDeviceCount() const;

    int
    getAudioInputDeviceIndex() const;

    QString
    getAudioInputDeviceName(int index) const;

    synthclone::SampleChannelCount
    getAudioOutputChannel(synthclone::SampleChannelCount channel) const;

    synthclone::SampleChannelCount
    getAudioOutputDeviceChannelCount() const;

    synthclone::SampleChannelCount
    getAudioOutputDeviceChannelCount(int index) const;

    int
    getAudioOutputDeviceCount() const;

    int
    getAudioOutputDeviceIndex() const;

    QString
    getAudioOutputDeviceName(int index) const;

    synthclone::SampleChannelCount
    getChannels() const;

    int
    getMIDIDeviceCount() const;

    int
    getMIDIDeviceIndex() const;

    QString
    getMIDIDeviceName(int index) const;

    synthclone::SampleRate
    getSampleRate() const;

    bool
    isActive() const;

public slots:

    void
    setAudioAPIIndex(int index);

    void
    setAudioInputChannel(synthclone::SampleChannelCount channel,
                         synthclone::SampleChannelCount deviceChannel);

    void
    setAudioInputDeviceIndex(int index);

    void
    setAudioOutputChannel(synthclone::SampleChannelCount channel,
                          synthclone::SampleChannelCount deviceChannel);

    void
    setAudioOutputDeviceIndex(int index);

    void
    setChannels(synthclone::SampleChannelCount channels);

    void
    setMIDIDeviceIndex(int index);

    void
    setSampleRate(synthclone::SampleRate sampleRate);

    void
    startJob(const synthclone::SamplerJob &job,
             synthclone::SampleStream &stream);

signals:

    void
    audioAPIIndexChanged(int index);

    void
    audioInputChannelChanged(synthclone::SampleChannelCount channel,
                             synthclone::SampleChannelCount deviceChannel);

    void
    audioInputDeviceAdded(int index, const QString &name);

    void
    audioInputDeviceChannelCountChanged(synthclone::SampleChannelCount count);

    void
    audioInputDeviceIndexChanged(int index);

    void
    audioOutputChannelChanged(synthclone::SampleChannelCount channel,
                              synthclone::SampleChannelCount deviceChannel);

    void
    audioOutputDeviceAdded(int index, const QString &name);

    void
    audioOutputDeviceChannelCountChanged(synthclone::SampleChannelCount count);

    void
    audioOutputDeviceIndexChanged(int index);

    void
    channelsChanged(synthclone::SampleChannelCount channels);

    void
    midiDeviceIndexChanged(int index);

    void
    midiError(const QString &message);

    void
    sampleRateChanged(synthclone::SampleRate sampleRate);

private:

    struct AudioDeviceData {
        PaDeviceIndex index;
        const PaDeviceInfo *info;
    };

    typedef QList<AudioDeviceData> AudioDeviceDataList;

    struct AudioAPIData {
        int defaultInputDeviceIndex;
        int defaultOutputDeviceIndex;
        const PaHostApiInfo *info;
        PaHostApiIndex index;
        AudioDeviceDataList inputDevices;
        AudioDeviceDataList outputDevices;
    };

    typedef QList<AudioAPIData> AudioAPIDataList;

    struct MIDIDeviceData {
        PmDeviceID id;
        const PmDeviceInfo *info;
    };

    typedef QList<MIDIDeviceData> MIDIDeviceDataList;

    struct Command {
        const synthclone::SamplerJob *job;
        float *sampleBuffer;
        synthclone::SampleStream *stream;
        synthclone::SampleFrameCount totalReleaseFrames;
        synthclone::SampleFrameCount totalSampleFrames;
    };

    struct Event {

        enum Type {
            TYPE_ABORTED,
            TYPE_COMPLETE,
            TYPE_ERROR,
            TYPE_INPUT_OVERFLOW,
            TYPE_INPUT_UNDERFLOW,
            TYPE_OUTPUT_OVERFLOW,
            TYPE_OUTPUT_UNDERFLOW,
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
    handleProcessEvent(const void *input, void *output, unsigned long frames,
                       const PaStreamCallbackTimeInfo *timeInfo,
                       PaStreamCallbackFlags statusFlags, void *userData);

    // Members

    void
    copyData(const float *input, float *output, unsigned long totalFrames,
             unsigned long startFrame=0);

    const AudioDeviceData &
    getAudioInputDeviceData() const;

    const AudioDeviceData &
    getAudioInputDeviceData(int index) const;

    const AudioDeviceData &
    getAudioOutputDeviceData() const;

    const AudioDeviceData &
    getAudioOutputDeviceData(int index) const;

    int
    handleProcessEvent(const float *input, float *output, unsigned long frames,
                       PaStreamCallbackFlags statusFlags);

    void
    initializeOutputFrame(float *output, synthclone::SampleFrameCount offset);

    void
    monitorEvents();

    synthclone::SampleFrameCount
    playData(const float *input, float *output, unsigned long totalFrames);

    synthclone::SampleFrameCount
    recordData(const float *input, float *output, unsigned long totalFrames);

    void
    runMIDI();

    void
    sendCommand(const Command &command);

    bool
    sendErrorEvent();

    bool
    sendEvent(const Event &event);

    bool
    sendJobFinalizationEvent(Event::Type type);

    bool
    sendMIDIMessage(synthclone::MIDIData status, synthclone::MIDIData data1,
                    synthclone::MIDIData data2=synthclone::MIDI_VALUE_NOT_SET);

    void
    sendProgressEvent(float progress);

    bool
    sendSimpleEvent(Event::Type type);

    void
    setErrorState(const char *message);

    void
    updateCommandState();

    bool aborted;
    bool active;
    AudioAPIDataList audioAPIs;
    int audioAPIIndex;
    synthclone::SampleChannelCount *audioInputChannelIndices;
    synthclone::SampleChannelCount audioInputDeviceChannelCount;
    int audioInputDeviceIndex;
    synthclone::SampleChannelCount *audioOutputChannelIndices;
    synthclone::SampleChannelCount audioOutputDeviceChannelCount;
    int audioOutputDeviceIndex;
    PaStream *audioStream;
    synthclone::SampleChannelCount channels;
    Command command;
    RingBuffer<Command> commandBuffer;
    synthclone::SampleFrameCount currentFrame;
    const char *errorMessage;
    RingBuffer<Event> eventBuffer;
    synthclone::Semaphore eventSemaphore;
    EventThread eventThread;
    bool idle;
    RingBuffer<PmMessage> midiBuffer;
    MIDIDeviceDataList midiDevices;
    int midiDeviceIndex;
    synthclone::Semaphore midiSemaphore;
    PortMidiStream *midiStream;
    MIDIThread midiThread;
    int progress;
    synthclone::SampleRate sampleRate;
    State state;

};

#endif
