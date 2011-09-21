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

#include <cassert>

#include "zonecomparerproxy.h"

ZoneComparerProxy::ZoneComparerProxy(const ZoneComparerProxy &proxy,
                                     QObject *parent):
    QObject(parent),
    comparer(proxy.comparer)
{
    // Empty
}

ZoneComparerProxy::ZoneComparerProxy(const synthclone::ZoneComparer &comparer,
                                     QObject *parent):
    QObject(parent),
    comparer(comparer)
{
    // Empty
}

ZoneComparerProxy::~ZoneComparerProxy()
{
    // Empty
}

bool
ZoneComparerProxy::operator()(const synthclone::Zone *zone1,
                              const synthclone::Zone *zone2) const
{
    assert(zone1);
    assert(zone2);
    return comparer.isLessThan(zone1, zone2);
}
