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

#include <QLayout>
#include <QScrollBar>

#include "resizeeventfilter.h"

ResizeEventFilter::ResizeEventFilter(QScrollArea *scrollArea, QWidget *widget,
                                     QObject *parent):
    QObject(parent)
{
    this->scrollArea = scrollArea;
    this->widget = widget;
}

ResizeEventFilter::~ResizeEventFilter()
{
    // Empty
}

bool
ResizeEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == widget) {
        switch (event->type()) {
        case QEvent::Show:
            widget->adjustSize();
            // Fallthrough on purpose ...
        case QEvent::Resize:
            scrollArea->setMinimumWidth
                (widget->minimumSizeHint().width() +
                 scrollArea->verticalScrollBar()->width());
            widget->layout()->activate();
            // Fallthrough on purpose ...
        default:
            ;
        }
    }
    return QObject::eventFilter(obj, event);
}
