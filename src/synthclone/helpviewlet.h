#ifndef __HELPVIEWLET_H__
#define __HELPVIEWLET_H__

#include <QtGui/QMainWindow>

#include "menuviewlet.h"

class HelpViewlet: public QObject {

    Q_OBJECT

public:

    explicit
    HelpViewlet(QMainWindow *mainWindow, QObject *parent=0);

    ~HelpViewlet();

    MenuViewlet *
    getMenuViewlet();

signals:

    void
    aboutRequest();

private:

    MenuViewlet *menuViewlet;

};

#endif
