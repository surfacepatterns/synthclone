#include "menuitemviewlet.h"

MenuItemViewlet::MenuItemViewlet(QObject *parent):
    QObject(parent)
{
    // Empty
}

MenuItemViewlet::~MenuItemViewlet()
{
    // Empty
}

void
MenuItemViewlet::setVisible(bool visible)
{
    getAction()->setVisible(visible);
}
