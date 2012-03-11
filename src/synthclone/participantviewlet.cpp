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

#include <QtCore/QLocale>

#include "participantviewlet.h"
#include "types.h"

ParticipantViewlet::ParticipantViewlet(QTreeView *treeView, QObject *parent):
    QObject(parent)
{
    for (int i = PARTICIPANTTREECOLUMN_TOTAL - 1; i >= 0; i--) {
        QStandardItem *item = new QStandardItem();
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
        items.append(item);
    }
    this->treeView = treeView;
    majorVersion = 0;
    minorVersion = 0;
    revision = 0;
    setActivated(false);
    setAuthor("");
    setName("");
    setSummary("");
    updateVersion();
}

ParticipantViewlet::~ParticipantViewlet()
{
    for (int i = children.count() - 1; i >= 0; i--) {
        remove(children[i]);
    }
}

void
ParticipantViewlet::add(ParticipantViewlet *viewlet)
{
    assert(viewlet);
    children.append(viewlet);
    this->items[0]->appendRow(viewlet->items);
    emit childAdded(viewlet);
}

void
ParticipantViewlet::emitActivationChangeRequest(bool active)
{
    emit activationChangeRequest(active);
}

ParticipantViewlet *
ParticipantViewlet::getChild(int index)
{
    assert((index >= 0) && (index < children.count()));
    return children[index];
}

int
ParticipantViewlet::getChildCount() const
{
    return children.count();
}

void
ParticipantViewlet::remove(ParticipantViewlet *viewlet)
{
    children.removeOne(viewlet);
    items[0]->takeRow(viewlet->items[0]->row());
    emit childRemoved(viewlet);
}

void
ParticipantViewlet::setActivated(bool activated)
{
    QStandardItem *item = items[PARTICIPANTTREECOLUMN_STATE];
    QString path;
    item->setData(activated, Qt::UserRole);
    if (activated) {
        item->setData(tr("Activated"), Qt::DisplayRole);
        path = ":/synthclone/images/16x16/ok.png";
    } else {
        item->setData(tr("Not Activated"), Qt::DisplayRole);
        path = ":/synthclone/images/16x16/delete.png";
    }
    item->setData(QIcon(path), Qt::DecorationRole);
}

void
ParticipantViewlet::setAuthor(const QString &author)
{
    items[PARTICIPANTTREECOLUMN_AUTHOR]->setData(author, Qt::DisplayRole);
}

void
ParticipantViewlet::setMajorVersion(int majorVersion)
{
    this->majorVersion = majorVersion;
    updateVersion();
}

void
ParticipantViewlet::setMinorVersion(int minorVersion)
{
    this->minorVersion = minorVersion;
    updateVersion();
}

void
ParticipantViewlet::setName(const QString &name)
{
    items[PARTICIPANTTREECOLUMN_NAME]->setData(name, Qt::DisplayRole);
}

void
ParticipantViewlet::setRevision(int revision)
{
    this->revision = revision;
    updateVersion();
}

void
ParticipantViewlet::setSummary(const QString &summary)
{
    items[PARTICIPANTTREECOLUMN_SUMMARY]->setData(summary, Qt::DisplayRole);
}

void
ParticipantViewlet::updateVersion()
{
    QLocale locale = QLocale::system();
    items[PARTICIPANTTREECOLUMN_VERSION]->
        setData(tr("%1.%2-%3", "participantVersionFormat").
                arg(locale.toString(majorVersion)).
                arg(locale.toString(minorVersion)).
                arg(locale.toString(revision)), Qt::DisplayRole);
}
