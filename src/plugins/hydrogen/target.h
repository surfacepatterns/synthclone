/*
 * libsynthclone_hydrogen - Hydrogen target plugin for `synthclone`
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

#include <QtCore/QDir>
#include <QtCore/QXmlStreamWriter>

#include <synthclone/target.h>

#include "layeralgorithm.h"
#include "sampleformat.h"
#include "zonekey.h"

class Target: public synthclone::Target {

    Q_OBJECT

public:

    explicit
    Target(const QString &name, QObject *parent=0);

    ~Target();

    QString
    getAuthor() const;

    QString
    getInfo() const;

    LayerAlgorithm
    getLayerAlgorithm() const;

    QString
    getLicense() const;

    QString
    getPath() const;

    SampleFormat
    getSampleFormat() const;

public slots:

    void
    build(const QList<synthclone::Zone *> &zones);

    void
    setAuthor(const QString &author);

    void
    setInfo(const QString &info);

    void
    setLayerAlgorithm(LayerAlgorithm algorithm);

    void
    setLicense(const QString &license);

    void
    setPath(const QString &path);

    void
    setSampleFormat(SampleFormat format);

signals:

    void
    authorChanged(const QString &author);

    void
    infoChanged(const QString &info);

    void
    layerAlgorithmChanged(LayerAlgorithm algorithm);

    void
    licenseChanged(const QString &license);

    void
    pathChanged(const QString &path);

    void
    sampleFormatChanged(SampleFormat format);

private:

    void
    writeElement(QXmlStreamWriter &writer, const QString &name,
                 const QString &value);

    void
    writeLayer(QXmlStreamWriter &writer, int instrument, int layer,
               float lowVelocity, float highVelocity,
               const synthclone::Zone *zone, const QDir &directory);

    QString author;
    QString info;
    LayerAlgorithm layerAlgorithm;
    QString license;
    QString path;
    SampleFormat sampleFormat;

};

#endif
