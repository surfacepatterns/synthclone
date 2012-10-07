/*
 * libsynthclone_lv2 - LV2 effect plugin for `synthclone`
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

#include <QtCore/QDebug>
#include <QtGui/QHeaderView>
#include <QtGui/QLayout>

#include <synthclone/error.h>
#include <synthclone/util.h>

#include "effectview.h"
#include "types.h"

// Static functions

void
EffectView::handlePortWriteRequest(SuilController controller,
                                   uint32_t portIndex, uint32_t bufferSize,
                                   uint32_t protocol, void const *buffer)
{
    static_cast<EffectView *>(controller)->
        handlePortWriteRequest(portIndex, bufferSize, protocol, buffer);
}

// Class definition

EffectView::EffectView(QObject *parent):
    synthclone::DesignerView(":/synthclone/plugins/lv2/effectview.ui", parent)
{
    host = suil_host_new(handlePortWriteRequest, 0, 0, 0);
    assert(host);
    instance = 0;

    QWidget *widget = getRootWidget();

    connect(&channelMapTableDelegate,
            SIGNAL(audioInputChannelChanged(synthclone::SampleChannelCount,
                                            int)),
            SIGNAL(audioInputChannelChanged(synthclone::SampleChannelCount,
                                            int)));
    connect(&channelMapTableDelegate,
            SIGNAL(audioOutputChannelChanged(synthclone::SampleChannelCount,
                                             int)),
            SIGNAL(audioOutputChannelChanged(synthclone::SampleChannelCount,
                                             int)));

    channelMapTableModel.setColumnCount(CHANNELMAPTABLECOLUMN_TOTAL);
    channelMapTableModel.setHeaderData(CHANNELMAPTABLECOLUMN_INPUT_CHANNEL,
                                       Qt::Horizontal, tr("Input Port"));
    channelMapTableModel.setHeaderData(CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL,
                                       Qt::Horizontal, tr("Output Port"));

    channelMapTableView =
        synthclone::getChild<QTableView>(widget, "channelMap");
    channelMapTableView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    channelMapTableView->setHorizontalScrollMode
        (QAbstractItemView::ScrollPerPixel);
    channelMapTableView->setVerticalScrollMode
        (QAbstractItemView::ScrollPerPixel);
    channelMapTableView->setItemDelegate(&channelMapTableDelegate);
    channelMapTableView->setModel(&channelMapTableModel);

    closeButton = synthclone::getChild<QPushButton>(widget, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    instances = synthclone::getChild<QSpinBox>(widget, "instances");
    connect(instances, SIGNAL(valueChanged(int)),
            SIGNAL(instanceCountChanged(int)));

    name = synthclone::getChild<QLineEdit>(widget, "name");
    connect(name, SIGNAL(textEdited(const QString &)),
            SIGNAL(nameChanged(const QString &)));

    parametersTab = synthclone::getChild<QWidget>(widget, "parametersTab");
}

EffectView::~EffectView()
{
    resetInstanceData();
    suil_host_free(host);
}

void
EffectView::addAudioInputPort(const QString &name)
{
    channelMapTableDelegate.addAudioInputPort(name);
    audioInputPortNames.append(name);
}

void
EffectView::addAudioOutputPort(const QString &name)
{
    channelMapTableDelegate.addAudioOutputPort(name);
    audioOutputPortNames.append(name);
}

void
EffectView::clearAudioInputPorts()
{
    channelMapTableDelegate.clearAudioInputPorts();
    audioInputPortNames.clear();
}

void
EffectView::clearAudioOutputPorts()
{
    channelMapTableDelegate.clearAudioOutputPorts();
    audioOutputPortNames.clear();
}

unsigned int
EffectView::getSupportQuality(const QString &widgetTypeURI)
{
    QByteArray bytes = widgetTypeURI.toAscii();
    return suil_ui_supported(LV2_UI__Qt4UI, bytes.constData());
}

void
EffectView::handlePortWriteRequest(uint32_t portIndex, uint32_t bufferSize,
                                   uint32_t protocol, void const *buffer)
{
    emit portValueChanged(portIndex, bufferSize, protocol, buffer);
}

void
EffectView::removeAudioInputPort()
{
    channelMapTableDelegate.removeAudioInputPort();
    audioInputPortNames.removeLast();
}

void
EffectView::removeAudioOutputPort()
{
    channelMapTableDelegate.removeAudioOutputPort();
    audioOutputPortNames.removeLast();
}

void
EffectView::resetInstanceData()
{
    if (instance) {
        parametersTab->layout()->removeWidget(instanceUI);
        // XX: Is this right?
        delete instanceUI;
        suil_instance_free(instance);
    }
    instance = 0;
}

void
EffectView::setAudioInputChannel(synthclone::SampleChannelCount inputChannel,
                                 int effectChannel)
{
    int channel = static_cast<int>(inputChannel);
    QString name = audioInputPortNames[effectChannel];
    setModelData(channel, CHANNELMAPTABLECOLUMN_INPUT_CHANNEL, effectChannel);
    setModelData(channel, CHANNELMAPTABLECOLUMN_INPUT_CHANNEL, name,
                 Qt::DisplayRole);
}

void
EffectView::setAudioOutputChannel(synthclone::SampleChannelCount outputChannel,
                                  int effectChannel)
{
    int channel = static_cast<int>(outputChannel);
    QString name = audioOutputPortNames[effectChannel];
    setModelData(channel, CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL, effectChannel);
    setModelData(channel, CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL, name,
                 Qt::DisplayRole);
}

void
EffectView::setInstanceCount(int instances)
{
    this->instances->setValue(instances);
}

void
EffectView::setModelData(int row, int column, const QVariant &value, int role)
{
    bool result = channelMapTableModel.setData
        (channelMapTableModel.index(row, column), value, role);
    assert(result);
}

void
EffectView::setName(const QString &name)
{
    this->name->setText(name);
}

void
EffectView::setPortValue(uint32_t index, uint32_t size, uint32_t protocol,
                         void const *value)
{
    qDebug() << "setPortValue:";
    qDebug() << "\tindex: " << index;
    qDebug() << "\tsize: " << size;
    qDebug() << "\tprotocol: " << protocol;
    qDebug() << "\tvalue: " << *(static_cast<float const *>(value));

    suil_instance_port_event(instance, index, size, protocol, value);
}

void
EffectView::setSampleChannelCount(synthclone::SampleChannelCount channels)
{
    channelMapTableModel.setRowCount(static_cast<int>(channels));
}

void
EffectView::setURIData(const QString &widgetURI, const QString &widgetTypeURI,
                       const QString &pluginURI, const QString &binaryPath,
                       const QString &bundlePath)
{
    resetInstanceData();
    QByteArray binaryPathBytes = binaryPath.toAscii();
    QByteArray bundlePathBytes = bundlePath.toAscii();
    QByteArray pluginURIBytes = pluginURI.toAscii();
    QByteArray widgetTypeURIBytes = widgetTypeURI.toAscii();
    QByteArray widgetURIBytes = widgetURI.toAscii();
    instance = suil_instance_new(host, this, LV2_UI__Qt4UI,
                                 pluginURIBytes.constData(),
                                 widgetURIBytes.constData(),
                                 widgetTypeURIBytes.constData(),
                                 bundlePathBytes.constData(),
                                 binaryPathBytes.constData(), 0);
    if (! instance) {
        QString message = tr("Failed to create LV2 UI for plugin '%1'").
            arg(pluginURI);
        throw synthclone::Error(message);
    }
    instanceUI = static_cast<QWidget *>(suil_instance_get_widget(instance));
    parametersTab->layout()->addWidget(instanceUI);
    instanceUI->show();
}
