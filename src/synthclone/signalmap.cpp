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

#include "signalmap.h"

SignalMap::SignalMap(QObject *parent):
    QObject(parent)
{
    // Empty
}

SignalMap::SignalMap(const SignalMap &map, QObject *parent):
    QObject(parent)
{
    this->map.unite(map.map);
}

SignalMap::~SignalMap()
{
    // Empty
}

SignalMap &
SignalMap::operator<<(const SignalPair &pair)
{
    map.insert(pair.first, pair.second);
    return *this;
}

const char *
SignalMap::get(const QLatin1String &signal) const
{
    return map.value(signal, 0);
}
