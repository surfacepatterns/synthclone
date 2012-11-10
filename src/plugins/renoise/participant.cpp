/*
 * libsynthclone_renoise - Renoise target plugin for `synthclone`
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

#include "participant.h"

Participant::Participant(QObject *parent):
    synthclone::Participant(tr("Renoise"), 0, 0, 1, "Devin Anderson",
                            tr("Creates Renoise instruments."), parent),
    addTargetAction(tr("Renoise"))
{
    fileView.setFilesVisible(true);
    fileView.setOperation(synthclone::FileSelectionView::OPERATION_SAVE);
    fileView.setSelectionFilter
        (synthclone::FileSelectionView::SELECTIONFILTER_ANY_FILE);
    fileView.setTitle(tr("Save Renoise Instrument"));
    connect(&addTargetAction, SIGNAL(triggered()),
            SLOT(handleTargetAddition()));
    connect(&fileView, SIGNAL(closeRequest()),
            SLOT(handleFileViewCloseRequest()));
    connect(&fileView, SIGNAL(pathsSelected(const QStringList &)),
            SLOT(handleFileViewPathSelection(const QStringList &)));
    connect(&targetView, SIGNAL(closeRequest()),
            SLOT(handleTargetViewCloseRequest()));
    connect(&targetView, SIGNAL(pathLookupRequest()),
            SLOT(handleTargetViewPathLookupRequest()));
    configuredTarget = 0;
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

Target *
Participant::addTarget()
{
    Target *target = new Target(tr("Renoise"), this);
    connect(target, SIGNAL(drumKitChanged(bool)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(instrumentNameChanged(QString)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(layerAlgorithmChanged(LayerAlgorithm)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(nameChanged(QString)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(pathChanged(QString)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(pitchInterpolationChanged(PitchInterpolation)),
            context, SLOT(setSessionModified()));

    synthclone::MenuAction *action =
        new synthclone::MenuAction(tr("Configure"), target);
    connect(action, SIGNAL(triggered()), SLOT(handleTargetConfiguration()));

    const synthclone::Registration &targetRegistration =
        context->addTarget(target);
    connect(&targetRegistration, SIGNAL(unregistered(QObject *)),
            SLOT(handleUnregistration(QObject *)));

    const synthclone::Registration &actionRegistration =
        context->addMenuAction(action, target);
    connect(&actionRegistration, SIGNAL(unregistered(QObject *)),
            SLOT(handleUnregistration(QObject *)));

    return target;
}

void
Participant::configureTarget(Target *target)
{
    assert(! configuredTarget);
    targetView.setDrumKit(target->isDrumKit());
    targetView.setInstrumentName(target->getInstrumentName());
    targetView.setLayerAlgorithm(target->getLayerAlgorithm());
    targetView.setName(target->getName());
    targetView.setPath(target->getPath());
    targetView.setPitchInterpolation(target->getPitchInterpolation());

    connect(target, SIGNAL(drumKitChanged(bool)),
            &targetView, SLOT(setDrumKit(bool)));
    connect(target, SIGNAL(instrumentNameChanged(const QString &)),
            &targetView, SLOT(setInstrumentName(const QString &)));
    connect(target, SIGNAL(layerAlgorithmChanged(LayerAlgorithm)),
            &targetView, SLOT(setLayerAlgorithm(LayerAlgorithm)));
    connect(target, SIGNAL(nameChanged(const QString &)),
            &targetView, SLOT(setName(const QString &)));
    connect(target, SIGNAL(pathChanged(const QString &)),
            &targetView, SLOT(setPath(const QString &)));
    connect(target, SIGNAL(pitchInterpolationChanged(PitchInterpolation)),
            &targetView, SLOT(setPitchInterpolation(PitchInterpolation)));

    connect(&targetView, SIGNAL(drumKitChangeRequest(bool)),
            target, SLOT(setDrumKit(bool)));
    connect(&targetView, SIGNAL(instrumentNameChangeRequest(const QString &)),
            target, SLOT(setInstrumentName(const QString &)));
    connect(&targetView, SIGNAL(layerAlgorithmChangeRequest(LayerAlgorithm)),
            target, SLOT(setLayerAlgorithm(LayerAlgorithm)));
    connect(&targetView, SIGNAL(nameChangeRequest(const QString &)),
            target, SLOT(setName(const QString &)));
    connect(&targetView, SIGNAL(pathChangeRequest(const QString &)),
            target, SLOT(setPath(const QString &)));
    connect(&targetView,
            SIGNAL(pitchInterpolationChangeRequest(PitchInterpolation)),
            target, SLOT(setPitchInterpolation(PitchInterpolation)));

    targetView.setVisible(true);
    configuredTarget = target;
}

void
Participant::deactivate(synthclone::Context &context)
{
    context.removeMenuAction(&addTargetAction);
    this->context = 0;
}

QVariant
Participant::getState(const synthclone::Target *target) const
{
    const Target *t = qobject_cast<const Target *>(target);
    assert(t);
    QVariantMap map;
    map["drumKit"] = t->isDrumKit();
    map["instrumentName"] = t->getInstrumentName();
    map["name"] = t->getName();
    map["path"] = t->getPath();

    QString algorithmStr;
    switch (t->getLayerAlgorithm()) {
    case LAYERALGORITHM_LINEAR_INTERPOLATION:
        algorithmStr = "LINEAR_INTERPOLATION";
        break;
    case LAYERALGORITHM_MAXIMUM:
        algorithmStr = "MAXIMUM";
        break;
    case LAYERALGORITHM_MINIMUM:
        algorithmStr = "MINIMUM";
        break;
    default:
        assert(false);
    }
    map["layerAlgorithm"] = algorithmStr;

    QString interpolationStr;
    switch (t->getPitchInterpolation()) {
    case PITCHINTERPOLATION_CUBIC:
        interpolationStr = "CUBIC";
        break;
    case PITCHINTERPOLATION_LINEAR:
        interpolationStr = "LINEAR";
        break;
    case PITCHINTERPOLATION_NONE:
        interpolationStr = "NONE";
        break;
    default:
        assert(false);
    }
    map["pitchInterpolation"] = interpolationStr;

    return map;
}

void
Participant::handleFileViewCloseRequest()
{
    fileView.setVisible(false);
}

void
Participant::handleFileViewPathSelection(const QStringList &paths)
{
    assert(paths.count() == 1);
    configuredTarget->setPath(paths[0]);
    fileView.setVisible(false);
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
Participant::handleTargetViewCloseRequest()
{
    disconnect(configuredTarget, SIGNAL(drumKitChanged(bool)),
               &targetView, SLOT(setDrumKit(bool)));
    disconnect(configuredTarget, SIGNAL(instrumentNameChanged(const QString &)),
               &targetView, SLOT(setInstrumentName(const QString &)));
    disconnect(configuredTarget,
               SIGNAL(layerAlgorithmChanged(LayerAlgorithm)),
               &targetView, SLOT(setLayerAlgorithm(LayerAlgorithm)));
    disconnect(configuredTarget, SIGNAL(nameChanged(const QString &)),
               &targetView, SLOT(setName(const QString &)));
    disconnect(configuredTarget, SIGNAL(pathChanged(const QString &)),
               &targetView, SLOT(setPath(const QString &)));
    disconnect(configuredTarget,
               SIGNAL(pitchInterpolationChanged(PitchInterpolation)),
               &targetView, SLOT(setPitchInterpolation(PitchInterpolation)));

    disconnect(&targetView, SIGNAL(drumKitChangeRequest(bool)),
               configuredTarget, SLOT(setDrumKit(bool)));
    disconnect(&targetView,
               SIGNAL(instrumentNameChangeRequest(const QString &)),
               configuredTarget, SLOT(setInstrumentName(const QString &)));
    disconnect(&targetView,
               SIGNAL(layerAlgorithmChangeRequest(LayerAlgorithm)),
               configuredTarget, SLOT(setLayerAlgorithm(LayerAlgorithm)));
    disconnect(&targetView, SIGNAL(nameChangeRequest(const QString &)),
               configuredTarget, SLOT(setName(const QString &)));
    disconnect(&targetView, SIGNAL(pathChangeRequest(const QString &)),
               configuredTarget, SLOT(setPath(const QString &)));
    disconnect(&targetView,
               SIGNAL(pitchInterpolationChangeRequest(PitchInterpolation)),
               configuredTarget,
               SLOT(setPitchInterpolation(PitchInterpolation)));

    targetView.setVisible(false);
    configuredTarget = 0;
}

void
Participant::handleTargetViewPathLookupRequest()
{
    assert(configuredTarget);
    fileView.setDirectory(QFileInfo(configuredTarget->getPath()).
                          canonicalPath());
    fileView.setVisible(true);
}

void
Participant::handleUnregistration(QObject *obj)
{
    delete obj;
}

void
Participant::restoreTarget(const QVariant &state)
{
    const QVariantMap map = state.toMap();
    Target *target = addTarget();
    target->setDrumKit(map.value("drumKit", false).toBool());
    target->setInstrumentName(map.value("instrumentName", "").toString());
    target->setName(map.value("name", "").toString());
    target->setPath(map.value("path", "").toString());

    QString algorithmStr =
        map.value("layerAlgorithm", "LINEAR_INTERPOLATION").toString();
    target->setLayerAlgorithm
        (algorithmStr == "MAXIMUM" ? LAYERALGORITHM_MAXIMUM :
         algorithmStr == "MINIMUM" ? LAYERALGORITHM_MINIMUM :
         LAYERALGORITHM_LINEAR_INTERPOLATION);

    QString interpolationStr =
        map.value("pitchInterpolation", "CUBIC").toString();
    target->setPitchInterpolation
        (interpolationStr == "LINEAR" ? PITCHINTERPOLATION_LINEAR :
         interpolationStr == "NONE" ? PITCHINTERPOLATION_NONE :
         PITCHINTERPOLATION_CUBIC);
}
