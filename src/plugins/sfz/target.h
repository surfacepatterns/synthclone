/*
 * libsynthclone_sfz - SFZ target plugin for `synthclone`
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

#ifndef __TARGET_H__
#define __TARGET_H__

#include <synthclone/target.h>
#include <synthclone/types.h>

#include "controllayer.h"
#include "types.h"

class Target: public synthclone::Target {

    Q_OBJECT

public:

    typedef QList<synthclone::MIDIData> ControlList;

    explicit
    Target(const QString &name, QObject *parent=0);

    ~Target();

    void
    build(const QList<synthclone::Zone *> &zones);

    CrossfadeCurve
    getControlCrossfadeCurve() const;

    const ControlLayer *
    getControlLayer(int index) const;

    ControlLayer *
    getControlLayer(int index);

    int
    getControlLayerCount() const;

    CrossfadeCurve
    getNoteCrossfadeCurve() const;

    QString
    getPath() const;

    SampleFormat
    getSampleFormat() const;

    CrossfadeCurve
    getVelocityCrossfadeCurve() const;

    bool
    isDrumKit() const;

public slots:

    ControlLayer *
    addControlLayer(synthclone::MIDIData control);

    void
    moveControlLayer(int fromIndex, int toIndex);

    void
    removeControlLayer(int index);

    void
    setControlCrossfadeCurve(CrossfadeCurve curve);

    void
    setDrumKit(bool drumKit);

    void
    setNoteCrossfadeCurve(CrossfadeCurve curve);

    void
    setPath(const QString &path);

    void
    setSampleFormat(SampleFormat format);

    void
    setVelocityCrossfadeCurve(CrossfadeCurve curve);

signals:

    void
    controlCrossfadeCurveChanged(CrossfadeCurve curve);

    void
    controlLayerAdded(const ControlLayer *controlLayer, int index);

    void
    controlLayerMoved(const ControlLayer *controlLayer, int fromIndex,
                      int toIndex);

    void
    controlLayerRemoved(const ControlLayer *controlLayer, int index);

    void
    drumKitChanged(bool drumKit);

    void
    noteCrossfadeCurveChanged(CrossfadeCurve curve);

    void
    pathChanged(const QString &path);

    void
    sampleFormatChanged(SampleFormat format);

    void
    velocityCrossfadeCurveChanged(CrossfadeCurve curve);

private:

    typedef QList<ControlLayer *> ControlLayerList;
    typedef QList<const synthclone::Zone *> ZoneList;

    typedef QMap<synthclone::MIDIData, ControlLayer *> ControlLayerMap;

    typedef QMap<synthclone::MIDIData, ZoneList *> VelocityZoneMap;
    typedef QMap<synthclone::MIDIData, VelocityZoneMap *> NoteZoneMap;
    typedef QMap<synthclone::MIDIData, NoteZoneMap *> ZoneMap;

    typedef QMap<const synthclone::Zone *, QStringList> RegionMap;

    void
    constructControlRanges(RegionMap &regionMap, ZoneList &zoneList,
                           int controlLayerIndex);

    void
    writeOpcode(QStringList &stringList, const QString &name,
                const QString &value);

    ControlList availableControls;
    CrossfadeCurve controlCrossfadeCurve;
    ControlLayerMap controlLayerMap;
    ControlLayerList controlLayers;
    bool drumKit;
    CrossfadeCurve noteCrossfadeCurve;
    QString path;
    SampleFormat sampleFormat;
    CrossfadeCurve velocityCrossfadeCurve;

};

#endif
