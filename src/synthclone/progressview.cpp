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

#include <synthclone/util.h>

#include "progressview.h"

ProgressView::ProgressView(QObject *parent):
    DialogView(":/synthclone/progressview.ui", parent)
{
    closeButton = synthclone::getChild<QPushButton>(dialog, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));
    connect(this, SIGNAL(closeEnabledChanged(bool)),
            closeButton, SLOT(setEnabled(bool)));

    messages = synthclone::getChild<QPlainTextEdit>(dialog, "messages");

    progressBar = synthclone::getChild<QProgressBar>(dialog, "progressBar");

    status = synthclone::getChild<QLabel>(dialog, "status");
}

ProgressView::~ProgressView()
{
    // Empty
}

void
ProgressView::addMessage(const QString &message)
{
    messages->appendPlainText(message);
}

void
ProgressView::clearMessages()
{
    messages->clear();
}

void
ProgressView::setProgress(float progress)
{
    assert((progress >= 0.0) && (progress <= 1.0));
    progressBar->setValue(static_cast<int>(progress * 10000.0));
}

void
ProgressView::setStatus(const QString &status)
{
    QLabel *label = this->status;
    label->setText(label->fontMetrics().elidedText(status, Qt::ElideRight,
                                                   label->width()));
}
