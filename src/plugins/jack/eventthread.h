#ifndef __EVENTTHREAD_H__
#define __EVENTTHREAD_H__

#include <QtCore/QThread>

class Sampler;

class EventThread: public QThread {

    Q_OBJECT

public:

    explicit
    EventThread(Sampler *sampler, QObject *parent=0);

    ~EventThread();

protected:

    void
    run();

private:

    Sampler *sampler;

};

#endif
