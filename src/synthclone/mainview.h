#ifndef __MAINVIEW_H__
#define __MAINVIEW_H__

#include <QtGui/QMainWindow>

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
