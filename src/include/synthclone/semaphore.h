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

#ifndef __SYNTHCLONE_SEMAPHORE_H__
#define __SYNTHCLONE_SEMAPHORE_H__

#if defined(SYNTHCLONE_PLATFORM_MACX) || defined(SYNTHCLONE_PLATFORM_UNIX)
#include <semaphore.h>
#elif defined(SYNTHCLONE_PLATFORM_WIN32)
#include <unistd.h>
#include <windows.h>
#else
#error "A semaphore implementation has not been defined for your platform"
#endif

#include <QtCore/QObject>
#include <QtCore/QString>

namespace synthclone {

    /**
     * A portable semaphore object, optimally including a realtime-safe 'post'
     * operation.
     */

    class Semaphore: public QObject {

        Q_OBJECT

    public:

        /**
         * Constructs a new Semaphore object.
         *
         * @param parent
         *   The parent object of the semaphore.
         */

        explicit
        Semaphore(QObject *parent=0);

        /**
         * Destroys a Semaphore object.
         */

        ~Semaphore();

        /**
         * Increments the semaphore.  If there is one or more threads waiting
         * on the semaphore, then a thread will be woken up.
         *
         * @note
         *   This operation is realtime-safe on UNIX, and probably
         *   realtime-safe on Mac.  It's unknown whether the operation is
         *   realtime-safe on Windows.
         */

        void
        post();

        /**
         * Decrements the semaphore if the semaphore's value is greater than
         * zero; otherwise, the thread will wait until another thread performs
         * a 'post' operation on the semaphore.
         */

        void
        wait();

    private:

        QString
	getErrorMessage();

#if defined(SYNTHCLONE_PLATFORM_MACX)
        QByteArray name;
        sem_t *semaphore;
#elif defined(SYNTHCLONE_PLATFORM_UNIX)
        sem_t semaphore;
#elif defined(SYNTHCLONE_PLATFORM_WIN32)
        HANDLE semaphore;
#endif

    };

}

#endif
