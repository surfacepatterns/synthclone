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

#include <QtGui/QFileDialog>

#include <synthclone/util.h>

#include "sessionloadview.h"

SessionLoadView::SessionLoadView(QObject *parent):
    DialogView(":/synthclone/sessionloadview.ui", parent)
{
    openButton = synthclone::getChild<QPushButton>(dialog, "openButton");
    openButton->setEnabled(false);
    connect(openButton, SIGNAL(clicked()), SLOT(handleOpenButtonClick()));

    quitButton = synthclone::getChild<QPushButton>(dialog, "quitButton");
    connect(quitButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    recentSessions =
        synthclone::getChild<QListWidget>(dialog, "recentSessions");
    connect(recentSessions, SIGNAL(currentTextChanged(QString)),
            SLOT(handleRecentSessionsCurrentTextChange(QString)));

    sampleChannelCount =
        synthclone::getChild<QSpinBox>(dialog, "sampleChannelCount");

    sampleRate = synthclone::getChild<QComboBox>(dialog, "sampleRate");
    sampleRateValidator = new QDoubleValidator(this);
    sampleRateValidator->setRange
        (1.0, static_cast<double>(synthclone::SAMPLE_RATE_MAXIMUM), 0);
    sampleRate->setValidator(sampleRateValidator);

    sessionCreationDirectory =
        synthclone::getChild<QLineEdit>(dialog, "sessionCreationDirectory");
    connect(sessionCreationDirectory, SIGNAL(textEdited(QString)),
            SLOT(updateCreationDirectory()));

    sessionCreationDirectoryLookupButton = synthclone::getChild<QPushButton>
        (dialog, "sessionCreationDirectoryLookupButton");
    connect(sessionCreationDirectoryLookupButton, SIGNAL(clicked()),
            SLOT(handleCreationDirectoryLookup()));

    sessionCreationName =
        synthclone::getChild<QLineEdit>(dialog, "sessionCreationName");
    connect(sessionCreationName, SIGNAL(textEdited(QString)),
            SLOT(updateCreationDirectory()));

    sessionCreationStatusIcon =
        synthclone::getChild<QLabel>(dialog, "sessionCreationStatusIcon");

    sessionCreationStatusMessage =
        synthclone::getChild<QLabel>(dialog, "sessionCreationStatusMessage");

    sessionOpenDirectory =
        synthclone::getChild<QLineEdit>(dialog, "sessionOpenDirectory");
    connect(sessionOpenDirectory, SIGNAL(textEdited(QString)),
            SLOT(updateOpenDirectory()));

    sessionOpenDirectoryLookupButton = synthclone::getChild<QPushButton>
        (dialog, "sessionOpenDirectoryLookupButton");
    connect(sessionOpenDirectoryLookupButton, SIGNAL(clicked()),
            SLOT(handleOpenDirectoryLookup()));

    sessionOpenStatusIcon =
        synthclone::getChild<QLabel>(dialog, "sessionOpenStatusIcon");

    sessionOpenStatusMessage =
        synthclone::getChild<QLabel>(dialog, "sessionOpenStatusMessage");

    tabWidget = synthclone::getChild<QTabWidget>(dialog, "tabWidget");
    connect(tabWidget, SIGNAL(currentChanged(int)), SLOT(handleTabChange(int)));

    sessionCreationEnabled = false;
    sessionOpenEnabled = false;
}

SessionLoadView::~SessionLoadView()
{
    delete sampleRateValidator;
}

void
SessionLoadView::addRecentSession(const QString &path)
{
    recentSessions->addItem(path);
}

void
SessionLoadView::clearRecentSessions()
{
    recentSessions->clear();
}

QString
SessionLoadView::getResource(Status status)
{
    switch (status) {
    case STATUS_ERROR:
        return ":/synthclone/images/16x16/error.png";
    case STATUS_INFO:
        return ":/synthclone/images/16x16/information.png";
    case STATUS_VALID:
        return ":/synthclone/images/16x16/ok.png";
    }
    assert(false);
}

void
SessionLoadView::handleCreationDirectoryLookup()
{
    emit creationDirectoryBrowseRequest(sessionCreationDirectory->text(),
                                        sessionCreationName->text());
}

void
SessionLoadView::handleOpenButtonClick()
{
    synthclone::SampleChannelCount sampleChannelCount;
    synthclone::SampleRate sampleRate;
    bool success;
    switch (tabWidget->currentIndex()) {
    case 0:
        sampleRate = static_cast<synthclone::SampleRate>
            (this->sampleRate->currentText().toULong(&success));
        if (! (success && sampleRate)) {
            emit error(tr("The supplied sample rate is not valid."));
            return;
        }
        sampleChannelCount = static_cast<synthclone::SampleChannelCount>
            (this->sampleChannelCount->value());
        assert(sampleChannelCount);
        emit creationRequest(sessionCreationDirectory->text(),
                             sessionCreationName->text(), sampleRate,
                             sampleChannelCount);
        break;
    case 1:
        emit openRequest(sessionOpenDirectory->text());
        break;
    default:
        assert(false);
    }
}

void
SessionLoadView::handleOpenDirectoryLookup()
{
    emit openDirectoryBrowseRequest(sessionOpenDirectory->text());
}

void
SessionLoadView::handleRecentSessionsCurrentTextChange(const QString &path)
{
    if (path.count()) {
        sessionOpenDirectory->setText(path);
        updateOpenDirectory();
    }
}

void
SessionLoadView::handleTabChange(int tab)
{
    switch (tab) {
    case 0:
        openButton->setEnabled(sessionCreationEnabled);
        openButton->setText(tr("Create"));
        break;
    case 1:
        openButton->setEnabled(sessionOpenEnabled);
        openButton->setText(tr("Open"));
        break;
    default:
        assert(false);
    }
}

void
SessionLoadView::setCreationEnabled(bool enabled)
{
    sessionCreationEnabled = enabled;
    if (tabWidget->currentIndex() == 0) {
        openButton->setEnabled(enabled);
    }
}

void
SessionLoadView::setCreationMessage(const QString &message)
{
    sessionCreationStatusMessage->setText(message);
}

void
SessionLoadView::setCreationName(const QString &name)
{
    sessionCreationName->setText(name);
}

void
SessionLoadView::setCreationPath(const QString &path)
{
    sessionCreationDirectory->setText(path);
}

void
SessionLoadView::setCreationStatus(Status status)
{
    sessionCreationStatusIcon->setPixmap(QPixmap(getResource(status)));
}

void
SessionLoadView::setOpenEnabled(bool enabled)
{
    sessionOpenEnabled = enabled;
    if (tabWidget->currentIndex() == 1) {
        openButton->setEnabled(enabled);
    }
}

void
SessionLoadView::setOpenMessage(const QString &message)
{
    sessionOpenStatusMessage->setText(message);
}

void
SessionLoadView::setOpenPath(const QString &path)
{
    int index = recentSessions->currentRow();
    sessionOpenDirectory->setText(path);
    for (int i = recentSessions->count() - 1; i >= 0; i--) {
        if (path == recentSessions->item(i)->text()) {
            if (i != index) {
                recentSessions->setCurrentRow(i);
            }
            return;
        }
    }
    if (index != -1) {
        recentSessions->setCurrentRow(-1);
    }
}

void
SessionLoadView::setOpenStatus(Status status)
{
    sessionOpenStatusIcon->setPixmap(QPixmap(getResource(status)));
}

void
SessionLoadView::setVisible(bool visible)
{
    if (visible) {
        tabWidget->setCurrentIndex(0);
        sessionCreationDirectory->setFocus();
    }
    View::setVisible(visible);
}

void
SessionLoadView::updateCreationDirectory()
{
    emit creationDirectoryChanged(sessionCreationDirectory->text(),
                                  sessionCreationName->text());
}

void
SessionLoadView::updateOpenDirectory()
{
    emit openDirectoryChanged(sessionOpenDirectory->text());
}
