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

#ifndef __EFFECTVIEW_H__
#define __EFFECTVIEW_H__

#include <QCheckBox>
#include <QDoubleSpinBox>
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
    setName(const QString &name);

    void
    setSampleFloor(float sampleFloor);

    void
    setSampleFloorEditorEnabled(bool enabled);

    void
    setTrimEnd(bool trimEnd);

    void
    setTrimStart(bool trimStart);

signals:

    void
    nameChanged(const QString &name);

    void
    sampleFloorChanged(float sampleFloor);

    void
    trimEndChanged(bool trimEnd);

    void
    trimStartChanged(bool trimStart);

private slots:

    void
    handleSampleFloorValueChangedSignal(double value);

private:

    QPushButton *closeButton;
    QLineEdit *name;
    QDoubleSpinBox *sampleFloor;
    QCheckBox *trimEnd;
    QCheckBox *trimStart;

};

#endif
