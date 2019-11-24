/*
 * synthclone - Synthesizer-cloning software
 * Copyright (C) 2011 Devin Anderson
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA.
 */

#ifndef __MENUVIEWLET_H__
#define __MENUVIEWLET_H__

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QMenu>

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
