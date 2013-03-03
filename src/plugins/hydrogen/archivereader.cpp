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
#include <stdexcept>

#include <QtCore/QDebug>

#include <archive_entry.h>

#include <synthclone/error.h>

#include "archivereader.h"

ArchiveReader::ArchiveReader(const QString &path, QObject *parent):
    QObject(parent)
{
    arch = archive_read_new();
    if (! arch) {
        throw std::bad_alloc();
    }
    try {
        if (archive_read_support_compression_gzip(arch) != ARCHIVE_OK) {
            throw synthclone::Error(archive_error_string(arch));
        }
        if (archive_read_support_format_tar(arch) != ARCHIVE_OK) {
            throw synthclone::Error(archive_error_string(arch));
        }
        QByteArray pathBytes = path.toLocal8Bit();
        if (archive_read_open_filename(arch, pathBytes.constData(), 8192) !=
            ARCHIVE_OK) {
            throw synthclone::Error(archive_error_string(arch));
        }
    } catch (...) {
        archive_read_finish(arch);
        throw;
    }
    closed = false;
    header = 0;
}

ArchiveReader::~ArchiveReader()
{
    if (! closed) {
        try {
            close();
        } catch (synthclone::Error &e) {
            qWarning() << tr("Error in ArchiveReader destructor: %1").
                arg(e.getMessage());
        }
    }
    archive_read_finish(arch);
    if (header) {
        delete header;
    }
}

void
ArchiveReader::close()
{
    assert(! closed);
    if (archive_read_close(arch) != ARCHIVE_OK) {
        throw synthclone::Error(archive_error_string(arch));
    }
    closed = true;
}

size_t
ArchiveReader::readData(char *buffer, size_t size)
{
    assert(buffer);
    assert(size);

    ssize_t result = archive_read_data(arch, buffer, size);
    if (result < 0) {
        throw synthclone::Error(archive_error_string(arch));
    }
    return static_cast<size_t>(result);
}

const ArchiveHeader *
ArchiveReader::readHeader()
{
    struct archive_entry *entry;
    switch (archive_read_next_header(arch, &entry)) {
    case ARCHIVE_EOF:
        return 0;
    case ARCHIVE_OK:
        break;
    default:
        throw synthclone::Error(archive_error_string(arch));
    }

    const char *path = archive_entry_pathname(entry);
    assert(path);
    qint64 size = static_cast<qint64>(archive_entry_size(entry));
    assert(size >= 0);

    if (! header) {
        header = new ArchiveHeader(path, size, this);
    } else {
        header->~ArchiveHeader();
        header = new (header) ArchiveHeader(path, size, this);
    }

    return header;
}

void
ArchiveReader::skipData()
{
    if (archive_read_data_skip(arch) < 0) {
        throw synthclone::Error(archive_error_string(arch));
    }
}
