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

#ifndef __UTIL_H__
#define __UTIL_H__

#include <limits>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QXmlStreamWriter>
#include <QWidget>
#include <QtXml/QDomElement>

#include <synthclone/sample.h>
#include <synthclone/types.h>

#include "zone.h"

QString
createUniqueFile(const QDir *directory=0, const QString &prefix=QString(),
                 const QString &suffix=QString());

bool
getBooleanAttribute(const QDomElement &element, const QString &name);

quint32
getWholeNumberAttribute(const QDomElement &element, const QString &name,
                        quint32 minimumValue=0,
                        quint32
                        maximumValue=std::numeric_limits<quint32>::max());

QVariant
readVariant(const QDomElement &element);

void
writeVariant(QXmlStreamWriter &writer, const QVariant &value);

void
writeZone(QXmlStreamWriter &writer, Zone *zone, QDir *samplesDirectory=0,
          QObject *parent=0);

#endif
