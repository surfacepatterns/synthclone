/*
 * libsynthclone - a plugin API for `synthclone`
 * Copyright (C) 2011 Devin Anderson
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __SYNTHCLONE_ZONE_H__
#define __SYNTHCLONE_ZONE_H__

#include <QtCore/QMap>

#include <synthclone/sample.h>
#include <synthclone/types.h>

namespace synthclone {

    /**
     * Zone objects contains data about how Sample objects should be acquired
     * from a Sampler.  After a Sample is acquired, a Zone object contains the
     * acquired (dry) Sample.  After effects are applied, a Zone object also
     * contains the wet Sample.
     */

    class Zone: public QObject {

        Q_OBJECT

    public:

        typedef QMap<MIDIData, MIDIData> ControlMap;

        /**
         * A zone's status indicates whether or not the zone is being used
         * within the session.  If the zone's status is not STATUS_NORMAL, then
         * attempts to modify or remove the zone from the session will fail.
         *
         * @par STATUS_NORMAL
         *   The Zone object is not being used by the application.
         *
         * @par STATUS_SAMPLER_JOB_QUEUE
         *   The Zone object is waiting in the SamplerJob queue.  When the
         *   SamplerJob pertaining to the Zone is at the front of the queue, the
         *   Sampler will perform some operation using the Zone.
         *
         * @par STATUS_SAMPLER_PLAYING_DRY_SAMPLE
         *   The Zone object is being used by the Sampler.  The Sampler is
         *   playing the dry Sample registered with the Zone.
         *
         * @par STATUS_SAMPLER_PLAYING_WET_SAMPLE
         *   The Zone object is being used by the Sampler.  The Sampler is
         *   playing the wet Sample registered with the Zone.
         *
         * @par STATUS_SAMPLER_SAMPLING
         *   The Zone object is being used by the Sampler.  The Sampler is
         *   sampling a note based on other information in the Zone.
         *
         * @par STATUS_EFFECT_JOB_QUEUE
         *   The Zone object is waiting in the EffectJob queue.  When the
         *   EffectJob pertaining to the Zone is at the front of the queue, the
         *   registered Effect objects will be applied to the Zone object's dry
         *   sample, and the result will be set as the Zone object's wet sample.
         *
         * @par STATUS_EFFECTS
         *   The registered Effect objects are being applied to the Zone
         *   object's dry sample.
         *
         * @par STATUS_TARGETS
         *   The registered targets are using the Zone to create patches.
         */

        enum Status {
            STATUS_NORMAL = 0,
            STATUS_SAMPLER_JOB_QUEUE = 1,
            STATUS_SAMPLER_PLAYING_DRY_SAMPLE = 2,
            STATUS_SAMPLER_PLAYING_WET_SAMPLE = 3,
            STATUS_SAMPLER_SAMPLING = 4,
            STATUS_EFFECT_JOB_QUEUE = 5,
            STATUS_EFFECTS = 6,
            STATUS_TARGETS = 7
        };

        /**
         * Gets the MIDI polyphonic aftertouch value.  If the polyphonic
         * aftertouch isn't set, then the value will be MIDI_VALUE_NOT_SET.
         * The polyphonic aftertouch value is used by the Sampler to send a
         * MIDI polyphonic aftertouch message before sampling.
         *
         * @returns
         *   The aftertouch value.
         */

        virtual MIDIData
        getAftertouch() const = 0;

        /**
         * Gets the MIDI channel.  The channel will be in the range [1, 16].
         * The channel is the MIDI channel that a Sampler uses when retrieving
         * a Sample for this Zone.
         *
         * @returns
         *   The MIDI channel.
         */

        virtual MIDIData
        getChannel() const = 0;

        /**
         * Gets the MIDI channel pressure value.  If the channel pressure isn't
         * set, then the value will be MIDI_VALUE_NOT_SET.  The channel
         * pressure value is used by the Sampler to send a MIDI channel
         * pressure message before sampling.
         *
         * @returns
         *   The pressure value.
         */

        virtual MIDIData
        getChannelPressure() const = 0;

        /**
         * Gets the internal control map containing all of the current control
         * values.
         *
         * @returns
         *   The map.
         */

        virtual const ControlMap &
        getControlMap() const = 0;

        /**
         * Returns the MIDI value for a control.  If the control isn't set, then
         * the value will be MIDI_VALUE_NOT_SET.  Control values are used by the
         * Sampler to send MIDI control messages before sampling.
         *
         * @param control
         *   The control index.
         *
         * @returns
         *   The control value.
         */

        virtual MIDIData
        getControlValue(MIDIData control) const = 0;

        /**
         * Gets the dry Sample object, which has been retrieved from a Sampler.
         * If a Sample object hasn't been retrieved, then this will be 0.
         *
         * @returns
         *   The Sample object.
         */

        virtual const Sample *
        getDrySample() const = 0;

        /**
         * Gets the MIDI note value.  The note value is used by the Sampler to
         * send MIDI note messages during the sampling process.
         *
         * @returns
         *   The note value.
         */

        virtual MIDIData
        getNote() const = 0;

        /**
         * Gets the release time.  The release time is used by a Sampler to add
         * space between the processing of SamplerJob objects in order to give a
         * note the chance to fade away.  If the release time is set too low,
         * then a sampled note may overlap another sampled note.
         *
         * @returns
         *   The release time.
         */

        virtual SampleTime
        getReleaseTime() const = 0;

        /**
         * Gets the sample time.  The sample time is used by a Sampler to
         * determine how long a note should be sampled.
         *
         * @returns
         *   The sample time.
         */

        virtual SampleTime
        getSampleTime() const = 0;

        /**
         * Gets the Zone object's Zone::Status.  A Zone object's Zone::Status
         * can be used to determine how the Zone is currently being used by
         * `synthclone`.
         *
         * @returns
         *   The Zone object's Zone::Status.
         */

        virtual Status
        getStatus() const = 0;

        /**
         * Gets the MIDI velocity.  The velocity is used by the Sampler to send
         * MIDI note messages.  Usually, the velocity is used to determine the
         * volume of the note, though some creative synthesizers allow you to
         * map the velocity to another parameter.
         *
         * @returns
         *   The MIDI velocity.
         */

        virtual MIDIData
        getVelocity() const = 0;

        /**
         * Gets the wet Sample object, which is the result of applying
         * registered Effect objects to the dry sample.  If a Sample object
         * hasn't been set, then this will be 0.
         *
         * @returns
         *   The Sample object.
         */

        virtual const Sample *
        getWetSample() const = 0;

        /**
         * Gets a flag indicating whether or not this Zone object's dry Sample
         * is stale.  If a dry Sample is stale, it means that the parameters
         * used to get the dry Sample have changed, or that this Zone doesn't
         * have a dry Sample.
         *
         * @returns
         *   A flag indicating the above.
         */

        virtual bool
        isDrySampleStale() const = 0;

        /**
         * Gets a flag indicating whether or not the wet Sample is stale.  If a
         * wet Sample is stale, it means that the parameters used to get the wet
         * Sample (Effect parameters, for example) have changed, or that the dry
         * Sample that was used to get the wet Sample has changed.
         *
         * @returns
         *   A flag indicating the above.
         */

        virtual bool
        isWetSampleStale() const = 0;

    public slots:

        /**
         * Sets the MIDI polyphonic aftertouch value.  The MIDI polyphonic
         * aftertouch value can be cleared by calling this method with
         * MIDI_VALUE_NOT_SET.
         *
         * @param aftertouch
         *   The aftertouch value.
         *
         * @sa
         *   getAftertouch()
         */

        virtual void
        setAftertouch(synthclone::MIDIData aftertouch) = 0;

        /**
         * Sets the MIDI channel.
         *
         * @param channel
         *   The MIDI channel.
         *
         * @sa
         *   getChannel()
         */

        virtual void
        setChannel(synthclone::MIDIData channel) = 0;

        /**
         * Sets the MIDI channel pressure value.  The MIDI channel pressure
         * value can be cleared by calling this method with MIDI_VALUE_NOT_SET.
         *
         * @param pressure
         *   The pressure value.
         *
         * @sa
         *   getChannelPressure()
         */

        virtual void
        setChannelPressure(synthclone::MIDIData pressure) = 0;

        /**
         * Sets a MIDI control value.  A control can be cleared by calling this
         * method with the value set to MIDI_VALUE_NOT_SET.
         *
         * @param control
         *   The control index.
         *
         * @param value
         *   The control value.
         *
         * @sa
         *   getControlValue()
         */

        virtual void
        setControlValue(synthclone::MIDIData control,
                        synthclone::MIDIData value) = 0;

        /**
         * Sets the dry Sample.  The data in the Sample is copied to a new
         * Sample object, which has its memory managed by this Zone.  The dry
         * Sample will be set as fresh (not stale).
         *
         * @param sample
         *   The sample to copy.
         *
         * @sa
         *   getDrySample()
         */

        virtual void
        setDrySample(synthclone::Sample *sample) = 0;

        /**
         * Sets the MIDI note.
         *
         * @param note
         *   The MIDI note.
         *
         * @sa
         *   getNote()
         */

        virtual void
        setNote(synthclone::MIDIData note) = 0;

        /**
         * Sets the release time.
         *
         * @param releaseTime
         *   The release time.
         *
         * @sa
         *   getReleaseTime()
         */

        virtual void
        setReleaseTime(SampleTime releaseTime) = 0;

        /**
         * Sets the sample time.
         *
         * @param sampleTime
         *   The sample time.
         *
         * @sa
         *   getSampleTime()
         */

        virtual void
        setSampleTime(SampleTime sampleTime) = 0;

        /**
         * Sets the MIDI velocity.
         *
         * @param velocity
         *   The MIDI velocity.
         *
         * @sa
         *   getVelocity()
         */

        virtual void
        setVelocity(synthclone::MIDIData velocity) = 0;

        /**
         * Used to indicate that the wet Sample is now stale.
         *
         * @sa
         *   isWetSampleStale()
         */

        virtual void
        setWetSampleStale() = 0;

    signals:

        /**
         * Emitted when the MIDI polyphonic aftertouch value is changed.
         *
         * @param aftertouch
         *   The new MIDI polyphonic aftertouch value.
         */

        void
        aftertouchChanged(synthclone::MIDIData aftertouch);

        /**
         * Emitted when the MIDI channel is changed.
         *
         * @param channel
         *   The new MIDI channel.
         */

        void
        channelChanged(synthclone::MIDIData channel);

        /**
         * Emitted when the MIDI channel pressure value is changed.
         *
         * @param pressure
         *   The new MIDI channel pressure value.
         */

        void
        channelPressureChanged(synthclone::MIDIData pressure);

        /**
         * Emitted when a MIDI control value is changed.
         *
         * @param control
         *   The control index.
         *
         * @param value
         *   The new control value.
         */

        void
        controlValueChanged(synthclone::MIDIData control,
                            synthclone::MIDIData value);

        /**
         * Emitted when the dry Sample is changed.  Note that the dry Sample can
         * be set to 0.
         *
         * @param drySample
         *   The new dry Sample (or lack thereof).
         */

        void
        drySampleChanged(const synthclone::Sample *drySample);

        /**
         * Emitted when the dry Sample is (un)marked as stale.
         *
         * @param stale
         *   Whether or not the dry Sample is now stale.
         */

        void
        drySampleStaleChanged(bool stale);

        /**
         * Emitted when the MIDI note is changed.
         *
         * @param note
         *   The new MIDI note.
         */

        void
        noteChanged(synthclone::MIDIData note);

        /**
         * Emitted when the release time is changed.
         *
         * @param releaseTime
         *   The new release time.
         */

        void
        releaseTimeChanged(synthclone::SampleTime releaseTime);

        /**
         * Emitted when the sample time is changed.
         *
         * @param sampleTime
         *   The new sample time.
         */

        void
        sampleTimeChanged(synthclone::SampleTime sampleTime);

        /**
         * Emitted when a zone's status is changed.
         *
         * @param status
         *   The new status.
         */

        void
        statusChanged(synthclone::Zone::Status status);

        /**
         * Emitted when the MIDI velocity is changed.
         *
         * @param velocity
         *   The new MIDI velocity.
         */

        void
        velocityChanged(synthclone::MIDIData velocity);

        /**
         * Emitted when the wet Sample is changed.  Note that the wet Sample can
         * be set to 0.
         *
         * @param wetSample
         *   The new wet Sample (or lack thereof).
         */

        void
        wetSampleChanged(const synthclone::Sample *wetSample);

        /**
         * Emitted when the wet Sample is (un)marked as stale.
         *
         * @param stale
         *   Whether or not the wet Sample is now stale.
         */

        void
        wetSampleStaleChanged(bool stale);

    protected:

        /**
         * Constructs a new Zone object.  This constructor cannot be accessed
         * directly.  To create new Zone objects, call Context::addZone().
         *
         * @param parent
         *   The parent object of the new Zone object.
         */

        explicit
        Zone(QObject *parent=0);

        /**
         * Destroys a Zone object.  The memory of Zone objects is managed by the
         * application.
         */

        virtual
        ~Zone();

    };

}

#endif
