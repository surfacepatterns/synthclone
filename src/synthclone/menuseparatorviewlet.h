#ifndef __MENUSEPARATORVIEWLET_H__
#define __MENUSEPARATORVIEWLET_H__

#include "menuleafviewlet.h"

class MenuSeparatorViewlet: public MenuLeafViewlet {

    Q_OBJECT

public:

    explicit
    MenuSeparatorViewlet(QObject *parent=0);

    ~MenuSeparatorViewlet();

};

#endif
