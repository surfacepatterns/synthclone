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

#ifndef __SYNTHCLONE_EFFECTJOB_H__
#define __SYNTHCLONE_EFFECTJOB_H__

#include <synthclone/zone.h>

namespace synthclone {

    /**
     * Represents a job that will be sent to registered Effect objects.
     */

    class EffectJob: public QObject {

        Q_OBJECT

    public:

        /**
         * Gets the Zone that the Effect will use to get information for this
         * EffectJob.
         *
         * @returns
         *   The Zone.
         */

        virtual const Zone *
        getZone() const = 0;

    protected:

        /**
         * Constructs a new EffectJob object.  EffectJob objects cannot be
         * created directly.  EffectJob objects are created by calling
         * Context::addEffectJob().
         *
         * @param parent
         *   The parent object of the new EffectJob.
         */

        explicit
        EffectJob(QObject *parent=0);

        /**
         * Destroys an EffectJob.  The application is responsible for managing
         * EffectJob memory.
         */

        virtual
        ~EffectJob();

    };

}

#endif
