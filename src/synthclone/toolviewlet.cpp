#include <synthclone/util.h>

#include "toolviewlet.h"

ToolViewlet::ToolViewlet(QMainWindow *mainWindow, QObject *parent):
    QObject(parent)
{
    QMenu *toolMenu = synthclone::getChild<QMenu>(mainWindow, "toolsMenu");
    menuViewlet = new MenuViewlet(toolMenu);
    // The 'tools' menu viewlet is empty when `synthclone` starts.
    menuViewlet->setVisible(false);
}

ToolViewlet::~ToolViewlet()
{
    menuViewlet->deleteLater();
}

MenuViewlet *
ToolViewlet::getMenuViewlet()
{
    return menuViewlet;
}
