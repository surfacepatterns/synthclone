/*
 * libsynthclone_hydrogen - Hydrogen target plugin for `synthclone`
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

#ifndef __TARGETVIEW_H__
#define __TARGETVIEW_H__

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>

#include <synthclone/designerview.h>

#include "types.h"

class TargetView: public synthclone::DesignerView {

    Q_OBJECT

public:

    explicit
    TargetView(QObject *parent=0);

    ~TargetView();

public slots:

    void
    setAuthor(const QString &author);

    void
    setInfo(const QString &info);

    void
    setKitName(const QString &name);

    void
    setLayerAlgorithm(LayerAlgorithm algorithm);

    void
    setLicense(const QString &license);

    void
    setName(const QString &name);

    void
    setPath(const QString &path);

    void
    setSampleFormat(SampleFormat format);

signals:

    void
    authorChangeRequest(const QString &author);

    void
    infoChangeRequest(const QString &info);

    void
    kitNameChangeRequest(const QString &name);

    void
    layerAlgorithmChangeRequest(LayerAlgorithm algorithm);

    void
    licenseChangeRequest(const QString &license);

    void
    nameChangeRequest(const QString &name);

    void
    pathChangeRequest(const QString &path);

    void
    pathLookupRequest();

    void
    sampleFormatChangeRequest(SampleFormat format);

private slots:

    void
    handleInfoChange();

    void
    handleLayerAlgorithmChange(int index);

    void
    handleSampleFormatChange(int index);

private:

    QLineEdit *author;
    QPushButton *closeButton;
    QPlainTextEdit *info;
    QLineEdit *kitName;
    QComboBox *layerAlgorithmComboBox;
    QLineEdit *license;
    QLineEdit *name;
    QLineEdit *path;
    QPushButton *pathLookupButton;
    QComboBox *sampleFormatComboBox;

};

#endif
