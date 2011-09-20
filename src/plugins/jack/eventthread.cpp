#include "eventthread.h"
#include "sampler.h"

#include <QtCore/QDebug>

EventThread::EventThread(Sampler *sampler, QObject *parent):
    QThread(parent)
{
    this->sampler = sampler;
}

EventThread::~EventThread()
{
    // Empty
}

void
EventThread::run()
{
    sampler->monitorEvents();
}
