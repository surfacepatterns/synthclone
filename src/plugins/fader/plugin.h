/*
 * libsynthclone_fader - Fader effect plugin for `synthclone`
 * Copyright (C) 2013 Devin Anderson
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

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <synthclone/iplugin.h>

#include "participant.h"

class Plugin: public QObject, public synthclone::IPlugin {

    Q_OBJECT
    Q_INTERFACES(synthclone::IPlugin)
    Q_PLUGIN_METADATA(IID "com.googlecode.synthclone.plugins.fader")

public:

    explicit
    Plugin(QObject *parent=0);

    ~Plugin();

    QByteArray
    getId() const;

    synthclone::Participant *
    getParticipant();

private:

    Participant participant;

};

#endif
