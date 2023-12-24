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

#include <cassert>

#include <QtWidgets/QLayout>

#include "dialogview.h"

DialogView::DialogView(const QString &path, QObject *parent):
    synthclone::DesignerView(path, parent)
{
    dialog = qobject_cast<QDialog *>(getRootWidget());
    assert(dialog);

    // XX: If the following line is uncommented, the size of the dialog set in
    // the .ui file changes.
    // dialog->layout()->setSizeConstraint(QLayout::SetFixedSize);

    dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::Dialog |
                           Qt::WindowCloseButtonHint);
    dialog->setModal(true);
}

DialogView::~DialogView()
{
    // Empty
}
