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

#ifndef __ZONECOMPARERPROXY_H__
#define __ZONECOMPARERPROXY_H__

#include <synthclone/zonecomparer.h>

class ZoneComparerProxy: public QObject {

    Q_OBJECT

public:

    ZoneComparerProxy(const ZoneComparerProxy &proxy, QObject *parent=0);

    explicit
    ZoneComparerProxy(const synthclone::ZoneComparer &comparer,
                      QObject *parent=0);

    ~ZoneComparerProxy();

    bool
    operator()(const synthclone::Zone *zone1,
               const synthclone::Zone *zone2) const;

private:

    const synthclone::ZoneComparer &comparer;

};

#endif
