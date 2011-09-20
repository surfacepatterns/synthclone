#ifndef __SESSIONVIEWLET_H__
#define __SESSIONVIEWLET_H__

#include <QtGui/QMainWindow>

#include <synthclone/types.h>

#include "menuviewlet.h"

class SessionViewlet: public QObject {

    Q_OBJECT

public:

    explicit
    SessionViewlet(QMainWindow *mainWindow, QObject *parent=0);

    ~SessionViewlet();

    MenuViewlet *
    getMenuViewlet();

public slots:

    void
    setLoadEnabled(bool enabled);

    void
    setQuitEnabled(bool enabled);

    void
    setSaveAsEnabled(bool enabled);

    void
    setSaveEnabled(bool enabled);

signals:

    void
    loadRequest();

    void
    quitRequest();

    void
    saveAsRequest();

    void
    saveRequest();

private:

    QAction *customItemsSeparator;
    QAction *loadAction;
    MenuViewlet *menuViewlet;
    QAction *quitAction;
    QAction *saveAction;
    QAction *saveAsAction;

};

#endif
