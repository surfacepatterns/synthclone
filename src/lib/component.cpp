/*
 * libsynthclone - a plugin API for `synthclone`
 * Copyright (C) 2011 Devin Anderson
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <synthclone/component.h>

using synthclone::Component;

Component::Component(const QString &name, QObject *parent):
    QObject(parent)
{
    this->name = name;
}

Component::~Component()
{
    // Empty
}

QString
Component::getName() const
{
    return name;
}

void
Component::setName(const QString &name)
{
    if (this->name != name) {
        this->name = name;
        emit nameChanged(name);
    }
}
