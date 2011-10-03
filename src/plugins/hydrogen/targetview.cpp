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

#include <cassert>

#include <synthclone/util.h>

#include "targetview.h"

TargetView::TargetView(QObject *parent):
    synthclone::DesignerView(":/synthclone/plugins/hydrogen/targetview.ui",
                             parent)
{
    QWidget *widget = getRootWidget();

    author = synthclone::getChild<QLineEdit>(widget, "author");
    connect(author, SIGNAL(textEdited(const QString &)),
            SIGNAL(authorChangeRequest(const QString &)));

    closeButton = synthclone::getChild<QPushButton>(widget, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    info = synthclone::getChild<QPlainTextEdit>(widget, "info");
    connect(info, SIGNAL(textChanged()), SLOT(handleInfoChange()));

    kitName = synthclone::getChild<QLineEdit>(widget, "kitName");
    connect(kitName, SIGNAL(textEdited(const QString &)),
            SIGNAL(kitNameChangeRequest(const QString &)));

    layerAlgorithmComboBox = synthclone::getChild<QComboBox>
        (widget, "layerAlgorithmComboBox");
    connect(layerAlgorithmComboBox, SIGNAL(currentIndexChanged(int)),
            SLOT(handleLayerAlgorithmChange(int)));

    license = synthclone::getChild<QLineEdit>(widget, "license");
    connect(license, SIGNAL(textEdited(const QString &)),
            SIGNAL(licenseChangeRequest(const QString &)));

    name = synthclone::getChild<QLineEdit>(widget, "name");
    connect(name, SIGNAL(textEdited(const QString &)),
            SIGNAL(nameChangeRequest(const QString &)));

    path = synthclone::getChild<QLineEdit>(widget, "path");
    connect(path, SIGNAL(textEdited(const QString &)),
            SIGNAL(pathChangeRequest(const QString &)));

    pathLookupButton = synthclone::getChild<QPushButton>
        (widget, "pathLookupButton");
    connect(pathLookupButton, SIGNAL(clicked()), SIGNAL(pathLookupRequest()));

    sampleFormatComboBox = synthclone::getChild<QComboBox>
        (widget, "sampleFormatComboBox");
    connect(sampleFormatComboBox, SIGNAL(currentIndexChanged(int)),
            SLOT(handleSampleFormatChange(int)));
}

TargetView::~TargetView()
{
    // Empty
}

void
TargetView::handleInfoChange()
{
    emit infoChangeRequest(info->toPlainText());
}

void
TargetView::handleLayerAlgorithmChange(int index)
{
    emit layerAlgorithmChangeRequest(static_cast<LayerAlgorithm>(index));
}

void
TargetView::handleSampleFormatChange(int index)
{
    emit sampleFormatChangeRequest(static_cast<SampleFormat>(index));
}

void
TargetView::setAuthor(const QString &author)
{
    this->author->setText(author);
}

void
TargetView::setInfo(const QString &info)
{
    if (this->info->toPlainText() != info) {
        this->info->setPlainText(info);
    }
}

void
TargetView::setKitName(const QString &name)
{
    kitName->setText(name);
}

void
TargetView::setLayerAlgorithm(LayerAlgorithm algorithm)
{
    layerAlgorithmComboBox->setCurrentIndex(static_cast<int>(algorithm));
}

void
TargetView::setLicense(const QString &license)
{
    this->license->setText(license);
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
TargetView::setSampleFormat(SampleFormat format)
{
    sampleFormatComboBox->setCurrentIndex(static_cast<int>(format));
}
