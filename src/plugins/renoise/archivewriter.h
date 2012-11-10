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

#ifndef __ARCHIVEWRITER_H__
#define __ARCHIVEWRITER_H__

#include <zip.h>

#include <QtCore/QFile>

#include <synthclone/sample.h>

class ArchiveWriter: public QObject {

    Q_OBJECT

public:

    ArchiveWriter(const QString &path, const QString &instrumentName,
                  QObject *parent=0);

    ~ArchiveWriter();

    void
    addConfiguration(const QString &configuration);

    void
    addSample(const QString &fileName, const synthclone::Sample &sample);

private:

    void
    addFile(const QString &path, const QString &entry,
            zip_uint16_t compressionMethod);

    static zip_int64_t
    handleSourceCommand(void *writer, void *data, zip_uint64_t length,
                        zip_source_cmd command);

    zip_int64_t
    handleSourceCommand(void *data, zip_uint64_t length,
                        zip_source_cmd command);

    int fileCount;
    QString instrumentName;
    QString path;
    zip_uint16_t sourceCompressionMethod;
    QFile *sourceFile;
    int sourceSystemError;
    int sourceZipError;

};

#endif
