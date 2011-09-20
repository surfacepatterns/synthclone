#ifndef __VIEWVIEWLET_H__
#define __VIEWVIEWLET_H__

#include <QtGui/QMainWindow>

#include "menuviewlet.h"

class ViewViewlet: public QObject {

    Q_OBJECT

public:

    explicit
    ViewViewlet(QMainWindow *mainWindow, QObject *parent=0);

    ~ViewViewlet();

    MenuViewlet *
    getMenuViewlet();

signals:

    void
    participantEditRequest();

private:

    MenuViewlet *menuViewlet;

};

#endif
