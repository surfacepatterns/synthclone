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

#if defined(SYNTHCLONE_PLATFORM_MACX) || defined(SYNTHCLONE_PLATFORM_UNIX)
#include <cerrno>
#include <cstring>
#elif defined(SYNTHCLONE_PLATFORM_WIN32)
#include <climits>
#endif

#if defined(SYNTHCLONE_PLATFORM_MACX)
#include <QtCore/QMutex>
#endif

#include <QtCore/QDebug>

#include <synthclone/error.h>
#include <synthclone/semaphore.h>
#include <synthclone/util.h>

#if defined(SYNTHCLONE_PLATFORM_MACX)
static quint64 counter;
static QMutex mutex;
#endif

using synthclone::Semaphore;

Semaphore::Semaphore(QObject *parent):
    QObject(parent)
{

#if defined(SYNTHCLONE_PLATFORM_MACX)
    QMutexLocker locker(&mutex);
    name = QString("__synthclone_semaphore_%1__").arg(counter).toAscii();
    semaphore = sem_open(name.constData(), O_CREAT, 0777, 0);
    if (semaphore == ((sem_t *) SEM_FAILED)) {
        throw synthclone::Error(getErrorMessage());
    }
    counter++;
#elif defined(SYNTHCLONE_PLATFORM_UNIX)
    if (sem_init(&semaphore, 0, 0)) {
        throw synthclone::Error(getErrorMessage());
    }
#elif defined(SYNTHCLONE_PLATFORM_WIN32)
    semaphore = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
    if (! semaphore) {
        throw synthclone::Error(getErrorMessage());
    }
#endif

}

Semaphore::~Semaphore()
{

#if defined(SYNTHCLONE_PLATFORM_MACX) || defined(SYNTHCLONE_PLATFORM_UNIX)
#if defined(SYNTHCLONE_PLATFORM_UNIX)
    sem_t *semaphore = &(this->semaphore);
#endif
    if (sem_destroy(semaphore)) {
        qWarning() << tr("Failed to destroy semaphore: %1").
	    arg(getErrorMessage());
	return;
    }
#if defined(SYNTHCLONE_PLATFORM_MACX)
    if (sem_close(semaphore)) {
        qWarning() << tr("Failed to close semaphore: %1").
	    arg(getErrorMessage());
	return;
    }
    if (sem_unlink(name)) {
        qWarning() << tr("Failed to unlink semaphore '%1': %2").
	    arg(name).arg(getErrorMessage());
    }
#endif
#elif defined(SYNTHCLONE_PLATFORM_WIN32)
    if (! CloseHandle(semaphore)) {
        qWarning() << tr("Failed to close semaphore handle: %1").
	    arg(getErrorMessage());
    }
#endif

}

QString
Semaphore::getErrorMessage()
{

#if defined(SYNTHCLONE_PLATFORM_MACX) || defined(SYNTHCLONE_PLATFORM_UNIX)
    return QString(strerror(errno));
#elif defined(SYNTHCLONE_PLATFORM_WIN32)
    DWORD error = GetLastError();
    char message[MAXERRORLENGTH];
    QString result;
    if (! FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), message,
			MAXERRORLENGTH, NULL)) {
        result = tr("unknown error code '%1'").arg(error);
    } else {
        result = message;
    }
    return result;
#endif

}

void
Semaphore::post()
{

#if defined(SYNTHCLONE_PLATFORM_MACX) || defined(SYNTHCLONE_PLATFORM_UNIX)
#if defined(SYNTHCLONE_PLATFORM_UNIX)
    sem_t *semaphore = &(this->semaphore);
#endif
    CONFIRM(! sem_post(semaphore),
	    tr("`sem_post` error: %1").arg(getErrorMessage()));
#elif defined(SYNTHCLONE_PLATFORM_WIN32)
    CONFIRM(ReleaseSemaphore(semaphore, 1, NULL),
            tr("`ReleaseSemaphore` error: %1").arg(getErrorMessage()));
#endif

}

void
Semaphore::wait()
{

#if defined(SYNTHCLONE_PLATFORM_MACX) || defined(SYNTHCLONE_PLATFORM_UNIX)
#if defined(SYNTHCLONE_PLATFORM_UNIX)
    sem_t *semaphore = &(this->semaphore);
#endif
    while (sem_wait(semaphore)) {
        if (errno != EINTR) {
            throw synthclone::Error(getErrorMessage());
        }
    }
#elif defined(SYNTHCLONE_PLATFORM_WIN32)
    if (WaitForSingleObject(semaphore, INFINITE) != WAIT_OBJECT_0) {
        throw synthclone::Error(getErrorMessage());
    }
#endif

}
