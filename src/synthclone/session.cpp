/*
 * synthclone - Synthesizer-cloning software
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
#include <cctype>

#include <QtCore/QFSFileEngine>
#include <QtCore/QScopedPointer>
#include <QtCore/QTemporaryFile>
#include <QtCore/QtAlgorithms>

#include <synthclone/error.h>
#include <synthclone/util.h>

#include "context.h"
#include "effectjob.h"
#include "samplerjob.h"
#include "session.h"
#include "util.h"
#include "zonecomparerproxy.h"
#include "zonelistloader.h"

// Static functions

void
Session::create(const QDir &directory, synthclone::SampleRate sampleRate,
                synthclone::SampleChannelCount count)
{
    CONFIRM(count > 0, tr("'%1': invalid sample channel count").arg(count));
    CONFIRM(sampleRate > 0, tr("'%1': invalid sample rate").arg(sampleRate));

    initializeDirectory(directory);
    QFile file;
    QXmlStreamWriter writer;
    initializeWriter(writer, file, directory);
    writer.writeStartDocument();
    writer.writeStartElement("synthclone-session");
    writer.writeAttribute("major-version",
                          QString::number(SYNTHCLONE_MAJOR_VERSION));
    writer.writeAttribute("minor-version",
                          QString::number(SYNTHCLONE_MINOR_VERSION));
    writer.writeAttribute("revision", QString::number(SYNTHCLONE_REVISION));
    writer.writeAttribute("sample-channel-count", QString::number(count));
    writer.writeAttribute("sample-rate", QString::number(sampleRate));
    writer.writeAttribute("aftertouch-property-visible", "false");
    writer.writeAttribute("channel-property-visible", "true");
    writer.writeAttribute("dry-sample-property-visible", "true");
    writer.writeAttribute("note-property-visible", "true");
    writer.writeAttribute("release-time-property-visible", "true");
    writer.writeAttribute("sample-time-property-visible", "true");
    writer.writeAttribute("status-property-visible", "true");
    writer.writeAttribute("velocity-property-visible", "true");
    writer.writeAttribute("wet-sample-property-visible", "true");
    QString controlTemplate = "control-property-%1-visible";
    for (synthclone::MIDIData i = 0; i < 0x80; i++) {
        writer.writeAttribute(controlTemplate.arg(i), "false");
    }

    writer.writeEmptyElement("zones");
    writer.writeEmptyElement("participants");
    writer.writeEmptyElement("sampler");
    writer.writeEmptyElement("effects");
    writer.writeEmptyElement("targets");
    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
}

void
Session::initializeDirectory(const QDir &directory)
{
    if (! directory.exists()) {
        QString path = directory.absolutePath();
        if (! QFSFileEngine().mkdir(path, true)) {
            throw synthclone::Error(tr("failed to create session directory at "
                                       "'%1'").arg(path));
        }
    }
}

void
Session::initializeWriter(QXmlStreamWriter &writer, QFile &file,
                          const QDir &directory)
{
    file.setFileName(directory.absoluteFilePath("synthclone-session.xml"));
    if (! file.open(QIODevice::WriteOnly)) {
        throw synthclone::Error(file.errorString());
    }
    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(-1);
}

bool
Session::isDirectory(const QDir &directory)
{
    QDomDocument document;
    return loadXML(directory, document);
}

bool
Session::loadXML(const QDir &directory, QDomDocument &document)
{
    bool result = directory.exists();
    if (result) {
        QFile file(directory.absoluteFilePath("synthclone-session.xml"));
        result = file.exists();
        if (result) {
            if (! file.open(QIODevice::ReadOnly)) {
                throw synthclone::Error(file.errorString());
            }
            int column;
            int line;
            QString message;
            result = document.setContent(&file, &message, &line, &column);
            file.close();
            if (! result) {
                QString s = tr("XML parse error at line %1, column %2: %3").
                    arg(line).arg(column).arg(message);
                throw synthclone::Error(s);
            }
            QString tag = document.documentElement().tagName();
            result = tag == "synthclone-session";
        }
    }
    return result;
}

// Class definition

Session::Session(QCoreApplication &application, QObject *parent):
    QObject(parent),
    application(application),
    effectJobThread(this),
    zoneIndexComparer(zones)
{
    connect(this, SIGNAL(effectJobCompletion()),
            SLOT(handleEffectJobCompletion()));
    connect(this, SIGNAL(effectJobError(QString)),
            SLOT(handleEffectJobError(QString)));
    for (int i = 0; i < 0x80; i++) {
        controlPropertiesVisible[i] = false;
    }
    aftertouchPropertyVisible = false;
    channelPropertyVisible = true;
    currentEffectJob = 0;
    currentEffectJobWetSample = 0;
    currentSamplerJob = 0;
    currentSamplerJobSample = 0;
    currentSamplerJobStream = 0;
    directory = 0;
    drySamplePropertyVisible = true;
    notePropertyVisible = true;
    releaseTimePropertyVisible = true;
    sampler = 0;
    sampleTimePropertyVisible = true;
    selectedEffect = 0;
    selectedTarget = 0;
    state = synthclone::SESSIONSTATE_CURRENT;
    statusPropertyVisible = true;
    velocityPropertyVisible = true;
    wetSamplePropertyVisible = true;
}

Session::~Session()
{
    unload();
    QList<const synthclone::Participant *> participants =
        participantDataMap.keys();
    for (int i = participants.count() - 1; i >= 0; i--) {
        ParticipantData *data = participantDataMap[participants[i]];
        if (! data->parent) {
            removeParticipant(data->participant);
        }
    }
    assert(! participantDataMap.count());
    assert(! participantIdMap.count());
    assert(! rootParticipants.count());
}

void
Session::abortCurrentSamplerJob()
{
    CONFIRM(sampler, tr("sampler is not registered with session"));
    sampler->abortJob();
}

void
Session::activateParticipant(const synthclone::Participant *participant)
{
    activateParticipant(participant, QVariant());
}

void
Session::activateParticipant(const synthclone::Participant *participant,
                             const QVariant &state)
{
    CONFIRM(participant, tr("participant is set to NULL"));

    ParticipantData *data = participantDataMap.value(participant, 0);

    CONFIRM(data, tr("participant is not registered with session"));
    CONFIRM(! data->context, tr("participant is already activated"));

    QByteArray &id = data->id;
    const synthclone::Participant *parent = data->parent;
    synthclone::Participant *mutableParticipant = data->participant;
    emit activatingParticipant(participant, parent, id);
    Context *context = new Context(*mutableParticipant, *this, this);
    mutableParticipant->activate(*context, state);
    data->context = context;
    emit participantActivated(participant, parent, id);
    setModified();
}

const synthclone::Registration &
Session::addEffect(synthclone::Effect *effect,
                   const synthclone::Participant *participant, int index)
{
    CONFIRM(effect, tr("effect is set to NULL"));
    CONFIRM(! effects.contains(effect),
            tr("effect is already registered with session"));
    CONFIRM((index >= -1) && (index <= effects.count()),
            tr("'%1': index is out of range").arg(index));
    CONFIRM(participant, tr("participant is set to NULL"));
    CONFIRM(isParticipantActivated(participant),
            tr("participant is not activated"));
    CONFIRM(! currentEffectJob, tr("effects are currently in use"));

    if (index == -1) {
        index = effects.count();
    }
    emit addingEffect(effect, index);
    ComponentData *data = new ComponentData();
    Registration *registration = new Registration(effect, this);
    data->participant = participant;
    data->registration = registration;
    effectDataMap.insert(effect, data);
    effects.insert(index, effect);
    emit effectAdded(effect, index);
    setModified();
    return *registration;
}

const synthclone::EffectJob *
Session::addEffectJob(synthclone::Zone *zone, int index)
{
    CONFIRM((index >= -1) && (index <= effectJobs.count()),
            tr("'%1': index is out of range").arg(index));
    CONFIRM(zone, tr("zone is set to NULL"));
    CONFIRM(zone->getDrySample(), tr("zone does not have a dry sample"));
    CONFIRM(zone->getStatus() == synthclone::Zone::STATUS_NORMAL,
            tr("zone is being used by a component"));
    getZoneIndex(zone);

    if (index == -1) {
        index = effectJobs.count();
    }
    EffectJob *job = new EffectJob(qobject_cast<Zone *>(zone), this);
    emit addingEffectJob(job, index);
    effectJobs.insert(index, job);
    zoneEffectJobMap.insert(zone, job);
    qobject_cast<Zone *>(zone)->
        setStatus(synthclone::Zone::STATUS_EFFECT_JOB_QUEUE);
    emit effectJobAdded(job, index);
    if (! currentEffectJob) {
        updateEffectJobs();
    }
    setModified();
    return job;
}

const synthclone::Registration &
Session::addMenuAction(synthclone::MenuAction *action, synthclone::Menu menu,
                       const QStringList &subMenus)
{
    CONFIRM(action, tr("action is set to NULL"));
    CONFIRM(! menuActionDataMap.contains(action),
            tr("menu action is already registered with session"));

    emit addingMenuAction(action, menu, subMenus);
    MenuItemData *data = new MenuItemData();
    Registration *registration = new Registration(action, this);
    data->menuBase.menu = menu;
    data->menuBaseType = MENUBASETYPE_MENU;
    data->registration = registration;
    data->subMenus = subMenus;
    menuActionDataMap.insert(action, data);
    emit menuActionAdded(action, menu, subMenus);
    setModified();
    return *registration;
}

const synthclone::Registration &
Session::addMenuAction(synthclone::MenuAction *action,
                       const synthclone::Effect *effect,
                       const QStringList &subMenus)
{
    CONFIRM(action, tr("action is set to NULL"));
    CONFIRM(! menuActionDataMap.contains(action),
            tr("menu action is already registered with session"));
    CONFIRM(effect, tr("effect is set to NULL"));

    ComponentData *effectData = effectDataMap.value(effect, 0);

    CONFIRM(effectData, tr("effect is not registered with session"));

    emit addingMenuAction(action, effect, subMenus);
    MenuItemData *data = new MenuItemData();
    Registration *registration = new Registration(action, this);
    data->menuBase.effect = effect;
    data->menuBaseType = MENUBASETYPE_EFFECT;
    data->registration = registration;
    data->subMenus = subMenus;
    menuActionDataMap.insert(action, data);
    effectData->menuActions.append(action);
    emit menuActionAdded(action, effect, subMenus);
    setModified();
    return *registration;
}

const synthclone::Registration &
Session::addMenuAction(synthclone::MenuAction *action,
                       const synthclone::Sampler *sampler,
                       const QStringList &subMenus)
{
    CONFIRM(action, tr("action is set to NULL"));
    CONFIRM(! menuActionDataMap.contains(action),
            tr("menu action is already registered with session"));
    CONFIRM(sampler, tr("sampler is set to NULL"));
    CONFIRM(sampler == this->sampler,
            tr("sampler is not registered with session"));

    emit addingMenuAction(action, sampler, subMenus);
    MenuItemData *data = new MenuItemData();
    Registration *registration = new Registration(action, this);
    data->menuBase.sampler = sampler;
    data->menuBaseType = MENUBASETYPE_SAMPLER;
    data->registration = registration;
    data->subMenus = subMenus;
    menuActionDataMap.insert(action, data);
    samplerData.menuActions.append(action);
    emit menuActionAdded(action, sampler, subMenus);
    setModified();
    return *registration;
}

const synthclone::Registration &
Session::addMenuAction(synthclone::MenuAction *action,
                       const synthclone::Target *target,
                       const QStringList &subMenus)
{
    CONFIRM(action, tr("action is set to NULL"));
    CONFIRM(! menuActionDataMap.contains(action),
            tr("menu action is already registered with session"));
    CONFIRM(target, tr("target is set to NULL"));

    ComponentData *targetData = targetDataMap.value(target, 0);

    CONFIRM(targetData, tr("target is not registered with session"));

    emit addingMenuAction(action, target, subMenus);
    MenuItemData *data = new MenuItemData();
    Registration *registration = new Registration(action, this);
    data->menuBase.target = target;
    data->menuBaseType = MENUBASETYPE_TARGET;
    data->registration = registration;
    data->subMenus = subMenus;
    menuActionDataMap.insert(action, data);
    targetData->menuActions.append(action);
    emit menuActionAdded(action, target, subMenus);
    setModified();
    return *registration;
}

const synthclone::Registration &
Session::addMenuSeparator(synthclone::MenuSeparator *separator,
                          synthclone::Menu menu, const QStringList &subMenus)
{
    CONFIRM(separator, tr("separator is set to NULL"));
    CONFIRM(! menuSeparatorDataMap.contains(separator),
            tr("menu separator is already registered with session"));

    emit addingMenuSeparator(separator, menu, subMenus);
    MenuItemData *data = new MenuItemData();
    Registration *registration = new Registration(separator, this);
    data->menuBase.menu = menu;
    data->menuBaseType = MENUBASETYPE_MENU;
    data->registration = registration;
    data->subMenus = subMenus;
    menuSeparatorDataMap.insert(separator, data);
    emit menuSeparatorAdded(separator, menu, subMenus);
    setModified();
    return *registration;
}

const synthclone::Registration &
Session::addMenuSeparator(synthclone::MenuSeparator *separator,
                          const synthclone::Effect *effect,
                          const QStringList &subMenus)
{
    CONFIRM(separator, tr("separator is set to NULL"));
    CONFIRM(! menuSeparatorDataMap.contains(separator),
            tr("menu separator is already registered with session"));
    CONFIRM(effect, tr("effect is set to NULL"));

    ComponentData *effectData = effectDataMap.value(effect, 0);

    CONFIRM(effectData, tr("effect is not registered with session"));

    emit addingMenuSeparator(separator, effect, subMenus);
    MenuItemData *data = new MenuItemData();
    Registration *registration = new Registration(separator, this);
    data->menuBase.effect = effect;
    data->menuBaseType = MENUBASETYPE_EFFECT;
    data->registration = registration;
    data->subMenus = subMenus;
    menuSeparatorDataMap.insert(separator, data);
    effectData->menuSeparators.append(separator);
    emit menuSeparatorAdded(separator, effect, subMenus);
    setModified();
    return *registration;
}

const synthclone::Registration &
Session::addMenuSeparator(synthclone::MenuSeparator *separator,
                          const synthclone::Sampler *sampler,
                          const QStringList &subMenus)
{
    CONFIRM(separator, tr("separator is set to NULL"));
    CONFIRM(! menuSeparatorDataMap.contains(separator),
            tr("menu separator is already registered with session"));
    CONFIRM(sampler, tr("sampler is set to NULL"));
    CONFIRM(sampler == this->sampler,
            tr("sampler is not registered with session"));

    emit addingMenuSeparator(separator, sampler, subMenus);
    MenuItemData *data = new MenuItemData();
    Registration *registration = new Registration(separator, this);
    data->menuBase.sampler = sampler;
    data->menuBaseType = MENUBASETYPE_SAMPLER;
    data->registration = registration;
    data->subMenus = subMenus;
    menuSeparatorDataMap.insert(separator, data);
    samplerData.menuSeparators.append(separator);
    emit menuSeparatorAdded(separator, sampler, subMenus);
    setModified();
    return *registration;
}

const synthclone::Registration &
Session::addMenuSeparator(synthclone::MenuSeparator *separator,
                          const synthclone::Target *target,
                          const QStringList &subMenus)
{
    CONFIRM(separator, tr("separator is set to NULL"));
    CONFIRM(! menuSeparatorDataMap.contains(separator),
            tr("menu separator is already registered with session"));
    CONFIRM(target, tr("target is set to NULL"));

    ComponentData *targetData = targetDataMap.value(target, 0);

    CONFIRM(targetData, tr("target is not registered with session"));

    emit addingMenuSeparator(separator, target, subMenus);
    MenuItemData *data = new MenuItemData();
    Registration *registration = new Registration(separator, this);
    data->menuBase.target = target;
    data->menuBaseType = MENUBASETYPE_TARGET;
    data->registration = registration;
    data->subMenus = subMenus;
    menuSeparatorDataMap.insert(separator, data);
    targetData->menuSeparators.append(separator);
    emit menuSeparatorAdded(separator, target, subMenus);
    setModified();
    return *registration;
}

const synthclone::Registration &
Session::addParticipant(synthclone::Participant *participant,
                        const QByteArray &id)
{
    CONFIRM(participant, tr("participant is set to NULL"));
    CONFIRM(! participantDataMap.value(participant, 0),
            tr("participant is already registered with session"));
    QList<QByteArray> parts = id.split('.');
    for (int i = parts.count() - 1; i >= 0; i--) {
        CONFIRM(verifySubId(parts[i]),
                tr("'%1': invalid participant id").arg(id.constData()));
    }

    emit addingParticipant(participant, 0, id);
    ParticipantData *data = new ParticipantData();
    Registration *registration = new Registration(participant, this);
    data->context = 0;
    data->id = id;
    data->parent = 0;
    data->participant = participant;
    data->registration = registration;
    participantDataMap[participant] = data;
    participantIdMap[id] = participant;
    rootParticipants.append(participant);
    emit participantAdded(participant, 0, id);
    setModified();
    return *registration;
}

const synthclone::Registration &
Session::addParticipant(synthclone::Participant *participant,
                        const synthclone::Participant *parent,
                        const QByteArray &subId)
{
    CONFIRM(participant, tr("participant is set to NULL"));
    CONFIRM(participantDataMap.contains(participant),
            tr("participant is already registered with session"));
    CONFIRM(verifySubId(subId),
            tr("'%1': invalid participant sub-id").arg(subId.constData()));
    assert(parent);

    ParticipantData *parentData = participantDataMap.value(parent, 0);

    assert(parentData);
    assert(parentData->context);

    QByteArray id;
    id.append(parentData->id);
    id.append('.');
    id.append(subId);
    emit addingParticipant(participant, parent, id);
    ParticipantData *data = new ParticipantData();
    Registration *registration = new Registration(participant, this);
    data->context = 0;
    data->id = id;
    data->parent = parent;
    data->participant = participant;
    data->registration = registration;
    participantDataMap[participant] = data;
    participantIdMap[id] = participant;
    parentData->children.append(participant);
    emit participantAdded(participant, parent, id);
    setModified();
    return *registration;
}

const synthclone::Registration &
Session::addSampler(synthclone::Sampler *sampler,
                    const synthclone::Participant *participant)
{
    CONFIRM(sampler, tr("sampler is set to NULL"));
    CONFIRM(sampler != this->sampler,
            tr("sampler is already registered with session"));
    CONFIRM(participant, tr("participant is set to NULL"));
    CONFIRM(isParticipantActivated(participant),
            tr("participant is not activated"));

    if (this->sampler) {
        removeSampler();
    }
    emit addingSampler(sampler);
    Registration *registration = new Registration(sampler, this);
    samplerData.participant = participant;
    samplerData.registration = registration;
    this->sampler = sampler;

    connect(sampler, SIGNAL(jobAborted()), SLOT(handleSamplerJobAbort()));
    connect(sampler, SIGNAL(jobCompleted()),
            SLOT(handleSamplerJobCompletion()));
    connect(sampler, SIGNAL(jobError(const QString &)),
            SLOT(handleSamplerJobError(const QString &)));

    emit samplerAdded(sampler);
    setModified();
    return *registration;
}

const synthclone::SamplerJob *
Session::addSamplerJob(synthclone::SamplerJob::Type type,
                       synthclone::Zone *zone, int index)
{
    CONFIRM((index >= -1) && (index <= samplerJobs.count()),
            tr("'%1': index is out of range").arg(index));
    CONFIRM(zone, tr("zone is set to NULL"));
    CONFIRM(zone->getStatus() == synthclone::Zone::STATUS_NORMAL,
            tr("zone is being used by a component"));
    getZoneIndex(zone);

    switch (type) {
    case synthclone::SamplerJob::TYPE_PLAY_DRY_SAMPLE:
        CONFIRM(zone->getDrySample(), tr("zone's dry sample is set to NULL"));
        break;
    case synthclone::SamplerJob::TYPE_PLAY_WET_SAMPLE:
        CONFIRM(zone->getWetSample(), tr("zone's wet sample is set to NULL"));
        // Fallthrough on purpose.
    case synthclone::SamplerJob::TYPE_SAMPLE:
        ;
    }

    if (index == -1) {
        index = samplerJobs.count();
    }
    Zone *zoneCopy = qobject_cast<Zone *>(zone);
    SamplerJob *job = new SamplerJob(type, zoneCopy, this);
    emit addingSamplerJob(job, index);
    samplerJobs.insert(index, job);
    zoneSamplerJobMap.insert(zone, job);
    zoneCopy->setStatus(synthclone::Zone::STATUS_SAMPLER_JOB_QUEUE);
    emit samplerJobAdded(job, index);
    if (! currentSamplerJob) {
        updateSamplerJobs();
    }
    setModified();
    return job;
}

const synthclone::Registration &
Session::addTarget(synthclone::Target *target,
                   const synthclone::Participant *participant, int index)
{
    CONFIRM(target, tr("target is set to NULL"));
    CONFIRM(! targets.contains(target),
            tr("target is already registered with session"));
    CONFIRM((index >= -1) && (index <= targets.count()),
            tr("'%1': index is out of range").arg(index));
    CONFIRM(participant, tr("participant is set to NULL"));
    CONFIRM(isParticipantActivated(participant),
            tr("participant is not activated"));

    if (index == -1) {
        index = targets.count();
    }
    emit addingTarget(target, index);
    ComponentData *data = new ComponentData();
    Registration *registration = new Registration(target, this);
    data->participant = participant;
    data->registration = registration;
    targetDataMap.insert(target, data);
    targets.insert(index, target);
    emit targetAdded(target, index);
    setModified();
    return *registration;
}

synthclone::Zone *
Session::addZone(int index)
{
    CONFIRM((index >= -1) && (index <= zones.count()),
            tr("'%1': index is out of range").arg(index));

    if (index == -1) {
        index = zones.count();
    }
    Zone *zone = new Zone(sessionSampleData, this);

    emit addingZone(zone, index);
    zones.insert(index, zone);

    connect(zone, SIGNAL(aftertouchChanged(synthclone::MIDIData)),
            SLOT(setModified()));
    connect(zone, SIGNAL(channelChanged(synthclone::MIDIData)),
            SLOT(setModified()));
    connect(zone, SIGNAL(controlValueChanged(synthclone::MIDIData,
                                             synthclone::MIDIData)),
            SLOT(setModified()));
    connect(zone, SIGNAL(drySampleChanged(const synthclone::Sample *)),
            SLOT(setModified()));
    connect(zone, SIGNAL(noteChanged(synthclone::MIDIData)),
            SLOT(setModified()));
    connect(zone, SIGNAL(releaseTimeChanged(synthclone::SampleTime)),
            SLOT(setModified()));
    connect(zone, SIGNAL(sampleTimeChanged(synthclone::SampleTime)),
            SLOT(setModified()));
    connect(zone, SIGNAL(velocityChanged(synthclone::MIDIData)),
            SLOT(setModified()));
    connect(zone, SIGNAL(wetSampleChanged(const synthclone::Sample *)),
            SLOT(setModified()));

    emit zoneAdded(zone, index);
    setModified();
    return zone;
}

void
Session::buildTargets()
{
    int count = targets.count();

    CONFIRM(count, tr("no targets are registered with session"));

    emit buildingTargets();
    for (int i = 0; i < zones.count(); i++) {
        Zone *zone = qobject_cast<Zone *>(zones[i]);

        CONFIRM(zone->getStatus() == synthclone::Zone::STATUS_NORMAL,
                tr("one or more zones is being used by a component"));

        zone->setStatus(synthclone::Zone::STATUS_TARGETS);
    }
    for (int i = 0; i < count; i++) {
        synthclone::Target *target = targets[i];
        connect(target, SIGNAL(validationError(QString)),
                SLOT(handleTargetValidationError()));
        targetValidationErrorsOccurred = false;
        emit validatingTarget(target);
        target->validate(zones);
        emit targetValidationCompleted(target);
        disconnect(target, SIGNAL(validationError(QString)),
                   this, SLOT(handleTargetValidationError()));
        if (! targetValidationErrorsOccurred) {
            emit savingTarget(target);
            try {
                target->save(zones);
            } catch (synthclone::Error &e) {
                emit targetSaveError(target, e.getMessage());
                continue;
            }
            emit targetSaved(target);
        }
    }
    for (int i = 0; i < zones.count(); i++) {
        qobject_cast<Zone *>(zones[i])->
            setStatus(synthclone::Zone::STATUS_NORMAL);
    }
    emit targetBuildingCompleted();
}

QString
Session::createUniqueSampleFile(const QDir &sessionDirectory)
{
    QDir samplesDirectory(getSamplesDirectory(sessionDirectory));
    return createUniqueFile(&samplesDirectory);
}

void
Session::deactivateParticipant(const synthclone::Participant *participant)
{
    CONFIRM(participant, tr("participant is set to NULL"));

    ParticipantData *data = participantDataMap.value(participant, 0);

    CONFIRM(data, tr("participant is not registered with session"));

    Context *context = data->context;

    CONFIRM(data->context, tr("participant is not currently activated"));

    ParticipantList &children = data->children;
    for (int i = children.count() - 1; i >= 0; i--) {
        removeParticipant(children[i]);
    }
    const QByteArray &id = data->id;
    const synthclone::Participant *parent = data->parent;
    emit deactivatingParticipant(participant, parent, id);
    data->participant->deactivate(*context);
    data->context = 0;
    delete context;
    emit participantDeactivated(participant, parent, id);
    setModified();
}

void
Session::emitLoadWarning(const QDomElement &element, const QString &message)
{
    emit loadWarning(element.lineNumber(), element.columnNumber(), message);
}

synthclone::Participant *
Session::getActivatedParticipant(const QDomElement &element)
{
    QString id = element.attribute("participant-id");
    QString message;
    if (id.isEmpty()) {
        message = tr("element does not contain 'participant-id' attribute");
        emitLoadWarning(element, message);
        return 0;
    }
    synthclone::Participant *participant =
        participantIdMap.value(id.toAscii(), 0);
    if (! participant) {
        message = tr("participant with id '%1' not found").arg(id);
        emitLoadWarning(element, message);
        return 0;
    }
    ParticipantData *data = participantDataMap.value(participant, 0);
    assert(data);
    if (! data->context) {
        message = tr("participant with id '%1' is not activated").arg(id);
        emitLoadWarning(element, message);
        return 0;
    }
    return participant;
}

int
Session::
getActivatedParticipantCount(const synthclone::Participant *parent) const
{
    const ParticipantList *participants;
    if (! parent) {
        participants = &rootParticipants;
    } else {
        ParticipantData *data = participantDataMap.value(parent, 0);
        CONFIRM(data, tr("the given parent is not a registered participant"));
        participants = &(data->children);
    }
    int activatedCount = 0;
    for (int i = participants->count() - 1; i >= 0; i--) {
        if (isParticipantActivated(participants->at(i))) {
            activatedCount++;
        }
    }
    return activatedCount;
}

const synthclone::EffectJob *
Session::getCurrentEffectJob() const
{
    return currentEffectJob;
}

const synthclone::SamplerJob *
Session::getCurrentSamplerJob() const
{
    return currentSamplerJob;
}

const QDir *
Session::getDirectory() const
{
    return directory;
}

const synthclone::Effect *
Session::getEffect(int index) const
{
    CONFIRM((index >= 0) && (index < effects.count()),
            tr("'%1': index is out of range").arg(index));

    return effects[index];
}

int
Session::getEffectCount() const
{
    return effects.count();
}

int
Session::getEffectIndex(const synthclone::Effect *effect) const
{
    CONFIRM(effect, tr("effect is set to NULL"));

    int index = effects.indexOf(const_cast<synthclone::Effect *>(effect));

    CONFIRM(index != -1, tr("effect is not registered with session"));

    return index;
}

const synthclone::EffectJob *
Session::getEffectJob(int index) const
{
    CONFIRM((index >= 0) && (index < effectJobs.count()),
            tr("'%1': index is out of range").arg(index));

    return effectJobs[index];
}

const synthclone::EffectJob *
Session::getEffectJob(const synthclone::Zone *zone) const
{
    synthclone::EffectJob *job = zoneEffectJobMap.value(zone, 0);

    CONFIRM(job, tr("no effect job corresponds to the given zone"));

    return job;
}

int
Session::getEffectJobCount() const
{
    return effectJobs.count();
}

int
Session::getEffectJobIndex(const synthclone::EffectJob *job) const
{
    CONFIRM(job, tr("job is set to NULL"));

    int index = effectJobs.indexOf(const_cast<synthclone::EffectJob *>(job));

    CONFIRM(index != -1, tr("effect job is not in session effect job list"));

    return index;
}

const synthclone::Component *
Session::getFocusedComponent() const
{
    return focusedComponent;
}

int
Session::getMajorVersion() const
{
    return SYNTHCLONE_MAJOR_VERSION;
}

int
Session::getMinorVersion() const
{
    return SYNTHCLONE_MINOR_VERSION;
}

const synthclone::Participant *
Session::getParticipant(const QByteArray &id) const
{
    const synthclone::Participant *participant = participantIdMap.value(id, 0);
    CONFIRM(participant,
            tr("'%1': id does not belong to a registered participant"));
    return participant;
}

const synthclone::Participant *
Session::getParticipant(int index, const synthclone::Participant *parent) const
{
    const ParticipantList *participants;
    if (! parent) {
        participants = &rootParticipants;
    } else {
        ParticipantData *data = participantDataMap.value(parent, 0);

        CONFIRM(data, tr("the given parent is not a registered participant"));

        participants = &(data->children);
    }

    CONFIRM((index >= 0) && (index < participants->count()),
            tr("'%1': index is out of range").arg(index));

    return participants->at(index);
}

int
Session::getParticipantCount(const synthclone::Participant *parent) const
{
    if (! parent) {
        return rootParticipants.count();
    }
    ParticipantData *data = participantDataMap.value(parent, 0);

    CONFIRM(data, tr("the given parent is not a registered participant"));

    return data->children.count();
}

QByteArray
Session::getParticipantId(const synthclone::Participant *participant) const
{
    CONFIRM(participant, tr("participant is set to NULL"));

    ParticipantData *data = participantDataMap.value(participant, 0);

    CONFIRM(data, tr("participant is not registered with session"));

    return data->id;
}

const synthclone::Participant *
Session::getParticipantParent(const synthclone::Participant *participant) const
{
    CONFIRM(participant, tr("participant is set to NULL"));

    ParticipantData *data = participantDataMap.value(participant, 0);

    CONFIRM(data, tr("participant is not registered with session"));

    return data->parent;
}

int
Session::getRevision() const
{
    return SYNTHCLONE_REVISION;
}

synthclone::SampleChannelCount
Session::getSampleChannelCount() const
{
    return sessionSampleData.getSampleChannelCount();
}

const synthclone::Sampler *
Session::getSampler() const
{
    return sampler;
}

synthclone::SampleRate
Session::getSampleRate() const
{
    return sessionSampleData.getSampleRate();
}

const synthclone::SamplerJob *
Session::getSamplerJob(int index) const
{
    CONFIRM((index >= 0) && (index < samplerJobs.count()),
            tr("'%1': index is out of range").arg(index));

    return samplerJobs[index];
}

const synthclone::SamplerJob *
Session::getSamplerJob(const synthclone::Zone *zone) const
{
    synthclone::SamplerJob *job = zoneSamplerJobMap.value(zone, 0);

    CONFIRM(job, tr("no sampler job corresponds to the given zone"));

    return job;
}

int
Session::getSamplerJobCount() const
{
    return samplerJobs.count();
}

int
Session::getSamplerJobIndex(const synthclone::SamplerJob *job) const
{
    CONFIRM(job, tr("job is set to NULL"));

    int index = samplerJobs.indexOf(const_cast<synthclone::SamplerJob *>(job));

    CONFIRM(index != -1, tr("sampler job is not in session sampler job list"));

    return index;
}

QDir
Session::getSamplesDirectory(const QDir &sessionDirectory)
{
    if (! sessionDirectory.exists("samples")) {
        if (! sessionDirectory.mkdir("samples")) {
            throw synthclone::Error(tr("failed to create samples directory"));
        }
    }
    QDir samplesDirectory(sessionDirectory);
    if (! samplesDirectory.cd("samples")) {
        throw synthclone::Error("failed to access session samples directory");
    }
    return samplesDirectory;
}

const synthclone::Effect *
Session::getSelectedEffect() const
{
    return selectedEffect;
}

const synthclone::Target *
Session::getSelectedTarget() const
{
    return selectedTarget;
}

synthclone::Zone *
Session::getSelectedZone(int index)
{
    CONFIRM((index >= 0) && (index < selectedZones.count()),
            tr("'%1': index is out of range").arg(index));

    return selectedZones[index];
}

int
Session::getSelectedZoneCount() const
{
    return selectedZones.count();
}

synthclone::SessionState
Session::getState() const
{
    return state;
}

const synthclone::Target *
Session::getTarget(int index) const
{
    CONFIRM((index >= 0) && (index < targets.count()),
            tr("'%1': index is out of range").arg(index));

    return targets[index];
}

int
Session::getTargetCount() const
{
    return targets.count();
}

int
Session::getTargetIndex(const synthclone::Target *target) const
{
    CONFIRM(target, "target is set to NULL");

    int index = targets.indexOf(const_cast<synthclone::Target *>(target));

    CONFIRM(index != -1, tr("target is not registered with session"));

    return index;
}

synthclone::Zone *
Session::getZone(int index)
{
    CONFIRM((index >= 0) && (index < zones.count()),
            tr("'%1': index is out of range").arg(index));

    return zones[index];
}

int
Session::getZoneCount() const
{
    return zones.count();
}

int
Session::getZoneIndex(const synthclone::Zone *zone) const
{
    CONFIRM(zone, tr("zone is set to NULL"));

    int index = zones.indexOf(const_cast<synthclone::Zone *>(zone));

    CONFIRM((index >= 0) && (index < zones.count()),
            tr("zone is not in session zone list"));

    return index;
}

void
Session::handleEffectJobCompletion()
{
    Zone *zone = qobject_cast<EffectJob *>(currentEffectJob)->getZone();
    zone->setStatus(synthclone::Zone::STATUS_NORMAL);
    zone->setWetSample(currentEffectJobWetSample, false);
    recycleCurrentEffectJob();
}

void
Session::handleEffectJobError(const QString &message)
{
    qobject_cast<EffectJob *>(currentEffectJob)->getZone()->
        setStatus(synthclone::Zone::STATUS_NORMAL);
    reportError(message);
    recycleCurrentEffectJob();
}

void
Session::handleSamplerJobAbort()
{
    Zone *zone = qobject_cast<SamplerJob *>(currentSamplerJob)->getZone();
    recycleCurrentSamplerJob();
    zone->setStatus(synthclone::Zone::STATUS_NORMAL);
}

void
Session::handleSamplerJobCompletion()
{
    Zone *zone = qobject_cast<SamplerJob *>(currentSamplerJob)->getZone();
    if (currentSamplerJob->getType() == synthclone::SamplerJob::TYPE_SAMPLE) {
        currentSamplerJobStream->close();
        if (zones.contains(zone)) {
            zone->setStatus(synthclone::Zone::STATUS_NORMAL);
            zone->setDrySample(currentSamplerJobSample, false);
            currentSamplerJobSample = 0;
        }
        recycleCurrentSamplerJob();
    } else {
        recycleCurrentSamplerJob();
        zone->setStatus(synthclone::Zone::STATUS_NORMAL);
    }
}

void
Session::handleSamplerJobError(const QString &message)
{
    emit samplerJobError(message);
    Zone *zone = qobject_cast<SamplerJob *>(currentSamplerJob)->getZone();
    recycleCurrentSamplerJob();
    zone->setStatus(synthclone::Zone::STATUS_NORMAL);
}

void
Session::handleTargetValidationError()
{
    targetValidationErrorsOccurred = true;
}

void
Session::handleZoneLoad(int current, int total)
{
    QString message = tr("Loading zone %1 of %2 ...").arg(current).arg(total);
    float progress = (static_cast<float>(current) / total) * 0.6;
    emit progressChanged(progress, message);
}

void
Session::insertSelectedZone(synthclone::Zone *zone)
{
    assert(zone);
    int index = getZoneIndex(zone);
    int lastIndex = selectedZones.count() - 1;
    if (lastIndex == -1) {
        selectedZones.append(zone);
    } else if (getZoneIndex(selectedZones[0]) > index) {
        selectedZones.insert(0, zone);
    } else if (getZoneIndex(selectedZones[lastIndex]) < index) {
        selectedZones.append(zone);
    } else {
        int firstIndex = 0;
        while ((lastIndex - firstIndex) > 1) {
            int testIndex = (lastIndex + firstIndex) / 2;
            int realIndex = getZoneIndex(selectedZones[testIndex]);
            assert(realIndex != index);
            if (realIndex > index) {
                lastIndex = testIndex;
            } else {
                firstIndex = testIndex;
            }
        }
        selectedZones.insert(lastIndex, zone);
    }
}

bool
Session::isAftertouchPropertyVisible() const
{
    return aftertouchPropertyVisible;
}

bool
Session::isChannelPropertyVisible() const
{
    return channelPropertyVisible;
}

bool
Session::isControlPropertyVisible(synthclone::MIDIData control) const
{
    CONFIRM(control < 0x80, tr("'%1': control is out of range").arg(control));
    return controlPropertiesVisible[control];
}

bool
Session::isDrySamplePropertyVisible() const
{
    return drySamplePropertyVisible;
}

bool
Session::isNotePropertyVisible() const
{
    return notePropertyVisible;
}

bool
Session::
isParticipantActivated(const synthclone::Participant *participant) const
{
    ParticipantData *data = participantDataMap.value(participant, 0);
    CONFIRM(data, tr("participant is not registered with session"));
    return static_cast<bool>(data->context);
}

bool
Session::isReleaseTimePropertyVisible() const
{
    return releaseTimePropertyVisible;
}

bool
Session::isSampleTimePropertyVisible() const
{
    return sampleTimePropertyVisible;
}

bool
Session::isStatusPropertyVisible() const
{
    return statusPropertyVisible;
}

bool
Session::isVelocityPropertyVisible() const
{
    return velocityPropertyVisible;
}

bool
Session::isWetSamplePropertyVisible() const
{
    return wetSamplePropertyVisible;
}

bool
Session::isZoneSelected(const synthclone::Zone *zone) const
{
    return selectedZones.contains(const_cast<synthclone::Zone *>(zone));
}

bool
Session::isZoneSelected(int index) const
{
    CONFIRM((index >= 0) && (index < zones.count()),
            tr("'%1': index is out of range").arg(index));

    return isZoneSelected(zones[index]);
}

void
Session::load(const QDir &directory)
{

    QDomDocument document;
    if (! loadXML(directory, document)) {
        throw synthclone::Error(tr("'%1' is not a valid session directory").
                                arg(directory.absolutePath()));
    }
    unload();
    state = synthclone::SESSIONSTATE_LOADING;
    emit stateChanged(state, &directory);

    QDomElement documentElement = document.documentElement();
    QString message;

    emit progressChanged(0.0, tr("Reading session ..."));

    quint32 majorVersion;
    quint32 minorVersion;
    quint32 revision;
    if (verifyWholeNumberAttribute(documentElement, "major-version",
                                   majorVersion, 0, 255) &&
        verifyWholeNumberAttribute(documentElement, "minor-version",
                                   minorVersion, 0, 255) &&
        verifyWholeNumberAttribute(documentElement, "revision",
                                   revision, 0, 255)) {
        quint32 currentVersion = (SYNTHCLONE_MAJOR_VERSION << 16) |
            (SYNTHCLONE_MINOR_VERSION << 8) | SYNTHCLONE_REVISION;
        quint32 version = (majorVersion << 16) | (minorVersion << 8) | revision;
        if (currentVersion != version) {
            message = tr("parsing session created by synthclone %1.%2.%3").
                arg(majorVersion).arg(minorVersion).arg(revision);
            emitLoadWarning(documentElement, message);
        }
    }
    quint32 uValue;

    this->directory = new QDir(directory);
    QDir samplesDirectory = getSamplesDirectory(directory);
    sessionSampleData.setSampleDirectory(&samplesDirectory);

    synthclone::SampleChannelCount maxChannels =
        std::numeric_limits<synthclone::SampleChannelCount>::max();
    synthclone::SampleChannelCount sampleChannelCount =
        verifyWholeNumberAttribute(documentElement, "sample-channel-count",
                                   uValue, 1, maxChannels) ?
        static_cast<synthclone::SampleChannelCount>(uValue) : 2;
    sessionSampleData.setSampleChannelCount(sampleChannelCount);

    synthclone::SampleRate sampleRate =
        verifyWholeNumberAttribute(documentElement, "sample-rate", uValue, 1) ?
        static_cast<synthclone::SampleRate>(uValue) : 44100;
    sessionSampleData.setSampleRate(sampleRate);

    // Property visibility flags
    aftertouchPropertyVisible =
        verifyBooleanAttribute(documentElement, "aftertouch-property-visible",
                               false);
    channelPropertyVisible =
        verifyBooleanAttribute(documentElement, "channel-property-visible",
                               true);
    drySamplePropertyVisible =
        verifyBooleanAttribute(documentElement, "dry-sample-property-visible",
                               true);
    notePropertyVisible =
        verifyBooleanAttribute(documentElement, "note-property-visible", true);
    releaseTimePropertyVisible =
        verifyBooleanAttribute(documentElement, "release-time-property-visible",
                               true);
    sampleTimePropertyVisible =
        verifyBooleanAttribute(documentElement, "sample-time-property-visible",
                               true);
    statusPropertyVisible =
        verifyBooleanAttribute(documentElement, "status-property-visible",
                               true);
    velocityPropertyVisible =
        verifyBooleanAttribute(documentElement, "velocity-property-visible",
                               true);
    wetSamplePropertyVisible =
        verifyBooleanAttribute(documentElement, "wet-sample-property-visible",
                               true);

    QString controlPropertyTemplate = "control-property-%1-visible";
    int i;
    for (synthclone::MIDIData i = 0; i < 0x80; i++) {
        controlPropertiesVisible[i] =
            verifyBooleanAttribute(documentElement,
                                   controlPropertyTemplate.arg(i), false);
    }

    int elementCount;
    float progress;

    // Zones
    emit progressChanged(0.0, tr("Loading zones ..."));
    QDomElement element = documentElement.firstChildElement("zones");
    if (element.isNull()) {
        message = tr("root element doesn't contain 'zones' child");
        emitLoadWarning(documentElement, message);
    } else {
        QDir samplesDirectory = getSamplesDirectory(directory);
        ZoneListLoader zoneListLoader(*this);
        connect(&zoneListLoader, SIGNAL(loadingZone(int, int)),
                SLOT(handleZoneLoad(int, int)));
        connect(&zoneListLoader, SIGNAL(warning(int, int, QString)),
                SIGNAL(loadWarning(int, int, QString)));
        zoneListLoader.loadZones(element, -1, &samplesDirectory);
    }

    synthclone::Participant *participant;

    // Participants
    emit progressChanged(0.6, tr("Loading participants ..."));
    element = documentElement.firstChildElement("participants");
    if (element.isNull()) {
        message = tr("root element doesn't contain 'participants' child");
        emitLoadWarning(documentElement, message);
    } else {
        elementCount = element.elementsByTagName("participant").count();
        QList<QString> loadedIds;
        for (i = 0, element = element.firstChildElement("participant");
             ! element.isNull();
             element = element.nextSiblingElement("participant"), i++) {

            message = tr("Loading participant %1 of %2 ...").arg(i + 1).
                arg(elementCount);
            progress = ((static_cast<float>(i) / elementCount) * 0.1) + 0.6;
            emit progressChanged(progress, message);

            QString id = element.attribute("id");
            if (id.isEmpty()) {
                message = tr("'participant' element does not contain 'id' "
                             "attribute");
                emitLoadWarning(element, message);
                continue;
            }
            if (loadedIds.contains(id)) {
                message = tr("duplicate participant id '%1' found").arg(id);
                emitLoadWarning(element, message);
                continue;
            }
            participant = participantIdMap.value(id.toAscii(), 0);
            if (! participant) {
                message = tr("no participant with id '%1'").arg(id);
                emitLoadWarning(element, message);
                continue;
            }
            if (isParticipantActivated(participant)) {
                message = tr("participant with id '%1' requires re-activation").
                    arg(id);
                emitLoadWarning(element, message);
                deactivateParticipant(participant);
            }
            activateParticipant(participant, readXMLState(element));
            loadedIds.append(id);
        }
    }

    // Sampler
    emit progressChanged(0.7, tr("Checking for sampler ..."));
    element = documentElement.firstChildElement("sampler");
    if (element.isNull()) {
        message = tr("root element doesn't contain 'sampler' child");
        emitLoadWarning(documentElement, message);
    } else if (! element.attribute("participant-id").isEmpty()) {

        emit progressChanged(0.7, tr("Loading sampler ..."));

        participant = getActivatedParticipant(element);
        if (participant) {
            participant->restoreSampler(readXMLState(element));
        }
    }

    // Effects
    emit progressChanged(0.8, tr("Loading effects ..."));
    element = documentElement.firstChildElement("effects");
    if (element.isNull()) {
        message = tr("root element doesn't contain 'effects' child");
        emitLoadWarning(documentElement, message);
    } else {
        elementCount = element.elementsByTagName("effect").count();
        for (i = 0, element = element.firstChildElement("effect");
             ! element.isNull();
             element = element.nextSiblingElement("effect"), i++) {

            message = tr("Loading effect %1 of %2 ...").arg(i + 1).
                arg(elementCount);
            progress = ((static_cast<float>(i) / elementCount) * 0.1) + 0.8;
            emit progressChanged(progress, message);

            participant = getActivatedParticipant(element);
            if (participant) {
                participant->restoreEffect(readXMLState(element));
            }
        }
    }

    // Targets
    emit progressChanged(0.9, tr("Loading targets ..."));
    element = documentElement.firstChildElement("targets");
    if (element.isNull()) {
        message = tr("root element doesn't contain 'targets' child");
        emitLoadWarning(documentElement, message);
    } else {
        elementCount = element.elementsByTagName("target").count();
        for (i = 0, element = element.firstChildElement("target");
             ! element.isNull();
             element = element.nextSiblingElement("target"), i++) {

            message = tr("Loading target %1 of %2 ...").arg(i + 1).
                arg(elementCount);
            progress = ((static_cast<float>(i) / elementCount) * 0.1) + 0.9;
            emit progressChanged(progress, message);

            participant = getActivatedParticipant(element);
            if (participant) {
                participant->restoreTarget(readXMLState(element));
            }
        }
    }
    effectJobThread.start();

    // XXX: Start target apply thread or whatever ...

    emit progressChanged(1.0, tr("Loaded."));

    state = synthclone::SESSIONSTATE_CURRENT;
    emit stateChanged(state, this->directory);
}

void
Session::moveEffect(int fromIndex, int toIndex)
{
    CONFIRM((fromIndex >= 0) && (fromIndex < effects.count()),
            tr("'%1': fromIndex is out of range").arg(fromIndex));
    CONFIRM((toIndex >= 0) && (toIndex < effects.count()),
            tr("'%1': toIndex is out of range").arg(toIndex));
    CONFIRM(fromIndex != toIndex, tr("fromIndex is equal to toIndex"));
    CONFIRM(! currentEffectJob, tr("effects are currently in use"));

    emit movingEffect(effects[fromIndex], fromIndex, toIndex);
    effects.move(fromIndex, toIndex);
    emit effectMoved(effects[toIndex], fromIndex, toIndex);
    setModified();
}

void
Session::moveEffectJob(int fromIndex, int toIndex)
{
    CONFIRM((fromIndex >= 0) && (fromIndex < effectJobs.count()),
            tr("'%1': fromIndex is out of range").arg(fromIndex));
    CONFIRM((toIndex >= 0) && (toIndex < effectJobs.count()),
            tr("'%1': toIndex is out of range").arg(toIndex));
    CONFIRM(fromIndex != toIndex, tr("fromIndex is equal to toIndex"));

    emit movingEffectJob(effectJobs[fromIndex], fromIndex, toIndex);
    effectJobs.move(fromIndex, toIndex);
    emit effectJobMoved(effectJobs[toIndex], fromIndex, toIndex);
    setModified();
}

void
Session::moveSamplerJob(int fromIndex, int toIndex)
{
    CONFIRM((fromIndex >= 0) && (fromIndex < samplerJobs.count()),
            tr("'%1': fromIndex is out of range").arg(fromIndex));
    CONFIRM((toIndex >= 0) && (toIndex < samplerJobs.count()),
            tr("'%1': toIndex is out of range").arg(toIndex));
    CONFIRM(fromIndex != toIndex, tr("fromIndex is equal to toIndex"));

    emit movingSamplerJob(samplerJobs[fromIndex], fromIndex, toIndex);
    samplerJobs.move(fromIndex, toIndex);
    emit samplerJobMoved(samplerJobs[toIndex], fromIndex, toIndex);
    setModified();
}

void
Session::moveTarget(int fromIndex, int toIndex)
{
    CONFIRM((fromIndex >= 0) && (fromIndex < targets.count()),
            tr("'%1': fromIndex is out of range").arg(fromIndex));
    CONFIRM((toIndex >= 0) && (toIndex < targets.count()),
            tr("'%1': toIndex is out of range").arg(toIndex));
    CONFIRM(fromIndex != toIndex, tr("fromIndex is equal to toIndex"));

    emit movingTarget(targets[fromIndex], fromIndex, toIndex);
    targets.move(fromIndex, toIndex);
    emit targetMoved(targets[toIndex], fromIndex, toIndex);
    setModified();
}

void
Session::moveZone(int fromIndex, int toIndex)
{
    CONFIRM((fromIndex >= 0) && (fromIndex < zones.count()),
            tr("'%1': fromIndex is out of range").arg(fromIndex));
    CONFIRM((toIndex >= 0) && (toIndex < zones.count()),
            tr("'%1': toIndex is out of range").arg(toIndex));
    CONFIRM(fromIndex != toIndex, tr("fromIndex is equal to toIndex"));

    synthclone::Zone *zone = zones[fromIndex];
    emit movingZone(zone, fromIndex, toIndex);
    zones.move(fromIndex, toIndex);

    // Preserve the sort order of the selected zones list.
    if (selectedZones.removeOne(zone)) {
        insertSelectedZone(zone);
    }

    emit zoneMoved(zone, fromIndex, toIndex);
    setModified();
}

void
Session::quit()
{
    unload();
    application.quit();
}

QVariant
Session::readXMLState(const QDomElement &element)
{
    QDomElement subElement = element.firstChildElement("state");
    if (subElement.isNull()) {
        QString message = tr("'%1' element doesn't contain 'state' child").
            arg(element.tagName());
        emitLoadWarning(element, message);
        return QVariant();
    }
    return readXMLVariant(subElement);
}

QVariant
Session::readXMLVariant(const QDomElement &element)
{
    try {
        return readVariant(element);
    } catch (synthclone::Error &e) {
        emitLoadWarning(element, e.getMessage());
    }
    return QVariant();
}

void
Session::recycleCurrentEffectJob()
{
    bool removed = zoneEffectJobMap.remove(currentEffectJob->getZone());
    assert(removed);
    currentEffectJob->deleteLater();
    currentEffectJob = 0;
    emit currentEffectJobChanged(0);
    updateEffectJobs();
}

void
Session::recycleCurrentSamplerJob()
{
    if (currentSamplerJobStream) {
        currentSamplerJobStream->deleteLater();
        currentSamplerJobStream = 0;
    }
    if (currentSamplerJobSample) {
        QString path = currentSamplerJobSample->getPath();
        currentSamplerJobSample->setTemporary(true);
        currentSamplerJobSample->deleteLater();
        currentSamplerJobSample = 0;
        if (! QFile(path).remove()) {
            emit samplerJobError("failed to remove sample at '%1'");
        }
    }
    bool removed = zoneSamplerJobMap.remove(currentSamplerJob->getZone());
    assert(removed);
    currentSamplerJob->deleteLater();
    currentSamplerJob = 0;
    emit currentSamplerJobChanged(0);
    updateSamplerJobs();
}

void
Session::removeComponentMenuItems(ComponentData *data)
{
    assert(data);
    int i;
    MenuActionList &actions = data->menuActions;
    for (i = actions.count() - 1; i >= 0; i--) {
        removeMenuAction(actions[i]);
    }
    MenuSeparatorList &separators = data->menuSeparators;
    for (i = separators.count() - 1; i >= 0; i--) {
        removeMenuSeparator(separators[i]);
    }
}

void
Session::removeEffect(const synthclone::Effect *effect)
{
    removeEffect(getEffectIndex(effect));
}

void
Session::removeEffect(int index)
{
    CONFIRM((index >= 0) && (index < effects.count()),
            tr("'%1': index is out of range").arg(index));
    CONFIRM(! currentEffectJob, tr("effects are currently in use"));

    synthclone::Effect *effect = effects[index];
    ComponentData *data = effectDataMap.value(effect, 0);
    removeComponentMenuItems(data);
    if (effect == selectedEffect) {
        setSelectedEffect(-1);
    }
    emit removingEffect(effect, index);
    effects.removeAt(index);
    QScopedPointer<Registration> registrationPtr(data->registration);
    delete effectDataMap.take(effect);
    emit effectRemoved(effect, index);
    setModified();
}

void
Session::removeEffectJob(const synthclone::EffectJob *job)
{
    removeEffectJob(getEffectJobIndex(job));
}

void
Session::removeEffectJob(int index)
{
    CONFIRM((index >= 0) && (index < effectJobs.count()),
            tr("'%1': index is out of range").arg(index));

    EffectJob *job = qobject_cast<EffectJob *>(takeEffectJob(index));
    Zone *zone = job->getZone();
    bool removed = zoneEffectJobMap.remove(zone);
    assert(removed);
    zone->setStatus(synthclone::Zone::STATUS_NORMAL);
    job->deleteLater();
    setModified();
}

void
Session::removeMenuAction(const synthclone::MenuAction *action)
{
    CONFIRM(action, tr("action is set to NULL"));

    MenuItemData *data = menuActionDataMap.value(action, 0);

    CONFIRM(data, tr("menu action is not registered with session"));

    QScopedPointer<Registration> registrationPtr(data->registration);
    const synthclone::Effect *effect;
    synthclone::Menu menu;
    bool removed;
    const synthclone::Target *target;
    QStringList &subMenus = data->subMenus;
    switch (data->menuBaseType) {
    case MENUBASETYPE_EFFECT:
        effect = data->menuBase.effect;
        emit removingMenuAction(action, effect, subMenus);
        removed = effectDataMap.value(effect)->menuActions.removeOne(action);
        assert(removed);
        menuActionDataMap.remove(action);
        emit menuActionRemoved(action, effect, subMenus);
        break;
    case MENUBASETYPE_MENU:
        menu = data->menuBase.menu;
        emit removingMenuAction(action, menu, subMenus);
        menuActionDataMap.remove(action);
        emit menuActionRemoved(action, menu, subMenus);
        break;
    case MENUBASETYPE_SAMPLER:
        emit removingMenuAction(action, sampler, subMenus);
        removed = samplerData.menuActions.removeOne(action);
        assert(removed);
        menuActionDataMap.remove(action);
        emit menuActionRemoved(action, sampler, subMenus);
        break;
    case MENUBASETYPE_TARGET:
        target = data->menuBase.target;
        emit removingMenuAction(action, target, subMenus);
        removed = targetDataMap.value(target)->menuActions.removeOne(action);
        assert(removed);
        menuActionDataMap.remove(action);
        emit menuActionRemoved(action, target, subMenus);
    }
    delete data;
    setModified();
}

void
Session::removeMenuSeparator(const synthclone::MenuSeparator *separator)
{
    CONFIRM(separator, tr("separator is set to NULL"));

    MenuItemData *data = menuSeparatorDataMap.value(separator, 0);

    CONFIRM(data, tr("menu separator is not registered with session"));

    QScopedPointer<Registration> registrationPtr(data->registration);
    const synthclone::Effect *effect;
    synthclone::Menu menu;
    bool removed;
    const synthclone::Target *target;
    QStringList &subMenus = data->subMenus;
    switch (data->menuBaseType) {
    case MENUBASETYPE_EFFECT:
        effect = data->menuBase.effect;
        emit removingMenuSeparator(separator, effect, subMenus);
        removed = effectDataMap.value(effect)->
            menuSeparators.removeOne(separator);
        assert(removed);
        menuSeparatorDataMap.remove(separator);
        emit menuSeparatorRemoved(separator, effect, subMenus);
        break;
    case MENUBASETYPE_MENU:
        menu = data->menuBase.menu;
        emit removingMenuSeparator(separator, menu, subMenus);
        menuSeparatorDataMap.remove(separator);
        emit menuSeparatorRemoved(separator, menu, subMenus);
        break;
    case MENUBASETYPE_SAMPLER:
        emit removingMenuSeparator(separator, sampler, subMenus);
        removed = samplerData.menuSeparators.removeOne(separator);
        assert(removed);
        menuSeparatorDataMap.remove(separator);
        emit menuSeparatorRemoved(separator, sampler, subMenus);
        break;
    case MENUBASETYPE_TARGET:
        target = data->menuBase.target;
        emit removingMenuSeparator(separator, target, subMenus);
        removed = targetDataMap.value(target)->
            menuSeparators.removeOne(separator);
        assert(removed);
        menuSeparatorDataMap.remove(separator);
        emit menuSeparatorRemoved(separator, target, subMenus);
    }
    delete data;
    setModified();
}

void
Session::removeParticipant(const synthclone::Participant *participant)
{
    CONFIRM(participant, tr("participant is set to NULL"));

    ParticipantData *data = participantDataMap.value(participant, 0);

    CONFIRM(data, tr("participant is not registered with session"));

    if (data->context) {
        deactivateParticipant(participant);
    }
    QByteArray &id = data->id;
    const synthclone::Participant *parent = data->parent;
    emit removingParticipant(participant, parent, id);
    assert(participantDataMap.contains(participant));
    assert(participantIdMap.contains(id));
    participantDataMap.remove(participant);
    participantIdMap.remove(id);
    bool removed;
    if (parent) {
        ParticipantData *parentData = participantDataMap.value(parent, 0);
        assert(parentData);
        removed = parentData->children.removeOne(data->participant);
    } else {
        removed = rootParticipants.removeOne(data->participant);
    }
    assert(removed);
    QScopedPointer<Registration> registrationPtr(data->registration);
    delete data;
    emit participantRemoved(participant, parent, id);
    setModified();
}

void
Session::removeSampler()
{
    CONFIRM(sampler, tr("sampler is not registered with session"));
    removeComponentMenuItems(&samplerData);

    emit removingSampler(sampler);

    disconnect(sampler, SIGNAL(jobAborted()),
               this, SLOT(handleSamplerJobAbort()));
    disconnect(sampler, SIGNAL(jobCompleted()),
               this, SLOT(handleSamplerJobCompletion()));
    disconnect(sampler, SIGNAL(jobError(const QString &)),
               this, SLOT(handleSamplerJobError(const QString &)));

    samplerData.participant = 0;
    QScopedPointer<Registration> registrationPtr(samplerData.registration);
    const synthclone::Sampler *oldSampler = sampler;
    sampler = 0;
    emit samplerRemoved(oldSampler);
    setModified();
}

void
Session::removeSamplerJob(const synthclone::SamplerJob *job)
{
    removeSamplerJob(getSamplerJobIndex(job));
}

void
Session::removeSamplerJob(int index)
{
    CONFIRM((index >= 0) && (index < samplerJobs.count()),
            tr("'%1': index is out of range").arg(index));

    SamplerJob *job = qobject_cast<SamplerJob *>(takeSamplerJob(index));
    Zone *zone = job->getZone();
    bool removed = zoneSamplerJobMap.remove(zone);
    assert(removed);
    zone->setStatus(synthclone::Zone::STATUS_NORMAL);
    job->deleteLater();
    setModified();
}

void
Session::removeTarget(const synthclone::Target *target)
{
    removeTarget(getTargetIndex(target));
}

void
Session::removeTarget(int index)
{
    CONFIRM((index >= 0) && (index < targets.count()),
            tr("'%1': index is out of range").arg(index));

    synthclone::Target *target = targets[index];
    ComponentData *data = targetDataMap.value(target, 0);
    removeComponentMenuItems(data);
    if (target == selectedTarget) {
        setSelectedTarget(-1);
    }
    emit removingTarget(target, index);
    targets.removeAt(index);
    QScopedPointer<Registration> registrationPtr(data->registration);
    delete targetDataMap.take(target);
    emit targetRemoved(target, index);
    setModified();
}

void
Session::removeZone(synthclone::Zone *zone)
{
    removeZone(getZoneIndex(zone));
}

void
Session::removeZone(int index)
{
    CONFIRM((index >= 0) && (index < zones.count()),
            tr("'%1': index is out of range").arg(index));

    synthclone::Zone *zone = zones[index];

    CONFIRM(zone->getStatus() == synthclone::Zone::STATUS_NORMAL,
            tr("zone is being used by a component"));

    setZoneSelected(index, false);
    emit removingZone(zone, index);
    zones.removeAt(index);
    emit zoneRemoved(zone, index);
    delete qobject_cast<Zone *>(zone);
    setModified();
}

void
Session::reportError(const QString &message)
{
    emit errorReported(message);
}

void
Session::runEffectJobs()
{
    for (;;) {
        effectJobSemaphore.acquire();
        if (! currentEffectJob) {
            // Session is being unloaded.
            break;
        }
        int count = effects.count();
        QString path;
        Zone *zone = qobject_cast<EffectJob *>(currentEffectJob)->getZone();
        const synthclone::Sample *drySample = zone->getDrySample();
        assert(drySample);
        QScopedPointer<synthclone::Sample> wetSamplePtr;
        try {
            if (! count) {
                // Simple case - just copy the file.
                path = createUniqueSampleFile(*directory);
                currentEffectJobWetSample =
                    new synthclone::Sample(*drySample, path);
                wetSamplePtr.reset(currentEffectJobWetSample);
            } else if (count == 1) {
                // Simple case - one input stream and one output stream.
                path = createUniqueSampleFile(*directory);
                currentEffectJobWetSample = new synthclone::Sample(path);
                wetSamplePtr.reset(currentEffectJobWetSample);
                synthclone::SampleInputStream inputStream(*drySample);
                synthclone::SampleOutputStream
                    outputStream(*currentEffectJobWetSample,
                                 inputStream.getSampleRate(),
                                 inputStream.getChannels());
                effects[0]->process(*zone, inputStream, outputStream);
            } else {
                // Complex case - 2 or more effects.
                synthclone::Sample *tempWetSample = new synthclone::Sample();
                wetSamplePtr.reset(tempWetSample);
                synthclone::SampleInputStream firstInputStream(*drySample);
                synthclone::SampleChannelCount channelCount =
                    firstInputStream.getChannels();
                synthclone::SampleRate sampleRate =
                    firstInputStream.getSampleRate();
                synthclone::SampleOutputStream
                    firstOutputStream(*tempWetSample, sampleRate, channelCount);
                effects[0]->process(*zone, firstInputStream, firstOutputStream);
                firstInputStream.close();
                firstOutputStream.close();
                synthclone::Sample *tempDrySample = tempWetSample;
                QScopedPointer<synthclone::Sample>
                    tempDrySamplePtr(tempDrySample);
                wetSamplePtr.take();
                for (int i = 1; i < (count - 1); i++) {
                    tempWetSample = new synthclone::Sample();
                    wetSamplePtr.reset(tempWetSample);
                    synthclone::SampleInputStream
                        tempInputStream(*tempDrySample);
                    synthclone::SampleOutputStream
                        tempOutputStream(*tempWetSample, sampleRate,
                                         channelCount);
                    effects[i]->process(*zone, tempInputStream,
                                        tempOutputStream);
                    tempDrySample = tempWetSample;
                    tempDrySamplePtr.reset(tempDrySample);
                    wetSamplePtr.take();
                }
                path = createUniqueSampleFile(*directory);
                currentEffectJobWetSample = new synthclone::Sample(path);
                wetSamplePtr.reset(currentEffectJobWetSample);
                synthclone::SampleInputStream inputStream(*tempDrySample);
                synthclone::SampleOutputStream
                    outputStream(*currentEffectJobWetSample, sampleRate,
                                 channelCount);
                effects[count - 1]->process(*zone, inputStream, outputStream);
            }
        } catch (synthclone::Error &e) {
            emit effectJobError(e.getMessage());
            continue;
        }
        wetSamplePtr.take();
        emit effectJobCompletion();
    }
}

void
Session::save()
{
    CONFIRM(directory, tr("no session currently loaded"));
    QDir directoryCopy = QDir(*directory);
    save(directoryCopy);
}

void
Session::save(const QDir &directory)
{
    initializeDirectory(directory);
    state = synthclone::SESSIONSTATE_SAVING;
    emit stateChanged(state, &directory);

    QFile file;
    QXmlStreamWriter writer;
    initializeWriter(writer, file, directory);

    QString message;
    float progress;
    emit progressChanged(0.0, tr("Saving session ..."));

    if (this->directory) {
        delete this->directory;
    }
    this->directory = new QDir(directory);
    QDir samplesDirectory = getSamplesDirectory(directory);
    sessionSampleData.setSampleDirectory(&samplesDirectory);

    writer.writeStartDocument();
    writer.writeStartElement("synthclone-session");
    writer.writeAttribute("major-version",
                          QString::number(SYNTHCLONE_MAJOR_VERSION));
    writer.writeAttribute("minor-version",
                          QString::number(SYNTHCLONE_MINOR_VERSION));
    writer.writeAttribute("revision", QString::number(SYNTHCLONE_REVISION));
    writer.writeAttribute
        ("sample-channel-count",
         QString::number(sessionSampleData.getSampleChannelCount()));
    writer.writeAttribute
        ("sample-rate", QString::number(sessionSampleData.getSampleRate()));

    // Property visibility flags
    writer.writeAttribute("aftertouch-property-visible",
                          aftertouchPropertyVisible ? "true" : "false");
    writer.writeAttribute("channel-property-visible",
                          channelPropertyVisible ? "true" : "false");
    writer.writeAttribute("dry-sample-property-visible",
                          drySamplePropertyVisible ? "true" : "false");
    writer.writeAttribute("note-property-visible",
                          notePropertyVisible ? "true" : "false");
    writer.writeAttribute("release-time-property-visible",
                          releaseTimePropertyVisible ? "true" : "false");
    writer.writeAttribute("sample-time-property-visible",
                          sampleTimePropertyVisible ? "true" : "false");
    writer.writeAttribute("status-property-visible",
                          statusPropertyVisible ? "true" : "false");
    writer.writeAttribute("velocity-property-visible",
                          velocityPropertyVisible ? "true" : "false");
    writer.writeAttribute("wet-sample-property-visible",
                          wetSamplePropertyVisible ? "true" : "false");
    QString controlPropertyTemplate = "control-property-%1-visible";
    for (synthclone::MIDIData i = 0; i < 0x80; i++) {
        writer.writeAttribute(controlPropertyTemplate.arg(i),
                              controlPropertiesVisible[i] ? "true" : "false");
    }

    // Zones
    emit progressChanged(0.0, tr("Saving zones ..."));
    writer.writeStartElement("zones");
    int count = zones.count();
    int i;
    for (i = 0; i < count; i++) {
        message = tr("Saving zone %1 of %2 ...").arg(i + 1).arg(count);
        progress = (static_cast<float>(i) / count) * 0.6;
        emit progressChanged(progress, message);

        Zone *zone = qobject_cast<Zone *>(zones[i]);
        writeZone(writer, zone, &samplesDirectory, this);
    }
    writer.writeEndElement();

    // Participants
    emit progressChanged(0.6, "Saving participants ...");
    writer.writeStartElement("participants");
    writeXMLParticipantList(writer, rootParticipants);
    writer.writeEndElement();

    const synthclone::Participant *participant;

    // Sampler
    writer.writeStartElement("sampler");
    if (sampler) {
        emit progressChanged(0.7, "Saving sampler ...");
        participant = writeXMLParticipantId(writer, samplerData.participant);
        writeXMLState(writer, participant->getState(sampler));
    }
    writer.writeEndElement();

    // Effects
    emit progressChanged(0.8, tr("Saving effects ..."));
    writer.writeStartElement("effects");
    count = effects.count();
    for (i = 0; i < count; i++) {

        message = tr("Saving effect %1 of %2 ...").arg(i + 1).arg(count);
        progress = ((static_cast<float>(i) / count) * 0.1) + 0.8;
        emit progressChanged(progress, message);

        const synthclone::Effect *effect = effects[i];
        writer.writeStartElement("effect");
        participant =
            writeXMLParticipantId(writer,
                                  effectDataMap.value(effect)->participant);
        writeXMLState(writer, participant->getState(effect));
        writer.writeEndElement();
    }
    writer.writeEndElement();

    // Targets
    emit progressChanged(0.9, tr("Saving targets ..."));
    writer.writeStartElement("targets");
    count = targets.count();
    for (i = 0; i < count; i++) {

        message = tr("Saving target %1 of %2 ...").arg(i + 1).arg(count);
        progress = ((static_cast<float>(i) / count) * 0.1) + 0.9;
        emit progressChanged(progress, message);

        const synthclone::Target *target = targets[i];
        writer.writeStartElement("target");
        participant =
            writeXMLParticipantId(writer,
                                  targetDataMap.value(target)->participant);
        writeXMLState(writer, participant->getState(target));
        writer.writeEndElement();
    }
    writer.writeEndElement();

    // End 'synthclone-session'
    writer.writeEndElement();

    writer.writeEndDocument();

    file.close();

    emit progressChanged(1.0, tr("Session saved."));

    state = synthclone::SESSIONSTATE_CURRENT;
    emit stateChanged(state, this->directory);
}

void
Session::setSelectedEffect(const synthclone::Effect *effect)
{
    setSelectedEffect(effect ? getEffectIndex(effect) : -1);
}

void
Session::setSelectedEffect(int index)
{
    CONFIRM((index >= -1) && (index < effects.count()),
            tr("'%1': index is out of range").arg(index));

    synthclone::Effect *effect = (index == -1) ? 0 : effects[index];
    if (effect != selectedEffect) {
        selectedEffect = effect;
        emit selectedEffectChanged(selectedEffect, index);
        setModified();
    }
}

void
Session::setSelectedTarget(const synthclone::Target *target)
{
    setSelectedTarget(target ? getTargetIndex(target) : -1);
}

void
Session::setSelectedTarget(int index)
{
    CONFIRM((index >= -1) && (index < targets.count()),
            tr("'%1': index is out of range").arg(index));

    synthclone::Target *target = (index == -1) ? 0 : targets[index];
    if (target != selectedTarget) {
        selectedTarget = target;
        emit selectedTargetChanged(selectedTarget, index);
        setModified();
    }
}

void
Session::setAftertouchPropertyVisible(bool visible)
{
    if (aftertouchPropertyVisible != visible) {
        aftertouchPropertyVisible = visible;
        emit aftertouchPropertyVisibilityChanged(visible);
    }
}

void
Session::setChannelPropertyVisible(bool visible)
{
    if (channelPropertyVisible != visible) {
        channelPropertyVisible = visible;
        emit channelPropertyVisibilityChanged(visible);
    }
}

void
Session::setControlPropertyVisible(synthclone::MIDIData control, bool visible)
{
    CONFIRM(control < 0x80, tr("'%1': control is out of range").arg(control));
    if (controlPropertiesVisible[control] != visible) {
        controlPropertiesVisible[control] = visible;
        emit controlPropertyVisibilityChanged(control, visible);
    }
}

void
Session::setDrySamplePropertyVisible(bool visible)
{
    if (drySamplePropertyVisible != visible) {
        drySamplePropertyVisible = visible;
        emit drySamplePropertyVisibilityChanged(visible);
    }
}

void
Session::setFocusedComponent(const synthclone::Component *component)
{
    if (component != focusedComponent) {
        if (component) {
            const synthclone::Effect *effect =
                qobject_cast<const synthclone::Effect *>(component);
            if (effect) {
                CONFIRM(effectDataMap.contains(effect),
                        tr("effect is not registered with session"));
                setSelectedEffect(effect);
            } else {
                const synthclone::Sampler *sampler =
                    qobject_cast<const synthclone::Sampler *>(component);
                if (sampler) {
                    CONFIRM(sampler == this->sampler,
                            tr("sampler is not registered with session"));
                } else {
                    const synthclone::Target *target =
                        qobject_cast<const synthclone::Target *>(component);
                    CONFIRM(target, tr("component type is invalid"));
                    CONFIRM(targetDataMap.contains(target),
                            tr("target is not registered with session"));
                    setSelectedTarget(target);
                }
            }
        }
        focusedComponent = component;
        emit focusedComponentChanged(component);
    }
}

void
Session::setModified()
{
    if (state == synthclone::SESSIONSTATE_CURRENT) {
        state = synthclone::SESSIONSTATE_MODIFIED;
        emit stateChanged(state, directory);
    }
}

void
Session::setNotePropertyVisible(bool visible)
{
    if (notePropertyVisible != visible) {
        notePropertyVisible = visible;
        emit notePropertyVisibilityChanged(visible);
    }
}

void
Session::setReleaseTimePropertyVisible(bool visible)
{
    if (releaseTimePropertyVisible != visible) {
        releaseTimePropertyVisible = visible;
        emit releaseTimePropertyVisibilityChanged(visible);
    }
}

void
Session::setSampleChannelCount(synthclone::SampleChannelCount count)
{
    sessionSampleData.setSampleChannelCount(count);
}

void
Session::setSampleRate(synthclone::SampleRate sampleRate)
{
    sessionSampleData.setSampleRate(sampleRate);
}

void
Session::setSampleTimePropertyVisible(bool visible)
{
    if (sampleTimePropertyVisible != visible) {
        sampleTimePropertyVisible = visible;
        emit sampleTimePropertyVisibilityChanged(visible);
    }
}

void
Session::setStatusPropertyVisible(bool visible)
{
    if (statusPropertyVisible != visible) {
        statusPropertyVisible = visible;
        emit statusPropertyVisibilityChanged(visible);
    }
}

void
Session::setVelocityPropertyVisible(bool visible)
{
    if (velocityPropertyVisible != visible) {
        velocityPropertyVisible = visible;
        emit velocityPropertyVisibilityChanged(visible);
    }
}

void
Session::setWetSamplePropertyVisible(bool visible)
{
    if (wetSamplePropertyVisible != visible) {
        wetSamplePropertyVisible = visible;
        emit wetSamplePropertyVisibilityChanged(visible);
    }
}

void
Session::setZoneSelected(const synthclone::Zone *zone, bool selected)
{
    setZoneSelected(getZoneIndex(zone), selected);
}

void
Session::setZoneSelected(int index, bool selected)
{
    CONFIRM((index >= 0) && (index < zones.count()),
            tr("'%1': index is out of range").arg(index));

    synthclone::Zone *zone = zones[index];
    if (selectedZones.contains(zone) != selected) {
        if (selected) {
            insertSelectedZone(zone);
        } else {
            bool removed = selectedZones.removeOne(zone);
            assert(removed);
        }
        emit zoneSelectionChanged(zone, index, selected);
        setModified();
    }
}

void
Session::sortZones(const synthclone::ZoneComparer &comparer, bool ascending,
                   int leftIndex, int rightIndex)
{

    // The algorithm here is based on the in-place quick sort algorithm found at
    // http://en.wikipedia.org/wiki/Quicksort.  We implement the algorithm
    // instead of using qStableSort because we want to keep track of each zone
    // move so that any objects listening for move signals can be updated.  This
    // makes any sort operation a bit slower than it could be, but allows
    // objects observing zone order to be notified when zones are moved.

    if (leftIndex < rightIndex) {
        int pivotIndex = (leftIndex + rightIndex) / 2;
        synthclone::Zone *pivotZone = zones[pivotIndex];
        swapZones(pivotIndex, rightIndex);
        int storeIndex = leftIndex;

        synthclone::Zone *checkZone;
        int i;
        if (ascending) {
            for (i = leftIndex; i < rightIndex; i++) {
                checkZone = zones[i];
                if (comparer.isLessThan(checkZone, pivotZone)) {
                    if (i != storeIndex) {
                        swapZones(storeIndex, i);
                    }
                    storeIndex++;
                }
            }
        } else {
            for (i = leftIndex; i < rightIndex; i++) {
                checkZone = zones[i];
                if (comparer.isLessThan(pivotZone, checkZone)) {
                    if (i != storeIndex) {
                        swapZones(storeIndex, i);
                    }
                    storeIndex++;
                }
            }
        }

        if (storeIndex != rightIndex) {
            swapZones(storeIndex, rightIndex);
        }
        sortZones(comparer, ascending, leftIndex, storeIndex - 1);
        sortZones(comparer, ascending, storeIndex + 1, rightIndex);
    }
}

void
Session::sortZones(const synthclone::ZoneComparer &comparer, bool ascending)
{
    sortZones(comparer, ascending, 0, zones.count() - 1);
    qStableSort(selectedZones.begin(), selectedZones.end(),
                ZoneComparerProxy(zoneIndexComparer));
}

void
Session::swapZones(int firstIndex, int secondIndex)
{
    assert((firstIndex >= 0) && (firstIndex < zones.count()));
    assert((secondIndex >= 0) && (secondIndex < zones.count()));
    assert(firstIndex != secondIndex);
    int highIndex;
    int lowIndex;
    if (firstIndex > secondIndex) {
        highIndex = firstIndex;
        lowIndex = secondIndex;
    } else {
        highIndex = secondIndex;
        lowIndex = firstIndex;
    }
    moveZone(lowIndex, highIndex);
    if ((highIndex - 1) != lowIndex) {
        moveZone(highIndex - 1, lowIndex);
    }
}

synthclone::EffectJob *
Session::takeEffectJob(int index)
{
    assert((index >= 0) && (index < effectJobs.count()));
    synthclone::EffectJob *job = effectJobs[index];
    emit removingEffectJob(job, index);
    effectJobs.removeAt(index);
    emit effectJobRemoved(job, index);
    return job;
}

synthclone::SamplerJob *
Session::takeSamplerJob(int index)
{
    assert((index >= 0) && (index < samplerJobs.count()));
    synthclone::SamplerJob *job = samplerJobs[index];
    emit removingSamplerJob(job, index);
    samplerJobs.removeAt(index);
    emit samplerJobRemoved(job, index);
    return job;
}

void
Session::unload()
{
    if (directory) {
        state = synthclone::SESSIONSTATE_UNLOADING;
        emit stateChanged(state, directory);

        // Let the effect job thread know that it needs to terminate.
        effectJobSemaphore.release();

        // XXX: Do the same with target structures.

        int i;

        // Take care of sampler jobs before removing components.
        for (i = samplerJobs.count() - 1; i >= 0; i--) {
            removeSamplerJob(i);
        }
        if (currentSamplerJob) {
            if (sampler) {
                sampler->abortJob();
                removeSampler();
            } else {
                currentSamplerJob->deleteLater();
                currentSamplerJob = 0;
                if (currentSamplerJobStream) {
                    currentSamplerJobStream->deleteLater();
                    currentSamplerJobStream = 0;
                }
                if (currentSamplerJobSample) {
                    currentSamplerJobSample->deleteLater();
                    currentSamplerJobSample = 0;
                }
            }
        }
        for (i = effectJobs.count() - 1; i >= 0; i--) {
            removeEffectJob(i);
        }

        // We need to make sure the effect job thread is terminated before
        // removing components, as contexts associated with participants that
        // are deactivated will want to remove the effects they've registered.
        effectJobThread.wait();

        // Removing activated root participants should remove all components,
        // menu items, child participants, etc.
        for (int i = rootParticipants.count() - 1; i >= 0; i--) {
            synthclone::Participant *participant = rootParticipants[i];
            if (isParticipantActivated(participant)) {
                deactivateParticipant(participant);
            }
        }
        for (int i = zones.count() - 1; i >= 0; i--) {
            removeZone(i);
        }

        sessionSampleData.setSampleDirectory(0);
        delete directory;
        directory = 0;

        assert(! currentEffectJob);
        assert(! currentSamplerJob);
        assert(! currentSamplerJobSample);
        assert(! currentSamplerJobStream);
        assert(! effects.count());
        assert(! effectDataMap.count());
        assert(! effectJobs.count());
        assert(! menuActionDataMap.count());
        assert(! menuSeparatorDataMap.count());
        assert(! sampler);
        assert(! samplerData.menuActions.count());
        assert(! samplerData.menuSeparators.count());
        assert(! samplerJobs.count());
        assert(! selectedEffect);
        assert(! selectedTarget);
        assert(! selectedZones.count());
        assert(! targets.count());
        assert(! targetDataMap.count());
        assert(! zones.count());
        assert(! zoneEffectJobMap.count());
        assert(! zoneSamplerJobMap.count());

        state = synthclone::SESSIONSTATE_CURRENT;
        emit stateChanged(state, 0);
    }
}

void
Session::updateEffectJobs()
{
    if ((! currentEffectJob) && (effectJobs.count())) {
        EffectJob *job = qobject_cast<EffectJob *>(takeEffectJob(0));
        currentEffectJob = job;
        emit currentEffectJobChanged(job);
        job->getZone()->setStatus(synthclone::Zone::STATUS_EFFECTS);
        effectJobSemaphore.release();
    }
}

void
Session::updateSamplerJobs()
{
    if (sampler && (! currentSamplerJob)) {
        while (samplerJobs.count()) {
            SamplerJob *job = qobject_cast<SamplerJob *>(takeSamplerJob(0));
            QScopedPointer<SamplerJob> jobPtr(job);
            synthclone::Sample *sample;
            synthclone::Zone::Status status;
            synthclone::SampleStream *stream;
            synthclone::SamplerJob::Type type = job->getType();
            Zone *zone = job->getZone();
            try {
                switch (type) {
                case synthclone::SamplerJob::TYPE_PLAY_DRY_SAMPLE:
                    sample = 0;
                    status =
                        synthclone::Zone::STATUS_SAMPLER_PLAYING_DRY_SAMPLE;
                    stream = new synthclone::SampleInputStream
                        (*(zone->getDrySample()), this);
                    break;
                case synthclone::SamplerJob::TYPE_PLAY_WET_SAMPLE:
                    sample = 0;
                    status =
                        synthclone::Zone::STATUS_SAMPLER_PLAYING_WET_SAMPLE;
                    stream = new synthclone::SampleInputStream
                        (*(zone->getWetSample()), this);
                    break;
                case synthclone::SamplerJob::TYPE_SAMPLE:
                    {
                        QString path = createUniqueSampleFile(*directory);
                        sample = new synthclone::Sample(path, this);
                        QScopedPointer<synthclone::Sample> samplePtr(sample);
                        status =
                            synthclone::Zone::STATUS_SAMPLER_SAMPLING;
                        stream = new synthclone::SampleOutputStream
                            (*sample, sessionSampleData.getSampleRate(),
                             sessionSampleData.getSampleChannelCount());
                        samplePtr.take();
                    }
                    break;
                default:
                    assert(false);
                }
                jobPtr.take();
            } catch (synthclone::Error &e) {
                emit samplerJobError(e.getMessage());
                continue;
            }
            currentSamplerJob = job;
            currentSamplerJobSample = sample;
            currentSamplerJobStream = stream;
            emit currentSamplerJobChanged(job);
            zone->setStatus(status);
            sampler->startJob(*job, *stream);
            break;
        }
    }
}

bool
Session::verifyBooleanAttribute(const QDomElement &element, const QString &name,
                                bool defaultValue)
{
    try {
        return getBooleanAttribute(element, name);
    } catch (synthclone::Error &e) {
        emitLoadWarning(element, e.getMessage());
    }
    return defaultValue;
}

bool
Session::verifySubId(const QByteArray &subId)
{
    if (subId.isEmpty()) {
        return false;
    }
    char c = subId[0];
    if (! (std::islower(c) || std::isdigit(c))) {
        return false;
    }
    int lastIndex = subId.count() - 1;
    for (int i = 1; i < lastIndex; i++) {
        c = subId[i];
        if (! (std::islower(c) || std::isdigit(c) || (c == '-'))) {
            return false;
        }
    }
    c = subId[lastIndex];
    return std::islower(c) || std::isdigit(c);
}

bool
Session::verifyWholeNumberAttribute(const QDomElement &element,
                                    const QString &name, quint32 &value,
                                    quint32 minimumValue, quint32 maximumValue)
{
    try {
        value = getWholeNumberAttribute(element, name, minimumValue,
                                        maximumValue);
    } catch (synthclone::Error &e) {
        emitLoadWarning(element, e.getMessage());
        return false;
    }
    return true;
}

const synthclone::Participant *
Session::writeXMLParticipantId(QXmlStreamWriter &writer,
                               const synthclone::Participant *participant)
{
    assert(participant);
    ParticipantData *data = participantDataMap.value(participant, 0);
    assert(data);
    writer.writeAttribute("participant-id", data->id);
    return participant;
}

void
Session::writeXMLParticipantList(QXmlStreamWriter &writer,
                                 const ParticipantList &participants)
{
    int count = participants.count();
    for (int i = 0; i < count; i++) {
        const synthclone::Participant *participant = participants[i];
        ParticipantData *data = participantDataMap.value(participant, 0);
        assert(data);
        if (! data->context) {
            continue;
        }
        writer.writeStartElement("participant");
        writer.writeAttribute("id", data->id);
        writeXMLState(writer, participant->getState());
        writer.writeEndElement();
        writeXMLParticipantList(writer, data->children);
    }
}

void
Session::writeXMLState(QXmlStreamWriter &writer, const QVariant &value)
{
    writer.writeStartElement("state");
    writeVariant(writer, value);
    writer.writeEndElement();
}
