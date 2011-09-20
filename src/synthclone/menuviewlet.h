#ifndef __MENUVIEWLET_H__
#define __MENUVIEWLET_H__

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtGui/QMenu>

#include "menuleafviewlet.h"

class MenuViewlet: public MenuItemViewlet {

    Q_OBJECT

public:

    explicit
    MenuViewlet(QMenu *menu, QObject *parent=0);

    explicit
    MenuViewlet(const QString &text, QObject *parent=0);

    MenuViewlet(QMenu *menu, QAction *customItemsBoundary, QObject *parent=0);

    ~MenuViewlet();

    void
    add(MenuLeafViewlet *viewlet);

    void
    add(MenuViewlet *viewlet);

    MenuItemViewlet *
    getChild(int index);

    int
    getChildCount();

    MenuViewlet *
    getSubMenuViewlet(const QString &text);

    QString
    getText();

    void
    move(int fromIndex, int toIndex);

    void
    remove(MenuLeafViewlet *viewlet);

    void
    remove(MenuViewlet *viewlet);

    void
    setText(const QString &text);

protected:

    QAction *
    getAction();

private:

    QMenu *
    getMenu();

    void
    remove(MenuItemViewlet *viewlet);

    QList<MenuItemViewlet *> children;
    QAction *customItemsBoundary;
    bool manageMenu;
    QMenu *menu;
    QMap<QString, MenuViewlet *> subMenuMap;

};

#endif
