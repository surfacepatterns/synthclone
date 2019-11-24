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

#ifndef __EFFECTVIEW_H__
#define __EFFECTVIEW_H__

#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>

#include <synthclone/designerview.h>

class EffectView: public synthclone::DesignerView {

    Q_OBJECT

public:

    explicit
    EffectView(QObject *parent=0);

    ~EffectView();

public slots:

    void
    setFadeInEnabled(bool enabled);

    void
    setFadeInStartVolume(float volume);

    void
    setFadeInTime(float time);

    void
    setFadeOutEnabled(bool enabled);

    void
    setFadeOutEndVolume(float volume);

    void
    setFadeOutTime(float time);

    void
    setName(const QString &name);

signals:

    void
    fadeInEnabledChanged(bool enabled);

    void
    fadeInStartVolumeChanged(float volume);

    void
    fadeInTimeChanged(float time);

    void
    fadeOutEnabledChanged(bool enabled);

    void
    fadeOutEndVolumeChanged(float volume);

    void
    fadeOutTimeChanged(float time);

    void
    nameChanged(const QString &name);

private slots:

    void
    handleFadeInStartVolumeChangedSignal(double volume);

    void
    handleFadeInTimeChangedSignal(double time);

    void
    handleFadeOutEndVolumeChangedSignal(double volume);

    void
    handleFadeOutTimeChangedSignal(double time);

private:

    QPushButton *closeButton;
    QGroupBox *fadeInGroupBox;
    QDoubleSpinBox *fadeInStartVolume;
    QDoubleSpinBox *fadeInTime;
    QDoubleSpinBox *fadeOutEndVolume;
    QGroupBox *fadeOutGroupBox;
    QDoubleSpinBox *fadeOutTime;
    QLineEdit *name;

};

#endif
