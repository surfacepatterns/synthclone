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

#include <synthclone/error.h>

#include "settings.h"

Settings::Settings(Session &session, QObject *parent):
    QObject(parent),
    settings("synthclone.googlecode.com", "synthclone")
{
    verifyReadStatus();
    settings.beginGroup("synthclone");
    connect(&session,
            SIGNAL(stateChanged(synthclone::SessionState, const QDir *)),
            SLOT(handleStateChange(synthclone::SessionState, const QDir *)));
}

Settings::~Settings()
{
    settings.endGroup();
    settings.sync();
}

void
Settings::addPluginPath(const QString &path)
{
    QDir directory(path);
    QString pluginPath = directory.absolutePath();
    QStringList pluginPaths = getPluginPaths();
    if (! pluginPaths.contains(pluginPath)) {
        pluginPaths.append(pluginPath);
        write("pluginPaths", pluginPaths);
    }
}

QStringList
Settings::getPluginPaths()
{
    return read("pluginPaths").toStringList();
}

QStringList
Settings::getRecentSessionPaths()
{
    return read("recentSessionPaths").toStringList();
}

void
Settings::handleStateChange(synthclone::SessionState state,
                            const QDir *directory)
{
    if ((state == synthclone::SESSIONSTATE_CURRENT) && directory) {
        QString path = directory->absolutePath();
        QStringList sessionPaths = getRecentSessionPaths();
        if (sessionPaths.contains(path)) {
            sessionPaths.removeOne(path);
        }
        sessionPaths.prepend(path);
        write("recentSessionPaths", sessionPaths);
    }
}

QVariant
Settings::read(const QString &key, const QVariant &defaultValue)
{
    settings.sync();
    QVariant value = settings.value(key, defaultValue);
    verifyReadStatus();
    return value;
}

void
Settings::removePluginPath(const QString &path)
{
    QString pluginPath = QDir(path).absolutePath();
    QStringList pluginPaths = getPluginPaths();
    if (pluginPaths.contains(pluginPath)) {
        pluginPaths.removeOne(pluginPath);
        write("pluginPaths", pluginPaths);
    }
}

void
Settings::verifyReadStatus() const
{
    switch (settings.status()) {
    case QSettings::AccessError:
        throw synthclone::Error(tr("Failed to read application settings"));
    case QSettings::FormatError:
        throw synthclone::Error(tr("Settings storage is corrupted"));
    default:
        ;
    }
}

void
Settings::write(const QString &key, const QVariant &value)
{
    settings.setValue(key, value);
    settings.sync();
    switch (settings.status()) {
    case QSettings::AccessError:
        throw synthclone::Error(tr("Failed to write application settings"));
    case QSettings::FormatError:
        // This shouldn't happen.
        assert(false);
    default:
        ;
    }
}
