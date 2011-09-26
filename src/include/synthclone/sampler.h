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

#ifndef __SYNTHCLONE_SAMPLER_H__
#define __SYNTHCLONE_SAMPLER_H__

#include <synthclone/component.h>
#include <synthclone/samplerjob.h>
#include <synthclone/samplestream.h>

namespace synthclone {

    /**
     * Component capable of capturing and playing back audio.
     */

    class Sampler: public Component {

        Q_OBJECT

    public:

        /**
         * Aborts the current job.  It's possible for this method to be called
         * before the session reflects that the current job is done, but after
         * the Sampler has finished the current job.  In that case, this method
         * should do nothing.
         *
         * @sa
         *   startJob()
         */

        virtual void
        abortJob() = 0;

        /**
         * Starts a new job.  Jobs should be run asynchronously, and should be
         * able to be aborted within a reasonable time interval.  How the job
         * is handled depends on the job type.
         *
         * If the job's type is:
         *   - SamplerJob::TYPE_PLAY_DRY_SAMPLE
         *   - SamplerJob::TYPE_PLAY_WET_SAMPLE
         *   .
         * ... then:
         *   -# Play the given audio.
         *   -# Use the progressChanged() and statusChanged() signals to
         *      indicate progress.
         *   -# When playing is complete, emit a jobCompleted() signal.
         *   .
         * If an error occurs during the playing process:
         *   -# Stop playing the sample.
         *   -# Emit a jobError() signal.
         *   .
         * If abort() is called during the playing process:
         *   -# Stop playing the sample.
         *   -# Emit a jobAborted() signal.
         *
         * If the job's type is:
         *   - SamplerJob::TYPE_SAMPLE
         *   .
         * ... then:
         *   -# Send a MIDI control message on the given MIDI channel for each
         *      control item that's set to a specific value.
         *   -# Send a MIDI note on message on the given MIDI channel for the
         *      given note and velocity values.
         *   -# If the channel pressure value is set, send a MIDI channel
         *      pressure message on the given MIDI channel with the given
         *      channel pressure value.
         *   -# If the aftertouch value is set, send a MIDI aftertouch message
         *      on the given MIDI channel with the given aftertouch value.
         *   -# If there is any known latency for the MIDI port and/or audio
         *      ports, then wait that time before sampling.
         *   -# Retrieve data until Zone::getSampleTime() has passed.  Use the
         *      progressChanged() and statusChanged() signals to indicate
         *      progress.
         *   -# After sampling is complete, send a MIDI note off event on the
         *      given MIDI channel.
         *   -# Send a reset all controllers event on the given channel.
         *   -# Wait the amount of time specified by Zone::getReleaseTime().
         *      Use the progressChanged() and statusChanged() signals to
         *      indicate progress.
         *   -# Make sure that all of the audio data has been written to the
         *      given stream.  Whether or not this is done asynchronously or
         *      after all data is retrieved is up to the sampler
         *      implementation.
         *   -# When sampling is complete, emit the jobCompleted() signal.
         *   .
         * If an error occurs during the sampling process:
         *   -# If MIDI messages have been sent, execute steps 6-8 above (if
         *      possible).
         *   -# Emit a jobError() signal.
         *   .
         * If abort() is called during this operation:
         *   -# If MIDI messages have been sent, execute steps 6-8 above (if
         *      possible).
         *   -# Emit a jobAborted() signal.
         *
         * @param job
         *   The job to be run.
         *
         * @param stream
         *   A stream object.  If the job type is:
         *     - SamplerJob::TYPE_PLAY_DRY_SAMPLE
         *     - SamplerJob::TYPE_PLAY_WET_SAMPLE
         *     .
         *   ... then the object will be a SampleInputStream.  If the job type
         *   is:
         *     - SamplerJob::TYPE_SAMPLE
         *     .
         *   ... then the object will be a SampleOutputStream.  Use
         *   qobject_cast to cast the object to the appropriate type.
         */

        virtual void
        startJob(const SamplerJob &job, SampleStream &stream) = 0;

    signals:

        /**
         * Emitted when a job has been aborted.
         */

        void
        jobAborted();

        /**
         * Emitted when a job finishes.
         */

        void
        jobCompleted();

        /**
         * Emitted when a job cannot be completed due to an error.
         *
         * @param message
         *   The error message.
         */

        void
        jobError(const QString &message);

    protected:

        /**
         * Constructs a new Sampler.  This constructor cannot be called
         * directly; instead, subclasses should call this constructor in their
         * constructors.
         *
         * @param name
         *   The initial name for the sampler.
         *
         * @param parent
         *   The parent object of the sampler.
         */

        explicit
        Sampler(const QString &name, QObject *parent=0);

        virtual
        ~Sampler();

    };

}

#endif
