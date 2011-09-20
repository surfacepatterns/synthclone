#include <cassert>

#include "menuviewlet.h"

MenuViewlet::MenuViewlet(QMenu *menu, QObject *parent):
    MenuItemViewlet(parent)
{
    customItemsBoundary = 0;
    manageMenu = false;
    this->menu = menu;
}

MenuViewlet::MenuViewlet(const QString &text, QObject *parent):
    MenuItemViewlet(parent)
{
    menu = new QMenu(text);
    menu->setTitle(text);
    manageMenu = true;
}

MenuViewlet::MenuViewlet(QMenu *menu, QAction *customItemsBoundary,
                         QObject *parent):
    MenuItemViewlet(parent)
{
    this->customItemsBoundary = customItemsBoundary;
    manageMenu = false;
    this->menu = menu;
}

MenuViewlet::~MenuViewlet()
{
    if (manageMenu) {
        menu->deleteLater();
    }
}

void
MenuViewlet::add(MenuLeafViewlet *viewlet)
{
    children.append(viewlet);
    menu->insertAction(customItemsBoundary,
                       qobject_cast<MenuItemViewlet *>(viewlet)->getAction());
}

void
MenuViewlet::add(MenuViewlet *viewlet)
{
    QString text = viewlet->getText();
    subMenuMap[text] = viewlet;
    children.append(viewlet);
    menu->insertMenu(customItemsBoundary, viewlet->getMenu());
}

QAction *
MenuViewlet::getAction()
{
    return menu->menuAction();
}

MenuItemViewlet *
MenuViewlet::getChild(int index)
{
    return children[index];
}

int
MenuViewlet::getChildCount()
{
    return children.count();
}

QMenu *
MenuViewlet::getMenu()
{
    return menu;
}

MenuViewlet *
MenuViewlet::getSubMenuViewlet(const QString &text)
{
    return subMenuMap.value(text, 0);
}

QString
MenuViewlet::getText()
{
    return menu->title();
}

void
MenuViewlet::move(int fromIndex, int toIndex)
{
    int count = children.count();
    assert((fromIndex >= 0) && (fromIndex < count));
    assert((toIndex >= 0) && (toIndex < count));
    assert(fromIndex != toIndex);
    QAction *movingAction = children[fromIndex]->getAction();
    menu->removeAction(movingAction);
    menu->insertAction(children[toIndex]->getAction(), movingAction);
    children.move(fromIndex, toIndex);
}

void
MenuViewlet::remove(MenuItemViewlet *viewlet)
{
    assert(children.contains(viewlet));
    menu->removeAction(viewlet->getAction());
    children.removeOne(viewlet);
}

void
MenuViewlet::remove(MenuLeafViewlet *viewlet)
{
    remove(qobject_cast<MenuItemViewlet *>(viewlet));
}

void
MenuViewlet::remove(MenuViewlet *viewlet)
{
    remove(qobject_cast<MenuItemViewlet *>(viewlet));
    subMenuMap.remove(viewlet->getText());
}

void
MenuViewlet::setText(const QString &text)
{
    menu->setTitle(text);
}
