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
