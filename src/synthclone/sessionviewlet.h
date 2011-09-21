/*
 * synthclone - Synthesizer-cloning software
 * Copyright (C) 2011 Devin Anderson
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA.
 */

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
