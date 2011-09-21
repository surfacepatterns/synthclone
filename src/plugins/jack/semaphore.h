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

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#ifdef SYNTHCLONE_PLATFORM_UNIX
#include <semaphore.h>
#else
#error "A semaphore implementation has not been defined for your platform"
#endif

#include <QtCore/QObject>

class Semaphore: public QObject {

    Q_OBJECT

public:

    explicit
    Semaphore(QObject *parent=0);

    ~Semaphore();

    void
    post();

    void
    wait();

private:

#ifdef SYNTHCLONE_PLATFORM_UNIX
    sem_t semaphore;
#endif

};

#endif
