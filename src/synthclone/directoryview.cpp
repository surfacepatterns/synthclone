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

#include <QtCore/QUrl>

#include "directoryview.h"

DirectoryView::DirectoryView(QObject *parent):
    synthclone::View(new QFileDialog(), parent)
{
    dialog = qobject_cast<QFileDialog *>(getRootWidget());
    connect(dialog, SIGNAL(accepted()), SLOT(handleDialogAccept()));
    connect(dialog, SIGNAL(rejected()), SIGNAL(closeRequest()));
    dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::Dialog |
                           Qt::WindowCloseButtonHint);
    dialog->setModal(true);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
}

DirectoryView::~DirectoryView()
{
    // Empty
}

void
DirectoryView::handleDialogAccept()
{
    QStringList paths = dialog->selectedFiles();
    assert(paths.count() == 1);
    QString path = paths[0];

    // Apparently, the current path can be a URL.  The Qt documentation doesn't
    // say anything about it, but it happens on Ubuntu 10.04.
    QUrl url(path);
    if (url.isValid() && (url.scheme() == "file")) {
        path = url.toLocalFile();
    }
    emit pathSelected(path);
}

void
DirectoryView::setMode(Mode mode)
{
    if (mode == MODE_LOADING) {
        dialog->setAcceptMode(QFileDialog::AcceptOpen);
    } else {
        dialog->setAcceptMode(QFileDialog::AcceptSave);
    }
}

void
DirectoryView::setPath(const QString &path)
{
    dialog->setDirectory(path);
}

void
DirectoryView::setTitle(const QString &title)
{
    dialog->setWindowTitle(title);
}
