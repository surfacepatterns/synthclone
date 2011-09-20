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
