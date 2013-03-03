/*
 * libsynthclone_hydrogen - Hydrogen target plugin for `synthclone`
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

#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include <QtCore/QDir>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtXml/QDomDocument>

#include <synthclone/sample.h>
#include <synthclone/types.h>

class Importer: public QObject {

    Q_OBJECT

public:

    explicit
    Importer(QObject *parent=0);

    ~Importer();

    QString
    getPath() const;

public slots:

    void
    import();

    void
    setPath(const QString &path);

signals:

    void
    layerImported(synthclone::MIDIData note, synthclone::MIDIData velocity,
                  synthclone::SampleTime time, synthclone::Sample &sample);

    void
    pathChanged(const QString &path);

private:

    void
    importSample(const QDir &kitDir, const QDomElement &element,
                 synthclone::MIDIData note, synthclone::MIDIData velocity);

    QString path;

};

#endif
