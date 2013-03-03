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

#include <cerrno>
#include <cstring>

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>

#include <synthclone/error.h>

#include "temporarydir.h"

#if defined(SYNTHCLONE_PLATFORM_MACX) || defined(SYNTHCLONE_PLATFORM_UNIX)
#include <cstdlib>
#else
#error "A temp directory implementation has not been defined for your platform"
#endif

TemporaryDir::TemporaryDir(QObject *parent):
    QObject(parent)
{
    QDir tempDir(QDir::tempPath());

#if SYNTHCLONE_PLATFORM_MACX || SYNTHCLONE_PLATFORM_UNIX
    QByteArray templateBytes = tempDir.absoluteFilePath("XXXXXX").toLocal8Bit();
    if (! mkdtemp(templateBytes.data())) {
        throw synthclone::Error(tr("failed to create temporary directory: %1").
                                arg(strerror(errno)));
    }
    path = QString(templateBytes);
#endif

}

TemporaryDir::~TemporaryDir()
{
    try {
        remove(path);
    } catch (synthclone::Error &e) {
        qWarning() << e.getMessage();
    }
}

QString
TemporaryDir::getPath() const
{
    return path;
}

void
TemporaryDir::remove(const QString &path)
{
    QDir directory(path);
    Q_FOREACH(QFileInfo info,
              directory.entryInfoList(QDir::NoDotAndDotDot | QDir::System |
                                      QDir::Hidden | QDir::AllDirs |
                                      QDir::Files)) {
        QString subPath = info.absoluteFilePath();
        if (info.isDir()) {
            remove(subPath);
        } else {
            QFile file(subPath);
            if (! file.remove()) {
                throw synthclone::Error(file.errorString());
            }
        }
    }
    if (! directory.rmdir(path)) {
        throw synthclone::Error(tr("failed to remove '%1': %2").
                                arg(path).arg(strerror(errno)));
    }
}
