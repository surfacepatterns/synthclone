#ifndef __MENULEAFVIEWLET_H__
#define __MENULEAFVIEWLET_H__

#include "menuitemviewlet.h"

class MenuLeafViewlet: public MenuItemViewlet {

    Q_OBJECT

protected:

    explicit
    MenuLeafViewlet(QObject *parent=0);

    virtual
    ~MenuLeafViewlet();

    QAction *
    getAction();

private:

    QAction *action;

};

#endif
