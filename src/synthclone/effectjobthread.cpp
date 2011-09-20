#include "effectjobthread.h"
#include "session.h"

EffectJobThread::EffectJobThread(Session *session, QObject *parent):
    QThread(parent)
{
    this->session = session;
}

EffectJobThread::~EffectJobThread()
{
    // Empty
}

void
EffectJobThread::run()
{
    session->runEffectJobs();
}
