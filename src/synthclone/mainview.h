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

#ifndef __MAINVIEW_H__
#define __MAINVIEW_H__

#include <QMainWindow>

#include <synthclone/designerview.h>

#include "componentviewlet.h"
#include "helpviewlet.h"
#include "menuactionviewlet.h"
#include "menuseparatorviewlet.h"
#include "menuviewlet.h"
#include "sessionviewlet.h"
#include "toolviewlet.h"
#include "viewviewlet.h"
#include "zoneviewlet.h"

class MainView: public synthclone::DesignerView {

    Q_OBJECT

public:

    explicit
    MainView(QObject *parent=0);

    ~MainView();

    ComponentViewlet *
    getComponentViewlet();

    HelpViewlet *
    getHelpViewlet();

    SessionViewlet *
    getSessionViewlet();

    ToolViewlet *
    getToolViewlet();

    ViewViewlet *
    getViewViewlet();

    ZoneViewlet *
    getZoneViewlet();

public slots:

    MenuActionViewlet *
    createMenuActionViewlet(QObject *parent=0);

    MenuSeparatorViewlet *
    createMenuSeparatorViewlet(QObject *parent=0);

    MenuViewlet *
    createMenuViewlet(const QString &text, QObject *parent=0);

    void
    destroyMenuActionViewlet(MenuActionViewlet *viewlet);

    void
    destroyMenuSeparatorViewlet(MenuSeparatorViewlet *viewlet);

    void
    destroyMenuViewlet(MenuViewlet *viewlet);

private:

    ComponentViewlet *componentViewlet;
    HelpViewlet *helpViewlet;
    QMainWindow *mainWindow;
    SessionViewlet *sessionViewlet;
    ToolViewlet *toolViewlet;
    ViewViewlet *viewViewlet;
    ZoneViewlet *zoneViewlet;

};

#endif
