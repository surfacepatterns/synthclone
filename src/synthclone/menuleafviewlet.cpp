#include "menuleafviewlet.h"

MenuLeafViewlet::MenuLeafViewlet(QObject *parent):
    MenuItemViewlet(parent)
{
    action = new QAction(this);
}

MenuLeafViewlet::~MenuLeafViewlet()
{
    action->deleteLater();
}

QAction *
MenuLeafViewlet::getAction()
{
    return action;
}
