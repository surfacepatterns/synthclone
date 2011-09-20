#include <synthclone/util.h>
#include <synthclone/view.h>

#include "closeeventfilter.h"

using synthclone::View;

View::View(QWidget *rootWidget, QObject *parent):
    QObject(parent)
{
    closeEventFilter = new CloseEventFilter(this);
    connect(closeEventFilter, SIGNAL(closeRequest()), SIGNAL(closeRequest()));
    rootWidget->installEventFilter(closeEventFilter);
    this->rootWidget = rootWidget;
}

View::~View()
{
    rootWidget->deleteLater();
    closeEventFilter->deleteLater();
}

QWidget *
View::getRootWidget()
{
    return rootWidget;
}

bool
View::isCloseEnabled() const
{
    return closeEventFilter->isCloseEnabled();
}

bool
View::isVisible() const
{
    return rootWidget->isVisible();
}

void
View::setCloseEnabled(bool enabled)
{
    if (closeEventFilter->isCloseEnabled() != enabled) {
        closeEventFilter->setCloseEnabled(enabled);
        emit closeEnabledChanged(enabled);
    }
}

void
View::setVisible(bool visible)
{
    if (rootWidget->isVisible() != visible) {
        rootWidget->setVisible(visible);
        emit visibilityChanged(visible);
    }
}
