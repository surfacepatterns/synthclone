#ifndef __UTIL_H__
#define __UTIL_H__

#include <limits>

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QXmlStreamWriter>
#include <QtGui/QWidget>
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
