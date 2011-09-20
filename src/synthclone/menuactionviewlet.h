#ifndef __MENUACTIONVIEWLET_H__
#define __MENUACTIONVIEWLET_H__

#include "menuleafviewlet.h"

class MenuActionViewlet: public MenuLeafViewlet {

    Q_OBJECT

public:

    explicit
    MenuActionViewlet(QObject *parent=0);

    ~MenuActionViewlet();

public slots:

    void
    setDescription(const QString &description);

    void
    setEnabled(bool enabled);

    void
    setText(const QString &text);

signals:

    void
    triggered();

};

#endif
