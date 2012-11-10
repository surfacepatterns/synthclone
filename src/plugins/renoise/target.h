/*
 * libsynthclone_renoise - Renoise target plugin for `synthclone`
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

#ifndef __TARGET_H__
#define __TARGET_H__

#include <QtCore/QDir>
#include <QtCore/QXmlStreamWriter>

#include <synthclone/target.h>

#include "archivewriter.h"
#include "types.h"
#include "zonekey.h"

class Target: public synthclone::Target {

    Q_OBJECT

public:

    explicit
    Target(const QString &name, QObject *parent=0);

    ~Target();

    QString
    getInstrumentName() const;

    LayerAlgorithm
    getLayerAlgorithm() const;

    QString
    getPath() const;

    PitchInterpolation
    getPitchInterpolation() const;

    bool
    isDrumKit() const;

public slots:

    void
    build(const QList<synthclone::Zone *> &zones);

    void
    setDrumKit(bool drumKit);

    void
    setInstrumentName(const QString &name);

    void
    setLayerAlgorithm(LayerAlgorithm algorithm);

    void
    setPath(const QString &path);

    void
    setPitchInterpolation(PitchInterpolation interpolation);

signals:

    void
    drumKitChanged(bool drumKit);

    void
    instrumentNameChanged(const QString &name);

    void
    layerAlgorithmChanged(LayerAlgorithm algorithm);

    void
    pathChanged(const QString &path);

    void
    pitchInterpolationChanged(PitchInterpolation interpolation);

private:

    void
    writeElement(QXmlStreamWriter &writer, const QString &name,
                 const QString &value);


    void
    writeMapping(QXmlStreamWriter &confWriter, const synthclone::Zone *zone,
                 int index, synthclone::MIDIData noteStart,
                 synthclone::MIDIData noteEnd,
                 synthclone::MIDIData velocityStart,
                 synthclone::MIDIData velocityEnd);

    void
    writeSample(ArchiveWriter &archiveWriter, QXmlStreamWriter &confWriter,
                const synthclone::Zone *zone);

    bool drumKit;
    QString instrumentName;
    LayerAlgorithm layerAlgorithm;
    QString path;
    PitchInterpolation pitchInterpolation;

};

#endif
