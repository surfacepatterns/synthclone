/*
 * libsynthclone_trimmer - Sample trimmer effect plugin for `synthclone`
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

#include <synthclone/util.h>

#include "effectview.h"

EffectView::EffectView(QObject *parent):
    synthclone::DesignerView(":/synthclone/plugins/trimmer/effectview.ui",
                             parent)
{
    QWidget *widget = getRootWidget();

    closeButton = synthclone::getChild<QPushButton>(widget, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    name = synthclone::getChild<QLineEdit>(widget, "name");
    connect(name, SIGNAL(textEdited(const QString &)),
            SIGNAL(nameChanged(const QString &)));

    sampleFloor = synthclone::getChild<QDoubleSpinBox>(widget, "sampleFloor");
    connect(sampleFloor, SIGNAL(valueChanged(double)),
            SLOT(handleSampleFloorValueChangedSignal(double)));

    trimEnd = synthclone::getChild<QCheckBox>(widget, "trimSampleEnd");
    connect(trimEnd, SIGNAL(toggled(bool)), SIGNAL(trimEndChanged(bool)));

    trimStart = synthclone::getChild<QCheckBox>(widget, "trimSampleStart");
    connect(trimStart, SIGNAL(toggled(bool)), SIGNAL(trimStartChanged(bool)));
}

EffectView::~EffectView()
{
    // Empty
}

void
EffectView::handleSampleFloorValueChangedSignal(double value)
{
    emit sampleFloorChanged(static_cast<float>(value));
}

void
EffectView::setName(const QString &name)
{
    this->name->setText(name);
}

void
EffectView::setSampleFloor(float value)
{
    sampleFloor->setValue(value);
}

void
EffectView::setSampleFloorEditorEnabled(bool enabled)
{
    sampleFloor->setEnabled(enabled);
}

void
EffectView::setTrimEnd(bool value)
{
    trimEnd->setChecked(value);
}

void
EffectView::setTrimStart(bool value)
{
    trimStart->setChecked(value);
}
