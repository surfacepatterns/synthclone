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

#include <QtCore/QPluginLoader>

#include <synthclone/error.h>

#include "pluginmanager.h"

PluginManager::PluginManager(QObject *parent):
    QObject(parent)
{
    // Empty
}

PluginManager::~PluginManager()
{
    for (int i = plugins.count() - 1; i >= 0; i--) {
        unloadPlugin(plugins[i]);
    }
}

synthclone::IPlugin *
PluginManager::loadPlugin(const QString &path)
{
    QString pluginPath = QFileInfo(path).absoluteFilePath();
    QPluginLoader pluginLoader(pluginPath);
    QObject *obj = pluginLoader.instance();
    if (! obj) {
        throw synthclone::Error(pluginLoader.errorString());
    }
    synthclone::IPlugin *plugin = qobject_cast<synthclone::IPlugin *>(obj);
    if (! plugin) {
        pluginLoader.unload();
        QString message = tr("'%1' does not contain a synthclone plugin").
            arg(pluginPath);
        throw synthclone::Error(message);
    }
    if (! plugins.contains(plugin)) {
        plugins.append(plugin);
    }
    return plugin;
}

void
PluginManager::unloadPlugin(synthclone::IPlugin *plugin)
{
    bool removed = plugins.removeOne(plugin);
    assert(removed);
    delete plugin;
}
