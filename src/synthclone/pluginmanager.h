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

#ifndef __PLUGINMANAGER_H__
#define __PLUGINMANAGER_H__

#include <QtCore/QList>
#include <QtCore/QMap>

#include <synthclone/iplugin.h>

class PluginManager: public QObject {

    Q_OBJECT

public:

    explicit
    PluginManager(QObject *parent=0);

    ~PluginManager();

    synthclone::IPlugin *
    loadPlugin(const QString &path);

    void
    unloadPlugin(synthclone::IPlugin *plugin);

private:

    QList<synthclone::IPlugin *> plugins;

};

#endif
