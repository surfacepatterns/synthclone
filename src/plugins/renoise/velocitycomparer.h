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

#ifndef __VELOCITYCOMPARER_H__
#define __VELOCITYCOMPARER_H__

#include <synthclone/zone.h>

class VelocityComparer: public QObject {

    Q_OBJECT

public:

    explicit
    VelocityComparer(QObject *parent=0);

    VelocityComparer(const VelocityComparer &comparer, QObject *parent=0);

    ~VelocityComparer();

    bool
    operator()(const synthclone::Zone *zone1,
               const synthclone::Zone *zone2) const;

};

#endif
