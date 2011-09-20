#ifndef __CONTEXTMENUEVENTFILTER_H__
#define __CONTEXTMENUEVENTFILTER_H__

#include <QtCore/QEvent>
#include <QtCore/QObject>

class ContextMenuEventFilter: public QObject {

    Q_OBJECT

public:

    explicit
    ContextMenuEventFilter(QObject *parent=0);

    ~ContextMenuEventFilter();

    bool
    eventFilter(QObject *obj, QEvent *event);

signals:

    void
    contextMenuRequest(int x, int y);

};

#endif
