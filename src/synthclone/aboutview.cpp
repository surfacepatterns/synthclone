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

#include <QtCore/QLocale>

#include <synthclone/util.h>

#include "aboutview.h"

AboutView::AboutView(QObject *parent):
    DialogView(":/synthclone/aboutview.ui", parent)
{
    closeButton = synthclone::getChild<QPushButton>(dialog, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    majorVersion = 0;
    minorVersion = 0;
    revision = 0;

    version = synthclone::getChild<QLabel>(dialog, "version");

    updateVersion();
}

AboutView::~AboutView()
{
    // Empty
}

void
AboutView::setMajorVersion(int majorVersion)
{
    this->majorVersion = majorVersion;
    updateVersion();
}

void
AboutView::setMinorVersion(int minorVersion)
{
    this->minorVersion = minorVersion;
    updateVersion();
}

void
AboutView::setRevision(int revision)
{
    this->revision = revision;
    updateVersion();
}

void
AboutView::updateVersion()
{
    QLocale locale = QLocale::system();
    version->setText(tr("%1.%2.%3", "versionFormat").
                     arg(locale.toString(majorVersion)).
                     arg(locale.toString(minorVersion)).
                     arg(locale.toString(revision)));
}
