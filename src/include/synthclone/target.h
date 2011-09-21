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

#ifndef __SYNTHCLONE_TARGET_H__
#define __SYNTHCLONE_TARGET_H__

#include <QtCore/QList>

#include <synthclone/component.h>
#include <synthclone/zone.h>

namespace synthclone {

    /**
     * Component capable of saving data to a patch.
     */

    class Target: public Component {

        Q_OBJECT

    public:

        /**
         * Saves zone data to a patch.
         *
         * @param zones
         *   The zone list to save.
         */

        virtual void
        save(const QList<Zone *> &zones) = 0;

        /**
         * Validates zone data for saving to a patch created by this target.
         * Subclasses should emit a validationWarning() signal or
         * validationError() signal each time a warning or error is found while
         * validating the zone data.
         *
         * The default implementation does nothing.
         *
         * @param zones
         *   The zone list to validate.
         */

        virtual void
        validate(const QList<Zone *> &zones) const;

    signals:

        /**
         * Emitted to indicate an error found during validation.
         *
         * @param message
         *   The error message.
         */

        void
        validationError(const QString &message);

        /**
         * Emitted to indicate a warning found during validation.
         *
         * @param message
         *   The warning message.
         */

        void
        validationWarning(const QString &message);

    protected:

        /**
         * Constructs a new Target object.  This constructor cannot be called
         * directly; instead, subclasses should call this constructor in their
         * constructors.
         *
         * @param name
         *   The initial name for the target.
         *
         * @param parent
         *   The parent object of the target.
         */

        explicit
        Target(const QString &name, QObject *parent=0);

        virtual
        ~Target();

    };

}

#endif
