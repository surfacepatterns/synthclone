/*
 * synthclone - Synthesizer-cloning software
 * Copyright (C) 2011-2013 Devin Anderson
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

#ifndef __SESSIONLOADVIEW_H__
#define __SESSIONLOADVIEW_H__

#include <QtGui/QDoubleValidator>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>

#include <synthclone/types.h>

#include "dialogview.h"

class SessionLoadView: public DialogView {

    Q_OBJECT

public:

    enum Status {
        STATUS_ERROR,
        STATUS_INFO,
        STATUS_VALID
    };

    explicit
    SessionLoadView(QObject *parent=0);

    ~SessionLoadView();

    void
    addRecentSession(const QString &path);

    void
    clearRecentSessions();

    void
    setCreationEnabled(bool enabled);

    void
    setCreationMessage(const QString &message);

    void
    setCreationName(const QString &name);

    void
    setCreationPath(const QString &path);

    void
    setCreationStatus(Status status);

    void
    setOpenEnabled(bool enabled);

    void
    setOpenMessage(const QString &message);

    void
    setOpenPath(const QString &path);

    void
    setOpenStatus(Status status);

public slots:

    void
    setVisible(bool visible);

signals:

    void
    creationDirectoryBrowseRequest(const QString &path, const QString &name);

    void
    creationDirectoryChanged(const QString &path, const QString &name);

    void
    creationRequest(const QString &path, const QString &name,
                    synthclone::SampleRate sampleRate,
                    synthclone::SampleChannelCount count);

    void
    error(const QString &message);

    void
    openDirectoryBrowseRequest(const QString &path);

    void
    openDirectoryChanged(const QString &path);

    void
    openRequest(const QString &path);

private slots:

    void
    handleCreationDirectoryLookup();

    void
    handleOpenButtonClick();

    void
    handleOpenDirectoryLookup();

    void
    handleRecentSessionsCurrentTextChange(const QString &path);

    void
    handleTabChange(int tab);

    void
    updateCreationDirectory();

    void
    updateOpenDirectory();

private:

    QString
    getResource(Status status);

    QString autoDetectSampleRateText;
    QPushButton *openButton;
    QPushButton *quitButton;
    QListWidget *recentSessions;
    QSpinBox *sampleChannelCount;
    QComboBox *sampleRate;
    QDoubleValidator *sampleRateValidator;
    QLineEdit *sessionCreationDirectory;
    QPushButton *sessionCreationDirectoryLookupButton;
    bool sessionCreationEnabled;
    QLineEdit *sessionCreationName;
    QLabel *sessionCreationStatusIcon;
    QLabel *sessionCreationStatusMessage;
    QLineEdit *sessionOpenDirectory;
    QPushButton *sessionOpenDirectoryLookupButton;
    bool sessionOpenEnabled;
    QLabel *sessionOpenStatusIcon;
    QLabel *sessionOpenStatusMessage;
    QTabWidget *tabWidget;

};

#endif
