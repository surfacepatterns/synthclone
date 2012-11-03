/*
 * libsynthclone_reverser - `synthclone` effect plugin that renders a sample
 * stream in reverse
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

#ifndef __EFFECT_H__
#define __EFFECT_H__

#include <synthclone/effect.h>

class Effect: public synthclone::Effect {

    Q_OBJECT

public:

    explicit
    Effect(const QString &name, QObject *parent=0);

    ~Effect();

    void
    process(const synthclone::Zone &zone,
            synthclone::SampleInputStream &inputStream,
            synthclone::SampleOutputStream &outputStream);

};

#endif
