#include "closeeventfilter.h"

using synthclone::CloseEventFilter;

CloseEventFilter::CloseEventFilter(QObject *parent):
    QObject(parent)
{
    closeEnabled = true;
}

CloseEventFilter::~CloseEventFilter()
{
    // Empty
}

bool
CloseEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Close) {
        event->ignore();
        if (closeEnabled) {
            emit closeRequest();
        }
        return true;
    }
    return QObject::eventFilter(obj, event);
}

bool
CloseEventFilter::isCloseEnabled() const
{
    return closeEnabled;
}

void
CloseEventFilter::setCloseEnabled(bool enabled)
{
    closeEnabled = enabled;
}
