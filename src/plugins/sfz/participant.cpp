/*
 * libsynthclone_sfz - SFZ target plugin for `synthclone`
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

#include <QtCore/QDebug>
#include <QtCore/QLocale>

#include "participant.h"

Participant::Participant(QObject *parent):
    synthclone::Participant(tr("SFZ"), 0, 0, 1, "Devin Anderson",
                            tr("Creates SFZ instruments."), parent),
    addTargetAction(tr("SFZ"))
{
    directoryView.setFilesVisible(false);
    directoryView.setOperation(synthclone::FileSelectionView::OPERATION_SAVE);
    directoryView.setTitle(tr("Save SFZ Instrument"));
    connect(&addTargetAction, SIGNAL(triggered()),
            SLOT(handleTargetAddition()));
    connect(&directoryView, SIGNAL(closeRequest()),
            SLOT(handleDirectoryViewCloseRequest()));
    connect(&directoryView, SIGNAL(pathsSelected(const QStringList &)),
            SLOT(handleDirectoryViewPathSelection(const QStringList &)));
    connect(&targetView, SIGNAL(closeRequest()),
            SLOT(handleTargetViewCloseRequest()));
    connect(&targetView, SIGNAL(pathLookupRequest()),
            SLOT(handleTargetViewPathLookupRequest()));
    context = 0;
}

Participant::~Participant()
{
    // Empty
}

void
Participant::activate(synthclone::Context &context, const QVariant &/*state*/)
{
    context.addMenuAction(&addTargetAction, synthclone::MENU_ADD_TARGET);
    this->context = &context;
    configuredTarget = 0;
}

void
Participant::addControlLayer(int index, const ControlLayer *layer)
{
    targetView.addControlLayer(index, layer->getControl());
    targetView.setControlLayerCrossfadingEnabled
        (index, layer->isCrossfadingEnabled());
    targetView.setControlLayerDefaultValue(index, layer->getDefaultValue());
    targetView.setControlLayerType(index, layer->getType());
    connect(layer, SIGNAL(crossfadingEnabledChanged(bool)),
            SLOT(handleControlLayerCrossfadingEnabledChange(bool)));
    connect(layer, SIGNAL(defaultValueChanged(synthclone::MIDIData)),
            SLOT(handleControlLayerDefaultValueChange(synthclone::MIDIData)));
    connect(layer, SIGNAL(typeChanged(synthclone::ControlType)),
            SLOT(handleControlLayerTypeChange(synthclone::ControlType)));
}

Target *
Participant::addTarget()
{
    Target *target = new Target(tr("SFZ"), this);
    connect(target, SIGNAL(controlCrossfadeCurveChanged(CrossfadeCurve)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(controlLayerAdded(const ControlLayer *, int)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(controlLayerMoved(const ControlLayer *, int, int)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(controlLayerRemoved(const ControlLayer *, int)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(drumKitChanged(bool)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(nameChanged(QString)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(noteCrossfadeCurveChanged(CrossfadeCurve)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(pathChanged(QString)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(sampleFormatChanged(SampleFormat)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(velocityCrossfadeCurveChanged(CrossfadeCurve)),
            context, SLOT(setSessionModified()));

    synthclone::MenuAction *action =
        new synthclone::MenuAction(tr("Configure"), target);
    connect(action, SIGNAL(triggered()), SLOT(handleTargetConfiguration()));

    const synthclone::Registration &targetRegistration =
        context->addTarget(target);
    connect(&targetRegistration, SIGNAL(unregistered(QObject *)),
            target, SLOT(deleteLater()));

    const synthclone::Registration &actionRegistration =
        context->addMenuAction(action, target);
    connect(&actionRegistration, SIGNAL(unregistered(QObject *)),
            action, SLOT(deleteLater()));

    return target;
}

void
Participant::configureTarget(Target *target)
{
    assert(! configuredTarget);
    targetView.setControlCrossfadeCurve(target->getControlCrossfadeCurve());
    targetView.setDrumKit(target->isDrumKit());
    targetView.setName(target->getName());
    targetView.setNoteCrossfadeCurve(target->getNoteCrossfadeCurve());
    targetView.setPath(target->getPath());
    targetView.setSampleFormat(target->getSampleFormat());
    targetView.setVelocityCrossfadeCurve(target->getVelocityCrossfadeCurve());
    int controlLayerCount = target->getControlLayerCount();
    for (int i = 0; i < controlLayerCount; i++) {
        addControlLayer(i, target->getControlLayer(i));
    }

    connect(target, SIGNAL(controlCrossfadeCurveChanged(CrossfadeCurve)),
            &targetView, SLOT(setControlCrossfadeCurve(CrossfadeCurve)));
    connect(target, SIGNAL(drumKitChanged(bool)),
            &targetView, SLOT(setDrumKit(bool)));
    connect(target, SIGNAL(noteCrossfadeCurveChanged(CrossfadeCurve)),
            &targetView, SLOT(setNoteCrossfadeCurve(CrossfadeCurve)));
    connect(target, SIGNAL(pathChanged(const QString &)),
            &targetView, SLOT(setPath(const QString &)));
    connect(target, SIGNAL(sampleFormatChanged(SampleFormat)),
            &targetView, SLOT(setSampleFormat(SampleFormat)));
    connect(target, SIGNAL(velocityCrossfadeCurveChanged(CrossfadeCurve)),
            &targetView, SLOT(setVelocityCrossfadeCurve(CrossfadeCurve)));

    connect(target, SIGNAL(controlLayerAdded(const ControlLayer *, int)),
            SLOT(handleTargetControlLayerAddition(const ControlLayer *, int)));
    connect(target, SIGNAL(controlLayerMoved(const ControlLayer *, int, int)),
            SLOT(handleTargetControlLayerMove(const ControlLayer *, int,
                                              int)));
    connect(target, SIGNAL(controlLayerRemoved(const ControlLayer *, int)),
            SLOT(handleTargetControlLayerRemoval(const ControlLayer *, int)));

    connect(&targetView,
            SIGNAL(controlCrossfadeCurveChangeRequest(CrossfadeCurve)),
            target, SLOT(setControlCrossfadeCurve(CrossfadeCurve)));
    connect(&targetView, SIGNAL(controlLayerAddRequest(synthclone::MIDIData)),
            target, SLOT(addControlLayer(synthclone::MIDIData)));
    connect(&targetView, SIGNAL(controlLayerMoveRequest(int, int)),
            target, SLOT(moveControlLayer(int, int)));
    connect(&targetView, SIGNAL(controlLayerRemoveRequest(int)),
            target, SLOT(removeControlLayer(int)));
    connect(&targetView, SIGNAL(drumKitChangeRequest(bool)),
            target, SLOT(setDrumKit(bool)));
    connect(&targetView, SIGNAL(nameChangeRequest(const QString &)),
            target, SLOT(setName(const QString &)));
    connect(&targetView,
            SIGNAL(noteCrossfadeCurveChangeRequest(CrossfadeCurve)),
            target, SLOT(setNoteCrossfadeCurve(CrossfadeCurve)));
    connect(&targetView, SIGNAL(pathChangeRequest(const QString &)),
            target, SLOT(setPath(const QString &)));
    connect(&targetView, SIGNAL(sampleFormatChangeRequest(SampleFormat)),
            target, SLOT(setSampleFormat(SampleFormat)));
    connect(&targetView,
            SIGNAL(velocityCrossfadeCurveChangeRequest(CrossfadeCurve)),
            target, SLOT(setVelocityCrossfadeCurve(CrossfadeCurve)));

    connect(&targetView,
            SIGNAL(controlLayerCrossfadingEnabledChangeRequest(int, bool)),
            SLOT(handleTargetViewCrossfadingEnabledChangeRequest(int, bool)));
    connect(&targetView,
            SIGNAL(controlLayerDefaultValueChangeRequest
                   (int, synthclone::MIDIData)),
            SLOT(handleTargetViewDefaultValueChangeRequest
                 (int, synthclone::MIDIData)));
    connect(&targetView,
            SIGNAL(controlLayerTypeChangeRequest
                   (int, synthclone::ControlType)),
            SLOT(handleTargetViewTypeChangeRequest
                 (int, synthclone::ControlType)));

    targetView.setVisible(true);
    configuredTarget = target;
}

void
Participant::deactivate(synthclone::Context &context)
{
    context.removeMenuAction(&addTargetAction);
    this->context = 0;
}

CrossfadeCurve
Participant::getCrossfadeCurveConstant(const QString &str) const
{
    return str == "GAIN" ? CROSSFADECURVE_GAIN :
        str == "POWER" ? CROSSFADECURVE_POWER : CROSSFADECURVE_NONE;
}

QString
Participant::getCrossfadeCurveString(CrossfadeCurve curve) const
{
    QString str;
    switch (curve) {
    case CROSSFADECURVE_GAIN:
        str = "GAIN";
        break;
    case CROSSFADECURVE_NONE:
        str = "NONE";
        break;
    case CROSSFADECURVE_POWER:
        str = "POWER";
        break;
    default:
        assert(false);
    }
    return str;
}

QVariant
Participant::getState(const synthclone::Target *target) const
{
    const Target *t = qobject_cast<const Target *>(target);
    assert(t);

    QVariantMap map;
    map["controlCrossfadeCurve"] =
        getCrossfadeCurveString(t->getControlCrossfadeCurve());
    map["drumKit"] = t->isDrumKit();
    map["name"] = t->getName();
    map["noteCrossfadeCurve"] =
        getCrossfadeCurveString(t->getNoteCrossfadeCurve());
    map["path"] = t->getPath();
    map["velocityCrossfadeCurve"] =
        getCrossfadeCurveString(t->getVelocityCrossfadeCurve());

    QString formatStr;
    switch (t->getSampleFormat()) {
    case SAMPLEFORMAT_OGG_VORBIS:
        formatStr = "OGG_VORBIS";
        break;
    case SAMPLEFORMAT_WAV_8BIT:
        formatStr = "WAV_8BIT";
        break;
    case SAMPLEFORMAT_WAV_16BIT:
        formatStr = "WAV_16BIT";
        break;
    case SAMPLEFORMAT_WAV_24BIT:
        formatStr = "WAV_24BIT";
        break;
    case SAMPLEFORMAT_WAV_32BIT:
        formatStr = "WAV_32BIT";
        break;
    case SAMPLEFORMAT_WAV_32BIT_FLOAT:
        formatStr = "WAV_32BIT_FLOAT";
        break;
    default:
        assert(false);
    }
    map["sampleFormat"] = formatStr;

    // Control layers
    QVariantList layers;
    int layerCount = t->getControlLayerCount();
    for (int i = 0; i < layerCount; i++) {
        const ControlLayer *layer = t->getControlLayer(i);
        QVariantMap layerMap;
        layerMap["control"] = static_cast<int>(layer->getControl());
        layerMap["crossfadingEnabled"] = layer->isCrossfadingEnabled();
        layerMap["defaultValue"] = static_cast<int>(layer->getDefaultValue());
        QString typeStr;
        switch (layer->getType()) {
        case synthclone::CONTROLTYPE_CONTINUOUS:
            typeStr = "CONTINUOUS";
            break;
        case synthclone::CONTROLTYPE_SWITCH:
            typeStr = "SWITCH";
            break;
        default:
            assert(false);
        }
        layerMap["type"] = typeStr;
        layers.append(layerMap);
    }
    map["controlLayers"] = layers;

    return map;
}

void
Participant::handleControlLayerCrossfadingEnabledChange(bool enabled)
{
    int index = configuredTarget->
        getControlLayerIndex(qobject_cast<ControlLayer *>(sender()));
    targetView.setControlLayerCrossfadingEnabled(index, enabled);
}

void
Participant::handleControlLayerDefaultValueChange(synthclone::MIDIData value)
{
    int index = configuredTarget->
        getControlLayerIndex(qobject_cast<ControlLayer *>(sender()));
    targetView.setControlLayerDefaultValue(index, value);
}

void
Participant::handleControlLayerTypeChange(synthclone::ControlType type)
{
    int index = configuredTarget->
        getControlLayerIndex(qobject_cast<ControlLayer *>(sender()));
    targetView.setControlLayerType(index, type);
}

void
Participant::handleDirectoryViewCloseRequest()
{
    directoryView.setVisible(false);
}

void
Participant::handleDirectoryViewPathSelection(const QStringList &paths)
{
    assert(paths.count() == 1);
    configuredTarget->setPath(paths[0]);
    directoryView.setVisible(false);
}

void
Participant::handleTargetAddition()
{
    configureTarget(addTarget());
}

void
Participant::handleTargetConfiguration()
{
    synthclone::MenuAction *action =
        qobject_cast<synthclone::MenuAction *>(sender());
    configureTarget(qobject_cast<Target *>(action->parent()));
}

void
Participant::handleTargetControlLayerAddition(const ControlLayer *layer,
                                              int index)
{
    addControlLayer(index, layer);
}

void
Participant::handleTargetControlLayerMove(const ControlLayer */*layer*/,
                                          int fromIndex, int toIndex)
{
    targetView.moveControlLayer(fromIndex, toIndex);
}

void
Participant::handleTargetControlLayerRemoval(const ControlLayer *layer,
                                             int index)
{
    removeControlLayer(index, layer);
}

void
Participant::handleTargetViewCloseRequest()
{
    disconnect(configuredTarget,
               SIGNAL(controlCrossfadeCurveChanged(CrossfadeCurve)),
               &targetView, SLOT(setControlCrossfadeCurve(CrossfadeCurve)));
    disconnect(configuredTarget, SIGNAL(drumKitChanged(bool)),
               &targetView, SLOT(setDrumKit(bool)));
    disconnect(configuredTarget,
               SIGNAL(noteCrossfadeCurveChanged(CrossfadeCurve)),
               &targetView, SLOT(setNoteCrossfadeCurve(CrossfadeCurve)));
    disconnect(configuredTarget, SIGNAL(pathChanged(const QString &)),
               &targetView, SLOT(setPath(const QString &)));
    disconnect(configuredTarget, SIGNAL(sampleFormatChanged(SampleFormat)),
               &targetView, SLOT(setSampleFormat(SampleFormat)));
    disconnect(configuredTarget,
               SIGNAL(velocityCrossfadeCurveChanged(CrossfadeCurve)),
               &targetView, SLOT(setVelocityCrossfadeCurve(CrossfadeCurve)));

    disconnect(configuredTarget,
               SIGNAL(controlLayerAdded(const ControlLayer *, int)),
               this,
               SLOT(handleTargetControlLayerAddition(const ControlLayer *,
                                                     int)));
    disconnect(configuredTarget,
               SIGNAL(controlLayerMoved(const ControlLayer *, int, int)),
               this,
               SLOT(handleTargetControlLayerMove(const ControlLayer *, int,
                                                 int)));
    disconnect(configuredTarget,
               SIGNAL(controlLayerRemoved(const ControlLayer *, int)),
               this,
               SLOT(handleTargetControlLayerRemoval(const ControlLayer *,
                                                    int)));

    disconnect(&targetView,
               SIGNAL(controlCrossfadeCurveChangeRequest(CrossfadeCurve)),
               configuredTarget,
               SLOT(setControlCrossfadeCurve(CrossfadeCurve)));
    disconnect(&targetView,
               SIGNAL(controlLayerAddRequest(synthclone::MIDIData)),
               configuredTarget, SLOT(addControlLayer(synthclone::MIDIData)));
    disconnect(&targetView, SIGNAL(controlLayerMoveRequest(int, int)),
               configuredTarget, SLOT(moveControlLayer(int, int)));
    disconnect(&targetView, SIGNAL(controlLayerRemoveRequest(int)),
               configuredTarget, SLOT(removeControlLayer(int)));
    disconnect(&targetView, SIGNAL(drumKitChangeRequest(bool)),
               configuredTarget, SLOT(setDrumKit(bool)));
    disconnect(&targetView, SIGNAL(nameChangeRequest(const QString &)),
               configuredTarget, SLOT(setName(const QString &)));
    disconnect(&targetView,
               SIGNAL(noteCrossfadeCurveChangeRequest(CrossfadeCurve)),
               configuredTarget, SLOT(setNoteCrossfadeCurve(CrossfadeCurve)));
    disconnect(&targetView, SIGNAL(pathChangeRequest(const QString &)),
               configuredTarget, SLOT(setPath(const QString &)));
    disconnect(&targetView, SIGNAL(sampleFormatChangeRequest(SampleFormat)),
               configuredTarget, SLOT(setSampleFormat(SampleFormat)));
    disconnect(&targetView,
               SIGNAL(velocityCrossfadeCurveChangeRequest(CrossfadeCurve)),
               configuredTarget,
               SLOT(setVelocityCrossfadeCurve(CrossfadeCurve)));

    disconnect(&targetView,
               SIGNAL(controlLayerCrossfadingEnabledChangeRequest(int, bool)),
               this,
               SLOT(handleTargetViewCrossfadingEnabledChangeRequest(int,
                                                                    bool)));
    disconnect(&targetView,
               SIGNAL(controlLayerDefaultValueChangeRequest
                      (int, synthclone::MIDIData)),
               this,
               SLOT(handleTargetViewDefaultValueChangeRequest
                    (int, synthclone::MIDIData)));
    disconnect(&targetView,
               SIGNAL(controlLayerTypeChangeRequest
                      (int, synthclone::ControlType)),
               this,
               SLOT(handleTargetViewTypeChangeRequest
                    (int, synthclone::ControlType)));

    for (int i = configuredTarget->getControlLayerCount() - 1; i >= 0; i--) {
        removeControlLayer(i, configuredTarget->getControlLayer(i));
    }

    targetView.setVisible(false);
    configuredTarget = 0;
}

void
Participant::handleTargetViewCrossfadingEnabledChangeRequest(int index,
                                                             bool enabled)
{
    configuredTarget->getControlLayer(index)->setCrossfadingEnabled(enabled);
}

void
Participant::
handleTargetViewDefaultValueChangeRequest(int index,
                                          synthclone::MIDIData value)
{
    configuredTarget->getControlLayer(index)->setDefaultValue(value);
}

void
Participant::handleTargetViewPathLookupRequest()
{
    assert(configuredTarget);
    directoryView.setDirectory(configuredTarget->getPath());
    directoryView.setVisible(true);
}

void
Participant::handleTargetViewTypeChangeRequest(int index,
                                               synthclone::ControlType type)
{
    configuredTarget->getControlLayer(index)->setType(type);
}

void
Participant::removeControlLayer(int index, const ControlLayer *layer)
{
    targetView.removeControlLayer(index, layer->getControl());
    disconnect(layer, SIGNAL(crossfadingEnabledChanged(bool)),
               this, SLOT(handleControlLayerCrossfadingEnabledChange(bool)));
    disconnect(layer, SIGNAL(defaultValueChanged(synthclone::MIDIData)),
               this,
               SLOT(handleControlLayerDefaultValueChange
                    (synthclone::MIDIData)));
    disconnect(layer, SIGNAL(typeChanged(synthclone::ControlType)),
               this,
               SLOT(handleControlLayerTypeChange(synthclone::ControlType)));
}

void
Participant::restoreTarget(const QVariant &state)
{
    const QVariantMap map = state.toMap();
    Target *target = addTarget();
    target->setControlCrossfadeCurve
        (getCrossfadeCurveConstant(map.value("controlCrossfadeCurve", "NONE").
                                   toString()));
    target->setDrumKit(map.value("drumKit", false).toBool());
    target->setName(map.value("name", "SFZ").toString());
    target->setNoteCrossfadeCurve
        (getCrossfadeCurveConstant(map.value("noteCrossfadeCurve", "NONE").
                                   toString()));
    target->setPath(map.value("path", "").toString());
    target->setVelocityCrossfadeCurve
        (getCrossfadeCurveConstant(map.value("velocityCrossfadeCurve", "NONE").
                                   toString()));

    QString formatStr = map.value("sampleFormat", "OGG_VORBIS").toString();
    SampleFormat format = formatStr == "WAV_8BIT" ? SAMPLEFORMAT_WAV_8BIT :
        formatStr == "WAV_16BIT" ? SAMPLEFORMAT_WAV_16BIT :
        formatStr == "WAV_24BIT" ? SAMPLEFORMAT_WAV_24BIT :
        formatStr == "WAV_32BIT" ? SAMPLEFORMAT_WAV_32BIT :
        formatStr == "WAV_32BIT_FLOAT" ? SAMPLEFORMAT_WAV_32BIT_FLOAT :
        SAMPLEFORMAT_OGG_VORBIS;
    target->setSampleFormat(format);

    // Control layers
    QList<synthclone::MIDIData> controls;
    QVariantList layers = map.value("controlLayers", QVariantList()).toList();
    int layerCount = layers.count();
    for (int i = 0; i < layerCount; i++) {
        QVariantMap layerMap = layers[i].toMap();
        synthclone::MIDIData control = static_cast<synthclone::MIDIData>
            (layerMap.value("control").toInt());
        if (controls.contains(control)) {
            qWarning() << tr("Control layer with control '%1' already loaded").
                arg(QLocale::system().toString(static_cast<int>(control)));
            continue;
        }
        ControlLayer *layer = target->addControlLayer(control);
        layer->setCrossfadingEnabled
            (layerMap.value("crossfadingEnabled", false).toBool());
        layer->setDefaultValue(static_cast<synthclone::MIDIData>
                               (layerMap.value("defaultValue", 0).toInt()));
        synthclone::ControlType type =
            layerMap.value("type", "CONTINUOUS").toString() == "CONTINUOUS" ?
            synthclone::CONTROLTYPE_CONTINUOUS :
            synthclone::CONTROLTYPE_SWITCH;
        layer->setType(type);
    }
}
