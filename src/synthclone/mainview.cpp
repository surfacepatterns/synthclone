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
    componentViewlet->deleteLater();
    helpViewlet->deleteLater();
    sessionViewlet->deleteLater();
    toolViewlet->deleteLater();
    viewViewlet->deleteLater();
    zoneViewlet->deleteLater();
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
    viewlet->deleteLater();
}

void
MainView::destroyMenuSeparatorViewlet(MenuSeparatorViewlet *viewlet)
{
    viewlet->deleteLater();
}

void
MainView::destroyMenuViewlet(MenuViewlet *viewlet)
{
    viewlet->deleteLater();
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
