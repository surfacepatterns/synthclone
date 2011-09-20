#include <QtGui/QContextMenuEvent>

#include "contextmenueventfilter.h"

ContextMenuEventFilter::ContextMenuEventFilter(QObject *parent):
    QObject(parent)
{
    // Empty
}

ContextMenuEventFilter::~ContextMenuEventFilter()
{
    // Empty
}

bool
ContextMenuEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ContextMenu) {
        QContextMenuEvent *contextMenuEvent =
            static_cast<QContextMenuEvent *>(event);
        emit contextMenuRequest(contextMenuEvent->x(), contextMenuEvent->y());
        return true;
    }
    return QObject::eventFilter(obj, event);
}
