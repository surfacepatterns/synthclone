/*
 * libsynthclone - a plugin API for `synthclone`
 * Copyright (C) 2011 Devin Anderson
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QTemporaryFile>

#include <synthclone/error.h>
#include <synthclone/sample.h>

using synthclone::Sample;

Sample::Sample(bool temporary, QObject *parent):
    QObject(parent)
{
    initializeTemporaryPath();
    this->temporary = temporary;
}

Sample::Sample(const QString &path, bool temporary, QObject *parent):
    QObject(parent)
{
    this->path = path;
    this->temporary = temporary;
}

Sample::Sample(const Sample &sample, bool temporary, QObject *parent):
    QObject(parent)
{
    initializeTemporaryPath();
    initializeData(sample);
    this->temporary = temporary;
}

Sample::Sample(const Sample &sample, const QString &path, bool temporary,
               QObject *parent):
    QObject(parent)
{
    this->path = path;
    initializeData(sample);
    this->temporary = temporary;
}

Sample::~Sample()
{
    QFile file(path);
    if (temporary && file.exists()) {
        if (! file.remove()) {
            qWarning() << tr("failed to remove '%1': %2").
                arg(path, file.errorString());
        }
    }
}

QString
Sample::getPath() const
{
    return path;
}

void
Sample::initializeData(const Sample &sample)
{
    QFile destinationFile(path);
    QString message;
    QFile sourceFile(sample.path);

    if (! destinationFile.open(QFile::WriteOnly)) {
        message = tr("could not open '%1': %2").
            arg(path, destinationFile.errorString());
        throw Error(message);
    }
    if (! sourceFile.open(QFile::ReadOnly)) {
        destinationFile.close();
        message = tr("could not open '%1': %2").
            arg(sample.path, sourceFile.errorString());
        throw Error(message);
    }

    for (;;) {
        QByteArray data = sourceFile.read(8192);
        if (data.isEmpty()) {
            break;
        }
        destinationFile.write(data);
    }

    destinationFile.close();
    sourceFile.close();
}

void
Sample::initializeTemporaryPath()
{
    QTemporaryFile file("");
    if (! file.open()) {
        QString message = tr("could not open temporary file: '%1'").
            arg(file.errorString());
        throw synthclone::Error(message);
    }
    file.close();
    file.setAutoRemove(false);
    path = QFileInfo(file).absoluteFilePath();
}

bool
Sample::isTemporary() const
{
    return temporary;
}

void
Sample::setTemporary(bool temporary)
{
    if (this->temporary != temporary) {
        this->temporary = temporary;
        emit temporaryChanged(temporary);
    }
}
