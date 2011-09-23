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

#include "participant.h"

Participant::Participant(QObject *parent):
    synthclone::Participant(tr("Hydrogen"), 0, 0, 1, "Devin Anderson",
                            tr("Creates Hydrogen drum kits."), parent),
    addTargetAction(tr("Hydrogen"))
{
    directoryView.setFilesVisible(false);
    directoryView.setOperation(synthclone::FileSelectionView::OPERATION_SAVE);
    directoryView.setTitle(tr("Save Hydrogen Kit"));
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

Target *
Participant::addTarget()
{
    Target *target = new Target(tr("Hydrogen"), this);
    connect(target, SIGNAL(authorChanged(QString)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(infoChanged(QString)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(layerAlgorithmChanged(LayerAlgorithm)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(licenseChanged(QString)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(nameChanged(QString)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(pathChanged(QString)),
            context, SLOT(setSessionModified()));
    connect(target, SIGNAL(sampleFormatChanged(SampleFormat)),
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
    targetView.setAuthor(target->getAuthor());
    targetView.setInfo(target->getInfo());
    targetView.setLayerAlgorithm(target->getLayerAlgorithm());
    targetView.setLicense(target->getLicense());
    targetView.setName(target->getName());
    targetView.setPath(target->getPath());
    targetView.setSampleFormat(target->getSampleFormat());

    connect(target, SIGNAL(authorChanged(const QString &)),
            &targetView, SLOT(setAuthor(const QString &)));
    connect(target, SIGNAL(infoChanged(const QString &)),
            &targetView, SLOT(setInfo(const QString &)));
    connect(target, SIGNAL(layerAlgorithmChanged(LayerAlgorithm)),
            &targetView, SLOT(setLayerAlgorithm(LayerAlgorithm)));
    connect(target, SIGNAL(licenseChanged(const QString &)),
            &targetView, SLOT(setLicense(const QString &)));
    connect(target, SIGNAL(nameChanged(const QString &)),
            &targetView, SLOT(setName(const QString &)));
    connect(target, SIGNAL(pathChanged(const QString &)),
            &targetView, SLOT(setPath(const QString &)));
    connect(target, SIGNAL(sampleFormatChanged(SampleFormat)),
            &targetView, SLOT(setSampleFormat(SampleFormat)));

    connect(&targetView, SIGNAL(authorChangeRequest(const QString &)),
            target, SLOT(setAuthor(const QString &)));
    connect(&targetView, SIGNAL(infoChangeRequest(const QString &)),
            target, SLOT(setInfo(const QString &)));
    connect(&targetView, SIGNAL(layerAlgorithmChangeRequest(LayerAlgorithm)),
            target, SLOT(setLayerAlgorithm(LayerAlgorithm)));
    connect(&targetView, SIGNAL(licenseChangeRequest(const QString &)),
            target, SLOT(setLicense(const QString &)));
    connect(&targetView, SIGNAL(nameChangeRequest(const QString &)),
            target, SLOT(setName(const QString &)));
    connect(&targetView, SIGNAL(pathChangeRequest(const QString &)),
            target, SLOT(setPath(const QString &)));
    connect(&targetView, SIGNAL(sampleFormatChangeRequest(SampleFormat)),
            target, SLOT(setSampleFormat(SampleFormat)));

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
    map["author"] = t->getAuthor();
    map["info"] = t->getInfo();
    map["layerAlgorithm"] = static_cast<int>(t->getLayerAlgorithm());
    map["license"] = t->getLicense();
    map["name"] = t->getName();
    map["path"] = t->getPath();
    map["sampleFormat"] = static_cast<int>(t->getSampleFormat());
    return map;
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
Participant::handleTargetViewCloseRequest()
{
    disconnect(configuredTarget, SIGNAL(authorChanged(const QString &)),
               &targetView, SLOT(setAuthor(const QString &)));
    disconnect(configuredTarget, SIGNAL(infoChanged(const QString &)),
               &targetView, SLOT(setInfo(const QString &)));
    disconnect(configuredTarget, SIGNAL(layerAlgorithmChanged(LayerAlgorithm)),
               &targetView, SLOT(setLayerAlgorithm(LayerAlgorithm)));
    disconnect(configuredTarget, SIGNAL(licenseChanged(const QString &)),
               &targetView, SLOT(setLicense(const QString &)));
    disconnect(configuredTarget, SIGNAL(nameChanged(const QString &)),
               &targetView, SLOT(setName(const QString &)));
    disconnect(configuredTarget, SIGNAL(pathChanged(const QString &)),
               &targetView, SLOT(setPath(const QString &)));
    disconnect(configuredTarget, SIGNAL(sampleFormatChanged(SampleFormat)),
               &targetView, SLOT(setSampleFormat(SampleFormat)));

    disconnect(&targetView, SIGNAL(authorChangeRequest(const QString &)),
               configuredTarget, SLOT(setAuthor(const QString &)));
    disconnect(&targetView, SIGNAL(infoChangeRequest(const QString &)),
               configuredTarget, SLOT(setInfo(const QString &)));
    disconnect(&targetView, SIGNAL(layerAlgorithmChangeRequest(LayerAlgorithm)),
               configuredTarget, SLOT(setLayerAlgorithm(LayerAlgorithm)));
    disconnect(&targetView, SIGNAL(licenseChangeRequest(const QString &)),
               configuredTarget, SLOT(setLicense(const QString &)));
    disconnect(&targetView, SIGNAL(nameChangeRequest(const QString &)),
               configuredTarget, SLOT(setName(const QString &)));
    disconnect(&targetView, SIGNAL(pathChangeRequest(const QString &)),
               configuredTarget, SLOT(setPath(const QString &)));
    disconnect(&targetView, SIGNAL(sampleFormatChangeRequest(SampleFormat)),
               configuredTarget, SLOT(setSampleFormat(SampleFormat)));

    targetView.setVisible(false);
    configuredTarget = 0;
}

void
Participant::handleTargetViewPathLookupRequest()
{
    assert(configuredTarget);
    directoryView.setDirectory(configuredTarget->getPath());
    directoryView.setVisible(true);
}

void
Participant::restoreTarget(const QVariant &state)
{
    const QVariantMap map = state.toMap();
    Target *target = addTarget();
    target->setAuthor(map.value("author", "").toString());
    target->setInfo(map.value("info", "").toString());
    target->setLayerAlgorithm
        (static_cast<LayerAlgorithm>
         (map.value("layerAlgorithm",
                    LAYERALGORITHM_LINEAR_INTERPOLATION).toInt()));
    target->setLicense(map.value("license", "").toString());
    target->setName(map.value("name", "").toString());
    target->setPath(map.value("path", "").toString());
    target->setSampleFormat
        (static_cast<SampleFormat>(map.value("sampleFormat",
                                             SAMPLEFORMAT_FLAC_24BIT).toInt()));
}
