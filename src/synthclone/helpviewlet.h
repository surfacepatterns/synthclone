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

#ifndef __HELPVIEWLET_H__
#define __HELPVIEWLET_H__

#include <QMainWindow>

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
