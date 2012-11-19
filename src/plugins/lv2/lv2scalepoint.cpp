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

#include "lv2scalepoint.h"

LV2ScalePoint::LV2ScalePoint(const LilvScalePoint *point, QObject *parent):
    QObject(parent)
{
    const LilvNode *node = lilv_scale_point_get_label(point);
    assert(node);
    label = lilv_node_as_string(node);

    node = lilv_scale_point_get_value(point);
    assert(node);
    value = lilv_node_as_float(node);
}

LV2ScalePoint::~LV2ScalePoint()
{
    // Empty
}

QString
LV2ScalePoint::getLabel() const
{
    return label;
}

float
LV2ScalePoint::getValue() const
{
    return value;
}
