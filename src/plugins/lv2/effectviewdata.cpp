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

#include "effectviewdata.h"

EffectViewData::EffectViewData(const QString &pluginURI, QObject *parent):
    QObject(parent)
{
    this->pluginURI = pluginURI;
    provided = false;
}

EffectViewData::EffectViewData(const QString &uri, const QString &typeURI,
                               const QString &binaryPath,
                               const QString &bundlePath,
                               const QString &pluginURI, QObject *parent):
    QObject(parent)
{
    this->binaryPath = binaryPath;
    this->bundlePath = bundlePath;
    this->pluginURI = pluginURI;
    provided = true;
    this->typeURI = typeURI;
    this->uri = uri;
}

EffectViewData::~EffectViewData()
{
    // Empty
}

QString
EffectViewData::getBinaryPath() const
{
    return binaryPath;
}

QString
EffectViewData::getBundlePath() const
{
    return bundlePath;
}

QString
EffectViewData::getPluginURI() const
{
    return pluginURI;
}

QString
EffectViewData::getTypeURI() const
{
    return typeURI;
}

QString
EffectViewData::getURI() const
{
    return uri;
}

bool
EffectViewData::isProvided() const
{
    return provided;
}
