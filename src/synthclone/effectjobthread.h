#ifndef __EFFECTJOBTHREAD_H__
#define __EFFECTJOBTHREAD_H__

#include <QtCore/QThread>

class Session;

class EffectJobThread: public QThread {

    Q_OBJECT

public:

    explicit
    EffectJobThread(Session *session, QObject *parent=0);

    ~EffectJobThread();

protected:

    void
    run();

private:

    Session *session;

};

#endif
