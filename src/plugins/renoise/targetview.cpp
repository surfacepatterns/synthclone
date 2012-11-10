/*
 * libsynthclone_renoise - Renoise target plugin for `synthclone`
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

#include <cassert>

#include <synthclone/util.h>

#include "targetview.h"

TargetView::TargetView(QObject *parent):
    synthclone::DesignerView(":/synthclone/plugins/renoise/targetview.ui",
                             parent)
{
    QWidget *widget = getRootWidget();

    closeButton = synthclone::getChild<QPushButton>(widget, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    drumKit = synthclone::getChild<QCheckBox>(widget, "drumKit");
    connect(drumKit, SIGNAL(clicked(bool)), SIGNAL(drumKitChangeRequest(bool)));

    instrumentName = synthclone::getChild<QLineEdit>(widget, "instrumentName");
    connect(instrumentName, SIGNAL(textEdited(const QString &)),
            SIGNAL(instrumentNameChangeRequest(const QString &)));

    layerAlgorithmComboBox = synthclone::getChild<QComboBox>
        (widget, "layerAlgorithmComboBox");
    connect(layerAlgorithmComboBox, SIGNAL(currentIndexChanged(int)),
            SLOT(handleLayerAlgorithmChange(int)));

    name = synthclone::getChild<QLineEdit>(widget, "name");
    connect(name, SIGNAL(textEdited(const QString &)),
            SIGNAL(nameChangeRequest(const QString &)));

    path = synthclone::getChild<QLineEdit>(widget, "path");
    connect(path, SIGNAL(textEdited(const QString &)),
            SIGNAL(pathChangeRequest(const QString &)));

    pathLookupButton = synthclone::getChild<QPushButton>
        (widget, "pathLookupButton");
    connect(pathLookupButton, SIGNAL(clicked()), SIGNAL(pathLookupRequest()));

    pitchInterpolationComboBox = synthclone::getChild<QComboBox>
        (widget, "pitchInterpolationComboBox");
    connect(pitchInterpolationComboBox, SIGNAL(currentIndexChanged(int)),
            SLOT(handlePitchInterpolationChange(int)));
}

TargetView::~TargetView()
{
    // Empty
}

void
TargetView::handleLayerAlgorithmChange(int index)
{
    emit layerAlgorithmChangeRequest(static_cast<LayerAlgorithm>(index));
}

void
TargetView::handlePitchInterpolationChange(int index)
{
    emit pitchInterpolationChangeRequest
        (static_cast<PitchInterpolation>(index));
}

void
TargetView::setDrumKit(bool drumKit)
{
    this->drumKit->setChecked(drumKit);
}

void
TargetView::setInstrumentName(const QString &name)
{
    instrumentName->setText(name);
}

void
TargetView::setLayerAlgorithm(LayerAlgorithm algorithm)
{
    layerAlgorithmComboBox->setCurrentIndex(static_cast<int>(algorithm));
}

void
TargetView::setName(const QString &name)
{
    this->name->setText(name);
}

void
TargetView::setPath(const QString &path)
{
    this->path->setText(path);
}

void
TargetView::setPitchInterpolation(PitchInterpolation interpolation)
{
    pitchInterpolationComboBox->setCurrentIndex
        (static_cast<int>(interpolation));
}
