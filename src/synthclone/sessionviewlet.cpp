#include <synthclone/util.h>

#include "sessionviewlet.h"
#include "util.h"

SessionViewlet::SessionViewlet(QMainWindow *mainWindow, QObject *parent):
    QObject(parent)
{
    QMenu *sessionMenu = synthclone::getChild<QMenu>(mainWindow, "sessionMenu");

    loadAction = synthclone::getChild<QAction>(mainWindow, "loadSessionAction");
    connect(loadAction, SIGNAL(triggered()), SIGNAL(loadRequest()));

    quitAction = synthclone::getChild<QAction>(mainWindow, "quitSessionAction");
    connect(quitAction, SIGNAL(triggered()), SIGNAL(quitRequest()));

    saveAction = synthclone::getChild<QAction>(mainWindow, "saveSessionAction");
    connect(saveAction, SIGNAL(triggered()), SIGNAL(saveRequest()));

    saveAsAction = synthclone::getChild<QAction>(mainWindow,
                                                 "saveSessionAsAction");
    connect(saveAsAction, SIGNAL(triggered()), SIGNAL(saveAsRequest()));

    // Hack: Optimally, we'd like to give an object name to the separator we
    // want to retrieve from the QtDesigner file.  Unfortunately, QtDesigner
    // doesn't allow the naming of QAction items that are separators (they all
    // have to be named 'separator').  So, we have to add the separator here.
    customItemsSeparator = new QAction(this);
    customItemsSeparator->setSeparator(true);
    sessionMenu->insertAction(quitAction, customItemsSeparator);

    menuViewlet = new MenuViewlet(sessionMenu, customItemsSeparator, this);
}

SessionViewlet::~SessionViewlet()
{
    menuViewlet->deleteLater();
    customItemsSeparator->deleteLater();
}

MenuViewlet *
SessionViewlet::getMenuViewlet()
{
    return menuViewlet;
}

void
SessionViewlet::setLoadEnabled(bool enabled)
{
    loadAction->setEnabled(enabled);
}

void
SessionViewlet::setQuitEnabled(bool enabled)
{
    quitAction->setEnabled(enabled);
}

void
SessionViewlet::setSaveAsEnabled(bool enabled)
{
    saveAsAction->setEnabled(enabled);
}

void
SessionViewlet::setSaveEnabled(bool enabled)
{
    saveAction->setEnabled(enabled);
}
