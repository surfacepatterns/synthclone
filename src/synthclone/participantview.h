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

#ifndef __PARTICIPANTVIEW_H__
#define __PARTICIPANTVIEW_H__

#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QStandardItemModel>
#include <QtGui/QTreeView>

#include "dialogview.h"
#include "participantviewlet.h"

class ParticipantView: public DialogView {

    Q_OBJECT

public:

    explicit
    ParticipantView(QObject *parent=0);

    ~ParticipantView();

    void
    add(ParticipantViewlet *viewlet);

    ParticipantViewlet *
    createParticipantViewlet();

    void
    destroyParticipantViewlet(ParticipantViewlet *viewlet);

    ParticipantViewlet *
    getChild(int index);

    int
    getChildCount() const;

    void
    remove(ParticipantViewlet *viewlet);

public slots:

    void
    setVisible(bool visible);

private slots:

    void
    handleTreeViewActivation(const QModelIndex &index);

    void
    registerViewlet(ParticipantViewlet *viewlet);

    void
    unregisterViewlet(ParticipantViewlet *viewlet);

private:

    QList<ParticipantViewlet *> children;
    QPushButton *closeButton;
    QMap<QStandardItem *, ParticipantViewlet *> itemViewletMap;
    QStandardItemModel treeModel;
    QTreeView *treeView;

};

#endif
