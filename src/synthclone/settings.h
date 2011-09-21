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

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <QtCore/QDir>
#include <QtCore/QSettings>

#include "session.h"

class Settings: public QObject {

    Q_OBJECT

public:

    explicit
    Settings(Session &session, QObject *parent=0);

    ~Settings();

    void
    addPluginPath(const QString &path);

    QStringList
    getPluginPaths();

    QStringList
    getRecentSessionPaths();

    void
    removePluginPath(const QString &path);

private slots:

    void
    handleStateChange(synthclone::SessionState state, const QDir *directory);

private:

    QVariant
    read(const QString &key, const QVariant &defaultValue=QVariant());

    void
    verifyReadStatus() const;

    void
    write(const QString &key, const QVariant &value);

    QSettings settings;

};

#endif
