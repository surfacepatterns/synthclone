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
     * Component capable of building a patch from a list of Zone objects.
     */

    class Target: public Component {

        Q_OBJECT

    public:

        /**
         * Builds a patch.
         *
         * @param zones
         *   The Zone list to use to build the patch.
         */

        virtual void
        build(const QList<Zone *> &zones) = 0;

    signals:

        /**
         * Emitted to indicate a warning found during the build process.
         *
         * @param message
         *   The warning message.
         */

        void
        buildWarning(const QString &message);

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

        /**
         * Destroys a Target object.
         */

        virtual
        ~Target();

    };

}

#endif
