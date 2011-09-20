#ifndef __TOOLVIEWLET_H__
#define __TOOLVIEWLET_H__

#include <QtGui/QMainWindow>

#include "menuviewlet.h"

class ToolViewlet: public QObject {

    Q_OBJECT

public:

    explicit
    ToolViewlet(QMainWindow *mainWindow, QObject *parent=0);

    ~ToolViewlet();

    MenuViewlet *
    getMenuViewlet();

private:

    MenuViewlet *menuViewlet;

};

#endif
