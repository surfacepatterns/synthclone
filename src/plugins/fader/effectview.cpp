/*
 * libsynthclone_fader - Fader effect plugin for `synthclone`
 * Copyright (C) 2013 Devin Anderson
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
    synthclone::DesignerView(":/synthclone/plugins/fader/effectview.ui", parent)
{
    QWidget *widget = getRootWidget();

    closeButton = synthclone::getChild<QPushButton>(widget, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    fadeInGroupBox = synthclone::getChild<QGroupBox>(widget, "fadeInGroupBox");
    connect(fadeInGroupBox, SIGNAL(clicked(bool)),
            SIGNAL(fadeInEnabledChanged(bool)));

    fadeInStartVolume =
        synthclone::getChild<QDoubleSpinBox>(widget, "fadeInStartVolume");
    connect(fadeInStartVolume, SIGNAL(valueChanged(double)),
            SLOT(handleFadeInStartVolumeChangedSignal(double)));

    fadeInTime = synthclone::getChild<QDoubleSpinBox>(widget, "fadeInTime");
    connect(fadeInTime, SIGNAL(valueChanged(double)),
            SLOT(handleFadeInTimeChangedSignal(double)));

    fadeOutEndVolume =
        synthclone::getChild<QDoubleSpinBox>(widget, "fadeOutEndVolume");
    connect(fadeOutEndVolume, SIGNAL(valueChanged(double)),
            SLOT(handleFadeOutEndVolumeChangedSignal(double)));

    fadeOutGroupBox =
        synthclone::getChild<QGroupBox>(widget, "fadeOutGroupBox");
    connect(fadeOutGroupBox, SIGNAL(clicked(bool)),
            SIGNAL(fadeOutEnabledChanged(bool)));

    fadeOutTime = synthclone::getChild<QDoubleSpinBox>(widget, "fadeOutTime");
    connect(fadeOutTime, SIGNAL(valueChanged(double)),
            SLOT(handleFadeOutTimeChangedSignal(double)));

    name = synthclone::getChild<QLineEdit>(widget, "name");
    connect(name, SIGNAL(textEdited(const QString &)),
            SIGNAL(nameChanged(const QString &)));
}

EffectView::~EffectView()
{
    // Empty
}

void
EffectView::handleFadeInStartVolumeChangedSignal(double volume)
{
    emit fadeInStartVolumeChanged(static_cast<float>(volume));
}

void
EffectView::handleFadeInTimeChangedSignal(double time)
{
    emit fadeInTimeChanged(static_cast<float>(time));
}

void
EffectView::handleFadeOutEndVolumeChangedSignal(double volume)
{
    emit fadeOutEndVolumeChanged(static_cast<float>(volume));
}

void
EffectView::handleFadeOutTimeChangedSignal(double time)
{
    emit fadeOutTimeChanged(static_cast<float>(time));
}

void
EffectView::setFadeInEnabled(bool enabled)
{
    fadeInGroupBox->setChecked(enabled);
}

void
EffectView::setFadeInStartVolume(float volume)
{
    fadeInStartVolume->setValue(volume);
}

void
EffectView::setFadeInTime(float time)
{
    fadeInTime->setValue(time);
}

void
EffectView::setFadeOutEnabled(bool enabled)
{
    fadeOutGroupBox->setChecked(enabled);
}

void
EffectView::setFadeOutEndVolume(float volume)
{
    fadeOutEndVolume->setValue(volume);
}

void
EffectView::setFadeOutTime(float time)
{
    fadeOutTime->setValue(time);
}

void
EffectView::setName(const QString &name)
{
    this->name->setText(name);
}
