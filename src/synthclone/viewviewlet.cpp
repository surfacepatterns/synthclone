#include <synthclone/util.h>

#include "viewviewlet.h"

ViewViewlet::ViewViewlet(QMainWindow *mainWindow, QObject *parent):
    QObject(parent)
{
    QAction *participantsAction =
        synthclone::getChild<QAction>(mainWindow, "participantsViewAction");
    connect(participantsAction, SIGNAL(triggered()),
            SIGNAL(participantEditRequest()));

    QMenu *viewMenu = synthclone::getChild<QMenu>(mainWindow, "viewMenu");
    menuViewlet = new MenuViewlet(viewMenu);
}

ViewViewlet::~ViewViewlet()
{
    menuViewlet->deleteLater();
}

MenuViewlet *
ViewViewlet::getMenuViewlet()
{
    return menuViewlet;
}
