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

#include "mainview.h"

MainView::MainView(QObject *parent):
    synthclone::DesignerView(":/synthclone/mainview.ui", parent)
{
    mainWindow = qobject_cast<QMainWindow *>(getRootWidget());
    componentViewlet = new ComponentViewlet(mainWindow, this);
    helpViewlet = new HelpViewlet(mainWindow, this);
    sessionViewlet = new SessionViewlet(mainWindow, this);
    toolViewlet = new ToolViewlet(mainWindow, this);
    viewViewlet = new ViewViewlet(mainWindow, this);
    zoneViewlet = new ZoneViewlet(mainWindow, this);
}

MainView::~MainView()
{
    delete componentViewlet;
    delete helpViewlet;
    delete sessionViewlet;
    delete toolViewlet;
    delete viewViewlet;
    delete zoneViewlet;
}

MenuActionViewlet *
MainView::createMenuActionViewlet(QObject *parent)
{
    return new MenuActionViewlet(parent);
}

MenuSeparatorViewlet *
MainView::createMenuSeparatorViewlet(QObject *parent)
{
    return new MenuSeparatorViewlet(parent);
}

MenuViewlet *
MainView::createMenuViewlet(const QString &text, QObject *parent)
{
    return new MenuViewlet(text, parent);
}

void
MainView::destroyMenuActionViewlet(MenuActionViewlet *viewlet)
{
    delete viewlet;
}

void
MainView::destroyMenuSeparatorViewlet(MenuSeparatorViewlet *viewlet)
{
    delete viewlet;
}

void
MainView::destroyMenuViewlet(MenuViewlet *viewlet)
{
    delete viewlet;
}

ComponentViewlet *
MainView::getComponentViewlet()
{
    return componentViewlet;
}

HelpViewlet *
MainView::getHelpViewlet()
{
    return helpViewlet;
}

SessionViewlet *
MainView::getSessionViewlet()
{
    return sessionViewlet;
}

ToolViewlet *
MainView::getToolViewlet()
{
    return toolViewlet;
}

ViewViewlet *
MainView::getViewViewlet()
{
    return viewViewlet;
}

ZoneViewlet *
MainView::getZoneViewlet()
{
    return zoneViewlet;
}
