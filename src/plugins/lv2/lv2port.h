/*
 * libsynthclone_lv2 - LV2 effect plugin for `synthclone`
 * Copyright (C) 2012 Devin Anderson
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

#ifndef __LV2PORT_H__
#define __LV2PORT_H__

#include <QtCore/QObject>
#include <QtCore/QVariant>

#include <lilv/lilv.h>

class LV2Port: public QObject {

    Q_OBJECT

public:

    LV2Port(const LilvPort *port, const LilvPlugin *plugin, LilvWorld *world,
            QObject *parent=0);

    ~LV2Port();

    QVariant
    getDefaultValue() const;

    QVariant
    getMaximumValue() const;

    QVariant
    getMinimumValue() const;

    uint32_t
    getIndex() const;

    QString
    getName() const;

    QString
    getSymbol() const;

    bool
    isAudioPort() const;

    bool
    isConnectionOptional() const;

    bool
    isControlPort() const;

    bool
    isInputPort() const;

    bool
    isOutputPort() const;

private:

    QVariant
    getNodeValue(const LilvNode *node) const;

    bool
    isType(const QString &typeURI) const;

    QVariant defaultValue;
    QVariant maximumValue;
    QVariant minimumValue;
    const LilvPort *port;
    const LilvPlugin *plugin;
    LilvWorld *world;

};

#endif
