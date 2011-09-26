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

#ifndef __SYNTHCLONE_REGISTRATION_H__
#define __SYNTHCLONE_REGISTRATION_H__

#include <QtCore/QObject>

namespace synthclone {

    /**
     * Registration objects are returned when objects are registered with the
     * session.  The sole purpose of registration objects is to send a signal
     * when the registered object has been removed from the session.
     */

    class Registration: public QObject {

        Q_OBJECT

    signals:

        /**
         * Emitted when the object associated with this registration has been
         * removed from the session.
         *
         * @param object
         *   The object associated with this registration.
         */

        void
        unregistered(QObject *object);

    protected:

        /**
         * Constructs a new registration object.  This constructor should not
         * be called directly.  Registration objects are returned when objects
         * are registered with the session via Context objects.
         *
         * @param parent
         *   The parent object of the new registration object.
         *
         * @sa
         *   Context::addEffect, Context::addMenuAction,
         *   Context::addMenuSeparator, Context::addParticipant,
         *   Context::addSampler, Context::addTarget
         */

        explicit
        Registration(QObject *parent=0);

        /**
         * Destroys a Registration object.  The application is responsible for
         * managing Registration object memory.
         */

        virtual
        ~Registration();

    };

}

#endif
