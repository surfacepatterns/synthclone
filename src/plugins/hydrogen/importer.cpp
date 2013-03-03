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

#include <cassert>

#include <QtCore/QDebug>
#include <QtCore/QFile>

#include <synthclone/error.h>
#include <synthclone/sampleinputstream.h>

#include "archivereader.h"
#include "importer.h"
#include "temporarydir.h"

Importer::Importer(QObject *parent):
    QObject(parent)
{
    path = "";
}

Importer::~Importer()
{
    // Empty
}

QString
Importer::getPath() const
{
    return path;
}

void
Importer::import()
{
    QDir kitDir;
    TemporaryDir *tempDir;
    QScopedPointer<TemporaryDir> tempDirPtr;
    if (QFileInfo(path).isDir()) {
        kitDir.setPath(path);
    } else {

        // Extract data from archive into temporary directory.
        ArchiveReader archiveReader(path);
        tempDir = new TemporaryDir(this);
        tempDirPtr.reset(tempDir);
        kitDir.setPath(tempDir->getPath());
        char readBuffer[8192];
        for (;;) {
            const ArchiveHeader *header = archiveReader.readHeader();
            if (! header) {
                break;
            }
            if (! header->isFile()) {
                continue;
            }
            QString headerPath = QFileInfo(header->getPath()).fileName();
            QFile file(kitDir.absoluteFilePath(headerPath));
            if (! file.open(QIODevice::WriteOnly)) {
                throw synthclone::Error(file.errorString());
            }
            for (;;) {
                size_t bytesRead = archiveReader.readData(readBuffer, 8192);
                if (! bytesRead) {
                    break;
                }
                size_t bytesWritten = file.write(readBuffer, bytesRead);
                assert(bytesRead == bytesWritten);
            }
            file.close();
        }
    }

    // Parse the drumkit.xml file
    QFile kitFile(kitDir.absoluteFilePath("drumkit.xml"));
    if (! kitFile.open(QIODevice::ReadOnly)) {
        throw synthclone::Error(kitFile.errorString());
    }
    QDomDocument document;
    int column;
    int line;
    QString message;
    bool result = document.setContent(&kitFile, &message, &line, &column);
    kitFile.close();
    if (! result) {
        message = tr("'%1': error in drumkit.xml at line %2, column %3: %4").
            arg(path).arg(line).arg(column).arg(message);
        throw synthclone::Error(message);
    }

    // Walk through the drumkit.xml file, creating zones for each layer.
    QDomElement element = document.documentElement();
    if (element.tagName() != "drumkit_info") {
        message = tr("'%1': drumkit.xml: no 'drumkit_info' element");
        throw synthclone::Error(message);
    }
    element = element.firstChildElement("instrumentList");
    if (element.isNull()) {
        message = tr("'%1' drumkit.xml: no 'instrumentList' element");
        throw synthclone::Error(message);
    }
    synthclone::MIDIData note;
    for (element = element.firstChildElement("instrument"), note = 0;
         ! element.isNull();
         element = element.nextSiblingElement("instrument"),
         note = (note + 1) % 128) {
        QDomElement layerElement = element.firstChildElement("layer");
        if (layerElement.isNull()) {
            importSample(kitDir, element, note, 127);
            continue;
        }
        for (; ! layerElement.isNull();
             layerElement = layerElement.nextSiblingElement("layer")) {
            QDomElement maxElement = layerElement.firstChildElement("max");
            synthclone::MIDIData velocity;
            if (maxElement.isNull()) {
                qWarning() << "'layer' element doesn't contain 'max' element - "
                    "assuming velocity of 127";
                velocity = 127;
            } else {
                QString strValue = maxElement.text();
                bool success;
                float value = strValue.toFloat(&success);
                if (! success) {
                    qWarning() << "'layer' element contains non-float 'max' "
                        "element - assuming velocity of 127";
                    velocity = 127;
                } else if (value > 1.0) {
                    qWarning() << "'layer' element contains 'max' value that's "
                        "too large - assuming velocity of 127";
                    velocity = 127;
                } else if (value < 0.0) {
                    qWarning() << "'layer' element contains 'max' value that's "
                        "too small - assuming velocity of 0";
                    velocity = 0;
                } else {
                    velocity = static_cast<synthclone::MIDIData>(value * 127.0);
                }
            }
            importSample(kitDir, layerElement, note, velocity);
        }
    }
}

void
Importer::importSample(const QDir &kitDir, const QDomElement &element,
                       synthclone::MIDIData note, synthclone::MIDIData velocity)
{
    // If there is no filename element, then we're done.
    QDomElement filenameElement = element.firstChildElement("filename");
    if (filenameElement.isNull()) {
        return;
    }
    try {
        synthclone::Sample sample
            (kitDir.absoluteFilePath(QFileInfo(filenameElement.text()).
                                     fileName()));
        synthclone::SampleInputStream stream(sample);

        // Check the total time consumed by the sample.  If it isn't in the
        // acceptable range for `synthclone` dry samples, then raise a
        // `synthclone::Error`.
        synthclone::SampleRate sampleRate = stream.getSampleRate();
        synthclone::SampleTime time = stream.getFrames() /
            static_cast<synthclone::SampleTime>(sampleRate);
        QString message;
        if (time > synthclone::SAMPLE_TIME_MAXIMUM) {
            message = tr("%1: sample time is %2, which is greater than %3 "
                         "seconds").
                arg(path).arg(time).arg(synthclone::SAMPLE_TIME_MAXIMUM);
            throw synthclone::Error(message);
        }
        if (time < synthclone::SAMPLE_TIME_MINIMUM) {
            message = tr("%1: sample time is %2, which is less than %3 "
                         "seconds").
                arg(path).arg(time).arg(synthclone::SAMPLE_TIME_MINIMUM);
            throw synthclone::Error(message);
        }

        emit layerImported(note, velocity, time, sample);
    } catch (synthclone::Error &e) {
        qWarning() << tr("%1, line %2, column %3: %4").
            arg(kitDir.absoluteFilePath("drumkit.xml")).
            arg(element.lineNumber()).arg(element.columnNumber()).
            arg(e.getMessage());
    }
}

void
Importer::setPath(const QString &path)
{
    if (this->path != path) {
        this->path = path;
        emit pathChanged(path);
    }
}
