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

#ifndef __DIRECTORYVIEW_H__
#define __DIRECTORYVIEW_H__

#include <QtGui/QFileDialog>

#include <synthclone/view.h>

class DirectoryView: public synthclone::View {

    Q_OBJECT

public:

    enum Mode {
        MODE_LOADING,
        MODE_SAVING
    };

    explicit
    DirectoryView(QObject *parent=0);

    ~DirectoryView();

    void
    setMode(Mode mode);

    void
    setPath(const QString &path);

    void
    setTitle(const QString &title);

signals:

    void
    pathSelected(const QString &path);

private slots:

    void
    handleDialogAccept();

private:

    QFileDialog *dialog;

};

#endif
