/*
 * libsynthclone_jack - JACK Audio Connection Kit sampler plugin for
 * `synthclone`
 * Copyright (C) 2011 Devin Anderson
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

#include <cassert>

#ifdef SYNTHCLONE_PLATFORM_UNIX
#include <cerrno>
#include <cstring>
#endif

#include <synthclone/error.h>

#include "semaphore.h"

Semaphore::Semaphore(QObject *parent):
    QObject(parent)
{

#ifdef SYNTHCLONE_PLATFORM_UNIX
    if (sem_init(&semaphore, 0, 0)) {
        throw synthclone::Error(std::strerror(errno));
    }
#endif

}

Semaphore::~Semaphore()
{

#ifdef SYNTHCLONE_PLATFORM_UNIX
    sem_destroy(&semaphore);
#endif

}

void
Semaphore::post()
{

#ifdef SYNTHCLONE_PLATFORM_UNIX
    int result = sem_post(&semaphore);
    assert(! result);
#endif

}

void
Semaphore::wait()
{

#ifdef SYNTHCLONE_PLATFORM_UNIX
    while (sem_wait(&semaphore)) {
        if (errno != EINTR) {
            throw synthclone::Error(std::strerror(errno));
        }
    }
#endif

}
