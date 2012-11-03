/*
 * libsynthclone_lv2 - LV2 effect plugin for `synthclone`
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

#include "lv2urimap.h"

// Static functions

LV2_URID
LV2URIMap::getId(LV2_URID_Map_Handle handle, const char *uri)
{
    return static_cast<LV2URIMap *>(handle)->getId(uri);
}

const char *
LV2URIMap::getURI(LV2_URID_Unmap_Handle handle, LV2_URID id)
{
    return static_cast<LV2URIMap *>(handle)->getURI(id);
}

// Class definition

LV2URIMap::LV2URIMap(QObject *parent):
    QObject(parent)
{
    uridMap.handle = this;
    uridMap.map = getId;
    uridUnmap.handle = this;
    uridUnmap.unmap = getURI;
}

LV2URIMap::~LV2URIMap()
{
    // Empty
}

LV2_URID
LV2URIMap::getId(const char *uri)
{
    QByteArray uriBytes(uri);
    int index = uris.indexOf(uriBytes);
    if (index == -1) {
        uris.append(uriBytes);
        index = uris.count() - 1;
    }
    return static_cast<LV2_URID>(index);
}

LV2_URID_Map *
LV2URIMap::getMap()
{
    return &uridMap;
}

LV2_URID_Unmap *
LV2URIMap::getUnmap()
{
    return &uridUnmap;
}

const char *
LV2URIMap::getURI(LV2_URID id) const
{
    if (static_cast<int>(id) >= uris.count()) {
        return 0;
    }
    const QByteArray &uriBytes = uris.at(id);
    return uriBytes.constData();
}
