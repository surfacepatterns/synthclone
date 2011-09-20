#include <synthclone/util.h>

#include "helpviewlet.h"

HelpViewlet::HelpViewlet(QMainWindow *mainWindow, QObject *parent):
    QObject(parent)
{
    QAction *aboutAction =
        synthclone::getChild<QAction>(mainWindow, "aboutHelpAction");
    connect(aboutAction, SIGNAL(triggered()), SIGNAL(aboutRequest()));

    QMenu *helpMenu = synthclone::getChild<QMenu>(mainWindow, "helpMenu");
    menuViewlet = new MenuViewlet(helpMenu);
}

HelpViewlet::~HelpViewlet()
{
    menuViewlet->deleteLater();
}

MenuViewlet *
HelpViewlet::getMenuViewlet()
{
    return menuViewlet;
}
