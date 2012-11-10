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

#include <cassert>
#include <cerrno>

#include <QtCore/QFileInfo>
#include <QtCore/QTemporaryFile>

#include <synthclone/error.h>

#include "archivewriter.h"

// Static data

struct ArchiveDestructor {

    static void
    cleanup(zip *archive)
    {
        if (archive) {
            if (zip_close(archive) == -1) {
                throw synthclone::Error(QString("zip_close(): %1").
                                        arg(zip_strerror(archive)));
            }
        }
    }

};

zip_int64_t
ArchiveWriter::handleSourceCommand(void *writer, void *data,
                                   zip_uint64_t length, zip_source_cmd command)
{
    return static_cast<ArchiveWriter *>(writer)->
        handleSourceCommand(data, length, command);
}

// Class definition

ArchiveWriter::ArchiveWriter(const QString &path, const QString &instrumentName,
                             QObject *parent):
    QObject(parent)
{
    if (QFileInfo(path).exists()) {
        QFile file(path);
        if (! file.remove()) {
            throw synthclone::Error(tr("failed to remove file '%1': %2").
                                    arg(path).arg(file.errorString()));
        }
    }
    fileCount = 0;
    this->instrumentName = instrumentName;
    this->path = path;
}

ArchiveWriter::~ArchiveWriter()
{
    // Empty
}

void
ArchiveWriter::addConfiguration(const QString &configuration)
{
    QTemporaryFile tempFile;
    if (! tempFile.open()) {
        throw synthclone::Error(tr("failed to open temporary file: %1").
                                arg(tempFile.errorString()));
    }
    tempFile.write(configuration.toLocal8Bit());
    tempFile.close();

    addFile(tempFile.fileName(), "Instrument.xml", ZIP_CM_STORE);
    //addFile(tempFile.fileName(), "Instrument.xml", ZIP_CM_DEFLATE);
}

void
ArchiveWriter::addFile(const QString &path, const QString &entry,
                       zip_uint16_t compressionMethod)
{
    // Open the archive.
    QByteArray pathBytes = this->path.toLocal8Bit();
    int error;
    zip *archive = zip_open(pathBytes.constData(), ZIP_CREATE, &error);
    if (! archive) {
        char errorMessage[1024];
        zip_error_to_str(errorMessage, 1024, error, errno);
        throw synthclone::Error(tr("failed to open zip archive '%1': %2").
                                arg(this->path).arg(errorMessage));
    }
    QScopedPointer<zip, ArchiveDestructor> archivePtr(archive);

    // Prepare the source.
    zip_source *source = zip_source_function(archive, handleSourceCommand,
                                             this);
    if (! source) {
        throw synthclone::Error(tr("zip_source_function(): %1").
                                arg(zip_strerror(archive)));
    }
    sourceCompressionMethod = compressionMethod;
    sourceFile = new QFile(path);
    QScopedPointer<QFile> sourceFilePtr(sourceFile);

    // Add the source.
    QByteArray entryBytes = entry.toLocal8Bit();
    zip_int64_t result = zip_add(archive, entryBytes.constData(), source);
    if (result == -1) {
        zip_source_free(source);
        sourceFile = 0;
        throw synthclone::Error(tr("zip_add(): %1").arg(zip_strerror(archive)));
    }

    // Closing the archive gets the file added to the archive.
    archivePtr.reset();

    fileCount++;
}

void
ArchiveWriter::addSample(const QString &name, const synthclone::Sample &sample)
{
    QString n = QString::number(fileCount);
    if (n.count() == 1) {
        n = "0" + n;
    }
    QString entry = QString("SampleData/Sample%1 (%2).flac").arg(n).arg(name);
    addFile(sample.getPath(), entry, ZIP_CM_STORE);
}

zip_int64_t
ArchiveWriter::handleSourceCommand(void *data, zip_uint64_t length,
                                   zip_source_cmd command)
{
    int *errors;
    qint64 readResult;
    struct zip_stat *statData;
    switch (command) {
    case ZIP_SOURCE_CLOSE:
        sourceFile->close();
        break;

    case ZIP_SOURCE_ERROR:
        if (length < (sizeof(int) * 2)) {
            return -1;
        }
        errors = static_cast<int *>(data);
        errors[0] = sourceZipError;
        errors[1] = sourceSystemError;
        return sizeof(int) * 2;

    case ZIP_SOURCE_FREE:
        break;

    case ZIP_SOURCE_OPEN:
        if (! sourceFile->open(QIODevice::ReadOnly)) {
            sourceZipError = ZIP_ER_OPEN;
            sourceSystemError = errno;
            return -1;
        }
        break;

    case ZIP_SOURCE_READ:
        readResult = sourceFile->read(static_cast<char *>(data), length);
        if (readResult == -1) {
            sourceZipError = ZIP_ER_READ;
            sourceSystemError = errno;
        }
        return static_cast<zip_int64_t>(readResult);

    case ZIP_SOURCE_STAT:
        statData = static_cast<struct zip_stat *>(data);
        zip_stat_init(statData);
        statData->comp_method = sourceCompressionMethod;
        statData->size = QFileInfo(*sourceFile).size();
        statData->valid |= ZIP_STAT_COMP_METHOD | ZIP_STAT_SIZE;
        return sizeof(struct zip_stat);

    default:
        assert(false);
    }
    return 0;
}
