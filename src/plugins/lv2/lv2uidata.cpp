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

#include <cassert>

#include "lv2uidata.h"

LV2UIData::LV2UIData(const LilvUI *ui, QObject *parent):
    QObject(parent)
{
    const LilvNode *node = lilv_ui_get_binary_uri(ui);
    assert(node);
    binaryPath = lilv_uri_to_path(lilv_node_as_uri(node));

    node = lilv_ui_get_bundle_uri(ui);
    assert(node);
    bundlePath = lilv_uri_to_path(lilv_node_as_uri(node));

    const LilvNodes *nodes = lilv_ui_get_classes(ui);
    LILV_FOREACH(nodes, iter, nodes) {
        node = lilv_nodes_get(nodes, iter);
        assert(node);
        typeURIs.append(lilv_node_as_uri(node));
    }

    node = lilv_ui_get_uri(ui);
    assert(node);
    uri = lilv_node_as_uri(node);
}

LV2UIData::~LV2UIData()
{
    // Empty
}

QString
LV2UIData::getBinaryPath() const
{
    return binaryPath;
}

QString
LV2UIData::getBundlePath() const
{
    return bundlePath;
}

QString
LV2UIData::getTypeURI(int index) const
{
    assert((index >= 0) && (index < typeURIs.count()));
    return typeURIs[index];
}

int
LV2UIData::getTypeURICount() const
{
    return typeURIs.count();
}

QString
LV2UIData::getURI() const
{
    return uri;
}
