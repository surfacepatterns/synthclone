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
#include <cstdlib>

#include <QtCore/QLocale>
#include <QtCore/QTemporaryFile>

#include <synthclone/error.h>

#include "util.h"

QString
createUniqueFile(const QDir *directory, const QString &prefix,
                 const QString &suffix)
{
    // 'XXXXXX' is used by the QTemporaryFile class as a placeholder for name
    // characters.
    assert(prefix.indexOf("XXXXXX") == -1);
    assert(suffix.indexOf("XXXXXX") == -1);
    QString pathTemplate = QString("%1XXXXXX%2").arg(prefix, suffix);
    assert(QFileInfo(pathTemplate).fileName() == pathTemplate);
    if (directory) {
        pathTemplate = directory->absoluteFilePath(pathTemplate);
    }
    QTemporaryFile tempFile(pathTemplate);
    if (! tempFile.open()) {
        throw synthclone::Error(tempFile.errorString());
    }
    tempFile.close();
    tempFile.setAutoRemove(false);
    return QFileInfo(tempFile).absoluteFilePath();
}

bool
getBooleanAttribute(const QDomElement &element, const QString &name)
{
    QString message;
    QString strValue = element.attribute(name);
    if (strValue.isEmpty()) {
        message = qApp->tr("'%1' element does not contain '%2' attribute").
            arg(element.tagName(), name);
        throw synthclone::Error(message);
    }
    if (strValue == "true") {
        return true;
    }
    if (strValue != "false") {
        message = qApp->tr("'%1' element contains non-boolean '%2' attribute").
            arg(element.tagName(), name);
        throw synthclone::Error(message);
    }
    return false;
}

quint32
getWholeNumberAttribute(const QDomElement &element, const QString &name,
                        quint32 minimumValue, quint32 maximumValue)
{
    QString message;
    QString strValue = element.attribute(name);
    if (strValue.isEmpty()) {
        message = qApp->tr("'%1' element does not contain '%2' attribute").
            arg(element.tagName(), name);
        throw synthclone::Error(message);
    }
    bool success;
    ulong uValue = strValue.toULong(&success);
    if (! success) {
        message = qApp->tr("'%1' element contains non-whole number '%2' "
                           "attribute").arg(element.tagName(), name);
        throw synthclone::Error(message);
    }
    if ((uValue < minimumValue) || (uValue > maximumValue)) {
        message = qApp->tr("'%1' element contains out of range '%2' attribute").
            arg(element.tagName(), name);
        throw synthclone::Error(message);
    }
    return static_cast<quint32>(uValue);
}

QVariant
readVariant(const QDomElement &element)
{
    QByteArray bytes = QByteArray::fromBase64(element.text().toLatin1());
    QString message;
    QDataStream stream(bytes);
    QVariant value;
    stream >> value;
    switch (stream.status()) {
    case QDataStream::ReadCorruptData:
        message = qApp->tr("encoded data contained by '%1' element is corrupt").
            arg(element.tagName());
        throw synthclone::Error(message);
    case QDataStream::ReadPastEnd:
        message = qApp->tr("read past end of encoded data in '%1' element").
            arg(element.tagName());
        throw synthclone::Error(message);
    default:
        ;
    }
    return value;
}

void
writeVariant(QXmlStreamWriter &writer, const QVariant &value)
{
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream << value;
    writer.writeCharacters(bytes.toBase64());
}

void
writeZone(QXmlStreamWriter &writer, Zone *zone, QDir *samplesDirectory,
          QObject *parent)
{
    writer.writeStartElement("zone");
    ulong uValue = static_cast<ulong>(zone->getAftertouch());
    writer.writeAttribute("aftertouch", QString::number(uValue));
    uValue = static_cast<ulong>(zone->getChannel());
    writer.writeAttribute("channel", QString::number(uValue));
    uValue = static_cast<ulong>(zone->getChannelPressure());
    writer.writeAttribute("channel-pressure", QString::number(uValue));
    writer.writeAttribute("dry-sample-stale",
                          zone->isDrySampleStale() ? "true" : "false");
    uValue = static_cast<ulong>(zone->getNote());
    writer.writeAttribute("note", QString::number(uValue));
    writer.writeAttribute("release-time",
                          QString::number(zone->getReleaseTime()));
    writer.writeAttribute("sample-time",
                          QString::number(zone->getSampleTime()));
    uValue = static_cast<ulong>(zone->getVelocity());
    writer.writeAttribute("velocity", QString::number(uValue));
    writer.writeAttribute("wet-sample-stale",
                          zone->isWetSampleStale() ? "true" : "false");

    const synthclone::Sample *drySample = zone->getDrySample();
    if (drySample) {
        if (samplesDirectory) {
            QFileInfo sampleInfo(drySample->getPath());
            QString name = sampleInfo.fileName();
            if (sampleInfo.absolutePath() != samplesDirectory->absolutePath()) {
                QString path = samplesDirectory->absoluteFilePath(name);
                synthclone::Sample *newDrySample =
                    new synthclone::Sample(*drySample, path, parent);
                zone->setDrySample(newDrySample);
            }
            writer.writeAttribute("dry-sample", name);
        } else {
            synthclone::Sample tempSample(*drySample);
            tempSample.setTemporary(false);
            writer.writeAttribute("dry-sample", tempSample.getPath());
        }
    }

    const synthclone::Sample *wetSample = zone->getWetSample();
    if (wetSample) {
        if (samplesDirectory) {
            QFileInfo sampleInfo(wetSample->getPath());
            QString name = sampleInfo.fileName();
            if (sampleInfo.absolutePath() != samplesDirectory->absolutePath()) {
                QString path = samplesDirectory->absoluteFilePath(name);
                synthclone::Sample *newWetSample =
                    new synthclone::Sample(*wetSample, path, parent);
                zone->setWetSample(newWetSample);
            }
            writer.writeAttribute("wet-sample", name);
        } else {
            synthclone::Sample tempSample(*drySample);
            tempSample.setTemporary(false);
            writer.writeAttribute("wet-sample", tempSample.getPath());
        }
    }

    // Zone controls
    writer.writeStartElement("controls");
    const Zone::ControlMap &controlMap = zone->getControlMap();
    Zone::ControlMap::const_iterator end = controlMap.constEnd();
    for (Zone::ControlMap::const_iterator iter = controlMap.constBegin();
         iter != end; iter++) {
        writer.writeStartElement("control");
        uValue = static_cast<ulong>(iter.key());
        writer.writeAttribute("index", QString::number(uValue));
        uValue = static_cast<ulong>(iter.value());
        writer.writeAttribute("value", QString::number(uValue));
        writer.writeEndElement();
    }
    writer.writeEndElement();

    writer.writeEndElement();
}
