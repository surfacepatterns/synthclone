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

#ifndef __SAMPLERJOB_H__
#define __SAMPLERJOB_H__

#include <synthclone/samplerjob.h>

#include "zone.h"

class SamplerJob: public synthclone::SamplerJob {

    Q_OBJECT

public:

    SamplerJob(Type type, Zone *zone, QObject *parent=0);

    ~SamplerJob();

    Type
    getType() const;

    Zone *
    getZone();

    const synthclone::Zone *
    getZone() const;

private:

    Type type;
    Zone *zone;

};

#endif
