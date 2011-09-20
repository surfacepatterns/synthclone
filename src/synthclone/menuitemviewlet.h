#ifndef __MENUITEMVIEWLET_H__
#define __MENUITEMVIEWLET_H__

#include <QtGui/QAction>

class MenuViewlet;

class MenuItemViewlet: public QObject {

    Q_OBJECT

    friend class MenuViewlet;

public slots:

    void
    setVisible(bool visible);

protected:

    explicit
    MenuItemViewlet(QObject *parent=0);

    virtual
    ~MenuItemViewlet();

    virtual QAction *
    getAction() = 0;

};

#endif
