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

#include "archiveheader.h"

ArchiveHeader::ArchiveHeader(const QString &path, qint64 size, QObject *parent):
    QObject(parent)
{
    assert(size >= 0);
    this->path = path;
    this->size = size;
}

ArchiveHeader::~ArchiveHeader()
{
    // Empty
}

QString
ArchiveHeader::getPath() const
{
    return path;
}

qint64
ArchiveHeader::getSize() const
{
    return size;
}

bool
ArchiveHeader::isFile() const
{
    return static_cast<bool>(size);
}
