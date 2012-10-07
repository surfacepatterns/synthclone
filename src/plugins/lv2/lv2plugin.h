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

#ifndef __LV2PLUGIN_H__
#define __LV2PLUGIN_H__

#include <QtCore/QStringList>

#include "lv2instance.h"
#include "lv2port.h"
#include "lv2uidata.h"

class LV2Plugin: public QObject {

    Q_OBJECT

public:

    explicit
    LV2Plugin(const LilvPlugin *plugin, LilvWorld *world, QObject *parent=0);

    ~LV2Plugin();

    const LV2Port &
    getAudioInputPort(int index) const;

    int
    getAudioInputPortCount() const;

    const LV2Port &
    getAudioOutputPort(int index) const;

    int
    getAudioOutputPortCount() const;

    QString
    getClass(int index) const;

    int
    getClassCount() const;

    const LV2Port &
    getControlInputPort(int index) const;

    int
    getControlInputPortCount() const;

    const LV2Port &
    getControlOutputPort(int index) const;

    int
    getControlOutputPortCount() const;

    QString
    getName() const;

    const LV2Port &
    getPort(int index) const;

    int
    getPortCount() const;

    QString
    getRequiredFeature(int index) const;

    int
    getRequiredFeatureCount() const;

    const LV2UIData &
    getUIData(int index) const;

    int
    getUIDataCount() const;

    QString
    getURI() const;

    LV2Instance *
    instantiate(double sampleRate) const;

    bool
    isReplaced() const;

private:

    QList<LV2Port *> audioInputPorts;
    QList<LV2Port *> audioOutputPorts;
    QStringList classList;
    QList<LV2Port *> controlInputPorts;
    QList<LV2Port *> controlOutputPorts;
    QList<LV2Port *> ports;
    const LilvPlugin *plugin;
    QStringList requiredFeatures;
    QList<LV2UIData *> uiDataList;
    LilvWorld *world;

};

#endif
