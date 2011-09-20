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
