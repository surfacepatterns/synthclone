/*
 * synthclone - Synthesizer-cloning software
 * Copyright (C) 2011-2012 Devin Anderson
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

#include <QtGui/QHeaderView>

#include <synthclone/util.h>

#include "participantview.h"
#include "types.h"

ParticipantView::ParticipantView(QObject *parent):
    DialogView(":/synthclone/participantview.ui", parent)
{
    closeButton = synthclone::getChild<QPushButton>(dialog, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    treeView = synthclone::getChild<QTreeView>(dialog, "treeView");
    treeView->setModel(&treeModel);
    treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(treeView, SIGNAL(activated(QModelIndex)),
            SLOT(handleTreeViewActivation(QModelIndex)));

    treeModel.setColumnCount(PARTICIPANTTREECOLUMN_TOTAL);
    treeModel.setRowCount(0);
    treeModel.setHeaderData(PARTICIPANTTREECOLUMN_AUTHOR, Qt::Horizontal,
                            tr("Author"));
    treeModel.setHeaderData(PARTICIPANTTREECOLUMN_NAME, Qt::Horizontal,
                            tr("Name"));
    treeModel.setHeaderData(PARTICIPANTTREECOLUMN_STATE, Qt::Horizontal,
                            tr("State"));
    treeModel.setHeaderData(PARTICIPANTTREECOLUMN_SUMMARY, Qt::Horizontal,
                            tr("Summary"));
    treeModel.setHeaderData(PARTICIPANTTREECOLUMN_VERSION, Qt::Horizontal,
                            tr("Version"));
}

ParticipantView::~ParticipantView()
{
    for (int i = children.count() - 1; i >= 0; i--) {
        remove(children[i]);
    }
}

void
ParticipantView::add(ParticipantViewlet *viewlet)
{
    assert(viewlet);
    children.append(viewlet);
    treeModel.appendRow(viewlet->items);
    registerViewlet(viewlet);
}

ParticipantViewlet *
ParticipantView::createParticipantViewlet()
{
    return new ParticipantViewlet(treeView, this);
}

void
ParticipantView::destroyParticipantViewlet(ParticipantViewlet *viewlet)
{
    delete viewlet;
}

ParticipantViewlet *
ParticipantView::getChild(int index)
{
    assert((index >= 0) && (index < children.count()));
    return children[index];
}

int
ParticipantView::getChildCount() const
{
    return children.count();
}

void
ParticipantView::handleTreeViewActivation(const QModelIndex &index)
{
    assert(index.isValid());
    QModelIndex parentIndex = index.parent();
    int row = index.row();
    QModelIndex stateIndex =
        treeModel.index(row, PARTICIPANTTREECOLUMN_STATE, parentIndex);
    ParticipantViewlet *viewlet =
        itemViewletMap.value(treeModel.itemFromIndex(stateIndex), 0);
    assert(viewlet);
    bool active = treeModel.data(stateIndex, Qt::UserRole).toBool();
    viewlet->emitActivationChangeRequest(! active);
}

void
ParticipantView::registerViewlet(ParticipantViewlet *viewlet)
{
    assert(! viewlet->getChildCount());
    itemViewletMap.insert(viewlet->items[PARTICIPANTTREECOLUMN_STATE], viewlet);
    connect(viewlet, SIGNAL(childAdded(ParticipantViewlet *)),
            SLOT(registerViewlet(ParticipantViewlet *)));
    connect(viewlet, SIGNAL(childRemoved(ParticipantViewlet *)),
            SLOT(unregisterViewlet(ParticipantViewlet *)));
}

void
ParticipantView::remove(ParticipantViewlet *viewlet)
{
    assert(viewlet);
    assert(! viewlet->getChildCount());
    int index = children.indexOf(viewlet);
    assert(index != -1);
    unregisterViewlet(viewlet);
    bool removed = treeModel.removeRow(viewlet->items[0]->row());
    assert(removed);
    children.removeAt(index);
}

void
ParticipantView::setVisible(bool visible)
{
    if (visible) {
        treeView->header()->resizeSections(QHeaderView::ResizeToContents);
    }
    View::setVisible(visible);
}

void
ParticipantView::unregisterViewlet(ParticipantViewlet *viewlet)
{
    disconnect(viewlet, SIGNAL(childAdded(ParticipantViewlet *)),
               this, SLOT(registerViewlet(ParticipantViewlet *)));
    disconnect(viewlet, SIGNAL(childRemoved(ParticipantViewlet *)),
               this, SLOT(unregisterViewlet(ParticipantViewlet *)));
    bool removed =
        itemViewletMap.remove(viewlet->items[PARTICIPANTTREECOLUMN_STATE]);
    assert(removed);
}
