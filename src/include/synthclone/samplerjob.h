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

#ifndef __SYNTHCLONE_SAMPLERJOB_H__
#define __SYNTHCLONE_SAMPLERJOB_H__

#include <synthclone/zone.h>

namespace synthclone {

    /**
     * Represents a job that will be sent to a Sampler.
     */

    class SamplerJob: public QObject {

        Q_OBJECT

    public:

        /**
         * The type of job that will be executed by the Sampler.
         */

        enum Type {
            TYPE_PLAY_DRY_SAMPLE=0,
            TYPE_PLAY_WET_SAMPLE=1,
            TYPE_SAMPLE=2
        };

        /**
         * Gets the SamplerJob::Type.
         *
         * @returns
         *   The SamplerJob::Type.
         */

        virtual Type
        getType() const = 0;

        /**
         * Gets the Zone that the Sampler will use to get information for this
         * SamplerJob.
         *
         * @returns
         *   The Zone.
         */

        virtual const Zone *
        getZone() const = 0;

    protected:

        /**
         * Constructs a new SamplerJob object.  SamplerJob objects cannot be
         * created directly.  SamplerJob objects are created by calling
         * Context::addSamplerJob().
         *
         * @param parent
         *   The parent object of the new SamplerJob.
         */

        explicit
        SamplerJob(QObject *parent=0);

        /**
         * Destroys a SamplerJob.  The application is responsible for managing
         * SamplerJob memory.
         */

        virtual
        ~SamplerJob();

    };

}

#endif
