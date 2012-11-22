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

#include <cassert>
#include <cfloat>
#include <cmath>

#include <QtCore/QDebug>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHeaderView>
#include <QtGui/QScrollBar>
#include <QtGui/QSlider>
#include <QtGui/QX11EmbedContainer>

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

    instanceUI = 0;

    name = synthclone::getChild<QLineEdit>(widget, "name");
    connect(name, SIGNAL(textEdited(const QString &)),
            SIGNAL(nameChanged(const QString &)));

    parametersTab = synthclone::getChild<QWidget>(widget, "parametersTab");

    parameterFormLayout =
        synthclone::getChild<QGridLayout>(widget, "parameterFormLayout");

    parameterFormWidget =
        synthclone::getChild<QWidget>(widget, "parameterFormWidget");

    parameterScrollArea =
        synthclone::getChild<QScrollArea>(widget, "parameterScrollArea");
    parameterScrollArea->setWidgetResizable(true);

    resizeEventFilter = new ResizeEventFilter(parameterScrollArea,
                                              parameterFormWidget);
    parameterFormWidget->installEventFilter(resizeEventFilter);

    tabWidget = synthclone::getChild<QTabWidget>(widget, "tabWidget");
}

EffectView::~EffectView()
{
    resetInstanceData();
    parameterFormWidget->removeEventFilter(resizeEventFilter);
    delete resizeEventFilter;
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
EffectView::addControlInputPort(const QString &name, ControlInputPortType type,
                                uint32_t index, float value, float minimumValue,
                                float maximumValue,
                                const QMap<float, QString> &scalePoints)
{
    QCheckBox *checkBox;
    QComboBox *comboBox;
    WidgetData *data = new WidgetData();
    QDoubleSpinBox *doubleSpinBox;
    int decimalPlaces;
    QLabel *label = new QLabel(name);
    double roughStep;
    int rowCount = parameterFormLayout->rowCount();
    QSlider *slider;
    float sliderValue;
    double smoothStep;
    QSpinBox *spinBox;

    data->index = index;
    data->label = label;
    data->maximumValue = maximumValue;
    data->minimumValue = minimumValue;
    data->type = type;

    QMap<float, QString>::const_iterator end;

    parameterFormLayout->addWidget(label, rowCount, 0);

    switch (type) {

    case CONTROLINPUTPORTTYPE_BOOLEAN:
        checkBox = new QCheckBox();
        checkBox->setChecked(value > 0.0);
        connect(checkBox, SIGNAL(clicked(bool)),
                SLOT(handleCheckBoxClick(bool)));

        parameterFormLayout->addWidget(checkBox, rowCount, 1, 2, 1);

        data->widget1 = checkBox;
        data->widget2 = 0;
        widgetDataMap.insert(checkBox, data);

        break;
    case CONTROLINPUTPORTTYPE_ENUMERATION:
        comboBox = new QComboBox();
        end = scalePoints.constEnd();
        for (QMap<float, QString>::const_iterator i = scalePoints.constBegin();
             i != end; i++) {
            float pointValue = i.key();
            comboBox->addItem(i.value(), pointValue);
            if (value == pointValue) {
                comboBox->setCurrentIndex(comboBox->count() - 1);
            }
        }
        connect(comboBox, SIGNAL(currentIndexChanged(int)),
                SLOT(handleComboBoxCurrentIndexChange(int)));

        parameterFormLayout->addWidget(comboBox, rowCount, 1, 2, 1);

        data->widget1 = comboBox;
        data->widget2 = 0;
        widgetDataMap.insert(comboBox, data);

        break;
    case CONTROLINPUTPORTTYPE_FLOAT:
        slider = new QSlider(Qt::Horizontal);
        slider->setMinimumWidth(200);
        slider->setRange(0, 100);
        sliderValue = ((value - minimumValue) / (maximumValue - minimumValue)) *
            100.0;
        slider->setValue(floor(sliderValue + 0.5));
        connect(slider, SIGNAL(sliderMoved(int)),
                SLOT(handleFloatSliderMove(int)));

        roughStep = static_cast<double>(maximumValue - minimumValue) / 1000.0;
        decimalPlaces = -static_cast<int>(log10(roughStep));
        smoothStep = pow(10, -decimalPlaces);
        doubleSpinBox = new QDoubleSpinBox();
        doubleSpinBox->setDecimals((decimalPlaces < 2) ? 2 : decimalPlaces);
        doubleSpinBox->setRange(minimumValue, maximumValue);
        doubleSpinBox->setSingleStep((smoothStep > 1.0) ? 1.0 : smoothStep);
        doubleSpinBox->setValue(value);
        connect(doubleSpinBox, SIGNAL(valueChanged(double)),
                SLOT(handleDoubleSpinBoxValueChange(double)));

        parameterFormLayout->addWidget(slider, rowCount, 1);
        parameterFormLayout->addWidget(doubleSpinBox, rowCount, 2);

        data->widget1 = doubleSpinBox;
        data->widget2 = slider;
        widgetDataMap.insert(doubleSpinBox, data);
        widgetDataMap.insert(slider, data);

        break;
    case CONTROLINPUTPORTTYPE_INTEGER:
        spinBox = new QSpinBox();
        spinBox->setRange(static_cast<int>(minimumValue),
                          static_cast<int>(maximumValue));
        spinBox->setValue(static_cast<int>(value));
        connect(spinBox, SIGNAL(valueChanged(int)),
                SLOT(handleSpinBoxValueChange(int)));

        slider = new QSlider(Qt::Horizontal);
        slider->setMinimumWidth(200);
        slider->setRange(static_cast<int>(minimumValue),
                         static_cast<int>(maximumValue));
        slider->setValue(static_cast<int>(value));
        connect(slider, SIGNAL(sliderMoved(int)),
                SLOT(handleIntSliderMove(int)));

        parameterFormLayout->addWidget(slider, rowCount, 1);
        parameterFormLayout->addWidget(spinBox, rowCount, 2);

        data->widget1 = spinBox;
        data->widget2 = slider;
        widgetDataMap.insert(spinBox, data);
        widgetDataMap.insert(slider, data);

        break;
    default:
        assert(false);
    }
    indexDataMap.insert(index, data);
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

void
EffectView::clearControlInputPorts()
{
    QMap<uint32_t, WidgetData *>::iterator end = indexDataMap.end();
    for (QMap<uint32_t, WidgetData *>::iterator iter = indexDataMap.begin();
         iter != end; iter++) {
        WidgetData *data = iter.value();
        assert(data);

        QLabel *label = data->label;
        parameterFormLayout->removeWidget(label);
        delete label;

        QWidget *widget = data->widget2;
        if (widget) {
            parameterFormLayout->removeWidget(widget);
            delete widget;
        }

        widget = data->widget1;
        parameterFormLayout->removeWidget(widget);
        delete widget;
    }
    indexDataMap.clear();
    widgetDataMap.clear();
}

unsigned int
EffectView::getSupportQuality(const QString &widgetTypeURI)
{
    QByteArray bytes = widgetTypeURI.toAscii();
    return suil_ui_supported(LV2_UI__Qt4UI, bytes.constData());
}

void
EffectView::handleCheckBoxClick(bool checked)
{
    QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
    assert(checkBox);
    WidgetData *data = widgetDataMap.value(checkBox, 0);
    assert(data);
    float value = checked ? data->maximumValue : data->minimumValue;
    emit portValueChanged(data->index, 4, 0, &value);
}

void
EffectView::handleComboBoxCurrentIndexChange(int index)
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
    assert(comboBox);
    WidgetData *data = widgetDataMap.value(comboBox, 0);
    assert(data);
    float value = comboBox->itemData(index).toFloat();
    emit portValueChanged(data->index, 4, 0, &value);
}

void
EffectView::handleDoubleSpinBoxValueChange(double n)
{
    QDoubleSpinBox *spinBox = qobject_cast<QDoubleSpinBox *>(sender());
    assert(spinBox);
    WidgetData *data = widgetDataMap.value(spinBox, 0);
    assert(data);
    float value = static_cast<float>(n);
    emit portValueChanged(data->index, 4, 0, &value);
}

void
EffectView::handleFloatSliderMove(int position)
{
    QSlider *slider = qobject_cast<QSlider *>(sender());
    assert(slider);
    WidgetData *data = widgetDataMap.value(slider, 0);
    assert(data);
    float value = data->minimumValue +
        ((data->maximumValue - data->minimumValue) *
         (static_cast<float>(position) / 100.0));
    emit portValueChanged(data->index, 4, 0, &value);
}

void
EffectView::handleIntSliderMove(int position)
{
    QSlider *slider = qobject_cast<QSlider *>(sender());
    assert(slider);
    WidgetData *data = widgetDataMap.value(slider, 0);
    assert(data);
    float value = static_cast<float>(position);
    emit portValueChanged(data->index, 4, 0, &value);
}

void
EffectView::handlePortWriteRequest(uint32_t portIndex, uint32_t bufferSize,
                                   uint32_t protocol, void const *buffer)
{
    emit portValueChanged(portIndex, bufferSize, protocol, buffer);
}

void
EffectView::handleSpinBoxValueChange(int n)
{
    QSpinBox *spinBox = qobject_cast<QSpinBox *>(sender());
    assert(spinBox);
    WidgetData *data = widgetDataMap.value(spinBox, 0);
    assert(data);
    float value = static_cast<float>(n);
    emit portValueChanged(data->index, 4, 0, &value);
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
        instanceUI = 0;
        suil_instance_free(instance);
        instance = 0;
    }
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
    channelMapTableView->resizeColumnToContents
        (CHANNELMAPTABLECOLUMN_INPUT_CHANNEL);
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
    channelMapTableView->resizeColumnToContents
        (CHANNELMAPTABLECOLUMN_OUTPUT_CHANNEL);
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
    if (instance) {
        suil_instance_port_event(instance, index, size, protocol, value);
    }
    WidgetData *data = indexDataMap.value(index, 0);
    if (data) {
        QCheckBox *checkBox;
        QComboBox *comboBox;
        int comboBoxIndex;
        QDoubleSpinBox *doubleSpinBox;
        float n = *(static_cast<const float *>(value));
        QSlider *slider;
        QSpinBox *spinBox;
        switch (data->type) {

        case CONTROLINPUTPORTTYPE_BOOLEAN:
            checkBox = qobject_cast<QCheckBox *>(data->widget1);
            assert(checkBox);
            checkBox->setChecked(n > 0.0);
            break;

        case CONTROLINPUTPORTTYPE_ENUMERATION:
            comboBox = qobject_cast<QComboBox *>(data->widget1);
            assert(comboBox);
            comboBoxIndex = comboBox->findData(n);
            if (comboBoxIndex != -1) {
                comboBox->setCurrentIndex(comboBoxIndex);
            } else {
                qWarning() << tr("EffectView::setPortValue - value %1 not "
                                 "found in enumeration").arg(n);
            }
            break;

        case CONTROLINPUTPORTTYPE_FLOAT:
            doubleSpinBox = qobject_cast<QDoubleSpinBox *>(data->widget1);
            assert(doubleSpinBox);
            slider = qobject_cast<QSlider *>(data->widget2);
            assert(slider);
            doubleSpinBox->setValue(n);
            n = ((n - data->minimumValue) /
                 (data->maximumValue - data->minimumValue)) * 100.0;
            slider->setValue(floor(n + 0.5));
            break;

        case CONTROLINPUTPORTTYPE_INTEGER:
            spinBox = qobject_cast<QSpinBox *>(data->widget1);
            assert(spinBox);
            slider = qobject_cast<QSlider *>(data->widget2);
            assert(slider);
            spinBox->setValue(n);
            slider->setValue(n);
            break;

        default:
            assert(false);
        }
    }
}

void
EffectView::setSampleChannelCount(synthclone::SampleChannelCount channels)
{
    channelMapTableModel.setRowCount(static_cast<int>(channels));
}

void
EffectView::setViewData(const EffectViewData &data)
{
    resetInstanceData();

    // If a UI is provided by the view data, then use the UI.  If not, then the
    // UI will be created step by step with addParameter calls.
    bool provided = data.isProvided();
    if (provided) {

        // Create SUIL instance.
        QByteArray binaryPathBytes = data.getBinaryPath().toAscii();
        QByteArray bundlePathBytes = data.getBundlePath().toAscii();
        QByteArray pluginURIBytes = data.getPluginURI().toAscii();
        QByteArray widgetTypeURIBytes = data.getTypeURI().toAscii();
        QByteArray widgetURIBytes = data.getURI().toAscii();
        LV2_Feature parentFeature;
        parentFeature.data = parametersTab;
        parentFeature.URI = LV2_UI__parent;
        LV2_Feature *featurePtrs[2];
        featurePtrs[0] = &parentFeature;
        featurePtrs[1] = 0;
        instance = suil_instance_new(host, this, LV2_UI__Qt4UI,
                                     pluginURIBytes.constData(),
                                     widgetURIBytes.constData(),
                                     widgetTypeURIBytes.constData(),
                                     bundlePathBytes.constData(),
                                     binaryPathBytes.constData(), featurePtrs);
        if (! instance) {
            QString message = tr("Failed to create LV2 UI for plugin '%1'").
                arg(data.getPluginURI());
            throw synthclone::Error(message);
        }

        // Get SUIL widget for instance.
        instanceUI = static_cast<QWidget *>
            (suil_instance_get_widget(instance));
        assert(instanceUI);

        // Add SUIL widget to UI.
        instanceUI->setWindowFlags((instanceUI->windowFlags() &
                                    (~Qt::WindowType_Mask)) | Qt::Widget);
        parametersTab->layout()->addWidget(instanceUI);

        // SUIL versions before 0.6.6 will create QX11EmbedContainer objects
        // that are unparented.  If the containers are reparented, they don't
        // work.  Later versions will reparent the widget.  So, if we find a
        // QX11EmbedContainer without a parent, we just use the generic UI.
        QX11EmbedContainer *container =
            qobject_cast<QX11EmbedContainer *>(instanceUI);
        if (container) {
            if (! container->parent()) {
                resetInstanceData();
                provided = false;
            }
        }
    }
    parameterScrollArea->setVisible(! provided);
}

void
EffectView::setVisible(bool visible)
{
    DesignerView::setVisible(visible);
    if (visible) {

        // Shameful hack: Make sure the view is initially large enough to
        // for the effect editor instead of waiting for the editor tab to be
        // clicked by the user before showing the expanded view.  I haven't
        // found a better way to do this.  If you know of a more elegant way to
        // do this, then please let me know.
        int currentIndex = tabWidget->currentIndex();
        if (currentIndex != 1) {
            tabWidget->setCurrentIndex(1);
            tabWidget->setCurrentIndex(currentIndex);
        }
    }
}
