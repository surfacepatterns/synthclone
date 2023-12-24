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

#ifndef __PARTICIPANTVIEWLET_H__
#define __PARTICIPANTVIEWLET_H__

#include <QtCore/QList>

#include <QtGui/QStandardItem>

#include <QtWidgets/QTreeView>

class ParticipantView;

class ParticipantViewlet: public QObject {

    Q_OBJECT

    friend class ParticipantView;

public:

    explicit
    ParticipantViewlet(QTreeView *treeView, QObject *parent=0);

    ~ParticipantViewlet();

    void
    add(ParticipantViewlet *viewlet);

    ParticipantViewlet *
    getChild(int index);

    int
    getChildCount() const;

    void
    remove(ParticipantViewlet *viewlet);

    void
    setActivated(bool activated);

    void
    setAuthor(const QString &author);

    void
    setMajorVersion(int majorVersion);

    void
    setMinorVersion(int minorVersion);

    void
    setName(const QString &name);

    void
    setRevision(int revision);

    void
    setSummary(const QString &summary);

signals:

    void
    activationChangeRequest(bool activation);

    void
    childAdded(ParticipantViewlet *viewlet);

    void
    childRemoved(ParticipantViewlet *viewlet);

private:

    void
    emitActivationChangeRequest(bool active);

    void
    updateVersion();

    QList<ParticipantViewlet *> children;
    QList<QStandardItem *> items;
    int majorVersion;
    int minorVersion;
    int revision;
    QTreeView *treeView;

};

#endif
