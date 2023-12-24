/*
 * libsynthclone_lv2 - LV2 effect plugin for `synthclone`
 * Copyright (C) 2012 Devin Anderson
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

#ifndef __RESIZEEVENTFILTER_H__
#define __RESIZEEVENTFILTER_H__

#include <QtCore/QEvent>
#include <QtWidgets/QScrollArea>

class ResizeEventFilter: public QObject {

    Q_OBJECT

public:

    ResizeEventFilter(QScrollArea *scrollArea, QWidget *widget,
                      QObject *parent=0);

    ~ResizeEventFilter();

    bool
    eventFilter(QObject *obj, QEvent *event);

private:

    QScrollArea *scrollArea;
    QWidget *widget;

};

#endif
