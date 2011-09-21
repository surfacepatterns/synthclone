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

#include "zoneindexcomparer.h"

ZoneIndexComparer::ZoneIndexComparer(QList<synthclone::Zone *> &zones,
                                     QObject *parent):
    synthclone::ZoneComparer(parent),
    zones(zones)
{
    // Empty
}

ZoneIndexComparer::~ZoneIndexComparer()
{
    // Empty
}

bool
ZoneIndexComparer::isLessThan(const synthclone::Zone *zone1,
                              const synthclone::Zone *zone2) const
{
    return zones.indexOf(const_cast<synthclone::Zone *>(zone1)) <
        zones.indexOf(const_cast<synthclone::Zone *>(zone2));
}
