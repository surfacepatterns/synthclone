/*
 * libsynthclone_lv2 - LV2 effect plugin for `synthclone` Copyright
 * (C) 2012-2013 Devin Anderson
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

#include <QtCore/QtAlgorithms>
#include <QtCore/QDebug>

#include "menuactiondatacomparer.h"
#include "participant.h"

Participant::Participant(QObject *parent):
    synthclone::Participant(tr("LV2"), 0, 0, 1, "Devin Anderson",
                            tr("Loads LV2 plugins as effect components"),
                            parent)
{
    connect(&effectView, SIGNAL(closeRequest()),
            SLOT(handleEffectViewCloseRequest()));
    configuredEffect = 0;
    context = 0;
    world = 0;
}

Participant::~Participant()
{
    // Empty
}

void
Participant::activate(synthclone::Context &context, const QVariant &/*state*/)
{
    world = new LV2World(this);
    this->context = &context;
    addPluginActions();
    effectView.setSampleChannelCount(context.getSampleChannelCount());
    connect(&context,
            SIGNAL(sampleChannelCountChanged(synthclone::SampleChannelCount)),
            &effectView,
            SLOT(setSampleChannelCount(synthclone::SampleChannelCount)));
    configuredEffect = 0;
}

Effect *
Participant::addEffect(const LV2Plugin *plugin)
{
    Effect *effect = new Effect(*plugin, *world, context->getSampleRate(),
                                context->getSampleChannelCount(), this);
    connect(context,
            SIGNAL(sampleChannelCountChanged(synthclone::SampleChannelCount)),
            effect, SLOT(setChannelCount(synthclone::SampleChannelCount)));
    connect(context, SIGNAL(sampleRateChanged(synthclone::SampleRate)),
            effect, SLOT(setSampleRate(synthclone::SampleRate)));

    synthclone::MenuAction *action =
        new synthclone::MenuAction(tr("Configure"), effect);
    connect(action, SIGNAL(triggered()), SLOT(handleEffectConfiguration()));

    const synthclone::Registration &effectRegistration =
        context->addEffect(effect);
    connect(&effectRegistration, SIGNAL(unregistered(QObject *)),
            SLOT(handleEffectUnregistration(QObject *)));

    const synthclone::Registration &actionRegistration =
        context->addMenuAction(action, effect);
    connect(&actionRegistration, SIGNAL(unregistered(QObject *)),
            SLOT(handleEffectActionUnregistration(QObject *)));

    registeredEffects.append(effect);
    return effect;
}

void
Participant::addPluginActions()
{
    QList<MenuActionData *> menuActionDataList;
    int pluginCount = world->getPluginCount();
    QList<MenuActionData *> uncategorizedActionDataList;
    for (int i = 0; i < pluginCount; i++) {
        const LV2Plugin &plugin = world->getPlugin(i);
        QString name = plugin.getName();
        QString s;

        // Make sure the plugin doesn't require any features that we don't
        // provide.
        int requiredFeatureCount = plugin.getRequiredFeatureCount();
        if (requiredFeatureCount) {
            s = tr("Plugin '%1' doesn't support required features:").
                arg(name);
            qWarning() << s;
            for (int j = 0; j < requiredFeatureCount; j++) {
                qWarning() << "\t" << plugin.getRequiredFeature(j);
            }
            continue;
        }

        // Check if the plugin has a UI that can be rendered by our view.
        QString bestTypeURI;
        const LV2UIData *bestUIData = 0;
        unsigned int quality = 0;
        for (int j = plugin.getUIDataCount() - 1; j >= 0; j--) {
            const LV2UIData &uiData = plugin.getUIData(j);
            for (int k = uiData.getTypeURICount() - 1; k >= 0; k--) {
                QString typeURI = uiData.getTypeURI(k);
                unsigned int uiQuality = effectView.getSupportQuality(typeURI);
                if (! uiQuality) {
                    continue;
                }
                if ((! quality) || (quality > uiQuality)) {
                    quality = uiQuality;
                    bestTypeURI = typeURI;
                    bestUIData = &uiData;
                }
            }
        }

        // Sigh.  Declarations need to be here because of the goto inside the
        // upcoming for loop.
        synthclone::MenuAction *action;
        MenuActionData *actionData;
        int pluginClassCount;
        EffectViewData *pluginUIData;
        QStringList subMenus;

        // Make sure audio can be passed to the plugin and retrieved from the
        // plugin in a way that we can process.
        int audioInputPorts = 0;
        int audioOutputPorts = 0;
        for (int j = plugin.getPortCount() - 1; j >= 0; j--) {
            const LV2Port &port = plugin.getPort(j);
            if (port.isAudioPort()) {
                if (port.isInputPort()) {
                    audioInputPorts++;
                } else if (port.isOutputPort()) {
                    audioOutputPorts++;
                } else {
                    s = tr("Plugin '%1' has an audio port that isn't an input "
                           "port or an output port").arg(name);
                    qWarning() << s;
                    goto nextPlugin;
                }
            } else if (! (port.isControlPort() ||
                          port.isConnectionOptional())) {
                s = tr("Plugin '%1' has a port that isn't an audio port or a "
                       "control port").arg(name);
                qWarning() << s;
                goto nextPlugin;
            }
        }
        if (! audioInputPorts) {
            s = tr("Plugin '%1' has no audio input ports").
                arg(name);
            qWarning() << s;
            continue;
        }
        if (! audioOutputPorts) {
            s = tr("Plugin '%1' has no audio output ports").
                arg(name);
            qWarning() << s;
            continue;
        }

        // Looks and smells like a plugin we can use.  Let's create the sections
        // and actions and add them to a list for sorting.
        subMenus.append("LV2");
        pluginClassCount = plugin.getClassCount();
        // We skip the first entry, which is the root 'Plugin' class.
        for (int j = 1; j < pluginClassCount; j++) {
            subMenus.append(plugin.getClass(j));
        }
        action = new synthclone::MenuAction(name, this);
        if (pluginClassCount > 1) {
            actionData = new MenuActionData(action, subMenus, this);
            menuActionDataList.append(actionData);
        } else {
            subMenus.append(tr("Uncategorized"));
            subMenus.append(name.left(1));
            actionData = new MenuActionData(action, subMenus, this);
            uncategorizedActionDataList.append(actionData);
        }

        // If we have a non-zero quality setting from above, then the plugin has
        // a UI that can be rendered with out view.  Otherwise, we'll be doing
        // the rendering ourselves.
        if (quality) {
            pluginUIData = new EffectViewData(bestUIData->getURI(), bestTypeURI,
                                              bestUIData->getBinaryPath(),
                                              bestUIData->getBundlePath(),
                                              plugin.getURI());
        } else {
            pluginUIData = new EffectViewData(plugin.getURI());
        }

        // Add plugin to lookup tables.
        actionPluginMap.insert(action, &plugin);
        pluginUIMap.insert(&plugin, pluginUIData);
        uriPluginMap.insert(plugin.getURI(), &plugin);

    nextPlugin:
        ;
    }

    // Sort and register the categorized actions.
    int categorizedActionCount = menuActionDataList.count();
    if (categorizedActionCount) {
        std::stable_sort(menuActionDataList.begin(), menuActionDataList.end(),
			 MenuActionDataComparer());
        while (menuActionDataList.count()) {
            MenuActionData *data = menuActionDataList.takeFirst();
            QScopedPointer<MenuActionData> dataPtr(data);
            synthclone::MenuAction *action = data->getAction();
            connect(action, SIGNAL(triggered()), SLOT(handleEffectAddition()));
            const synthclone::Registration &registration =
                context->addMenuAction(action, synthclone::MENU_ADD_EFFECT,
                                       data->getSections());
            connect(&registration, SIGNAL(unregistered(QObject *)),
                    SLOT(handleActionUnregistration(QObject *)));
        }
    }

    // Sort and register the uncategorized actions.
    if (uncategorizedActionDataList.count()) {
        std::stable_sort(uncategorizedActionDataList.begin(),
			 uncategorizedActionDataList.end(),
			 MenuActionDataComparer());
        if (categorizedActionCount) {
            synthclone::MenuSeparator *separator =
                new synthclone::MenuSeparator(this);
            const synthclone::Registration &separatorRegistration =
                context->addMenuSeparator(separator,
                                          synthclone::MENU_ADD_EFFECT,
                                          QStringList("LV2"));
            connect(&separatorRegistration, SIGNAL(unregistered(QObject *)),
                    SLOT(handleSeparatorUnregistration(QObject *)));
        }
        while (uncategorizedActionDataList.count()) {
            MenuActionData *data = uncategorizedActionDataList.takeFirst();
            QScopedPointer<MenuActionData> dataPtr(data);
            synthclone::MenuAction *action = data->getAction();
            connect(action, SIGNAL(triggered()), SLOT(handleEffectAddition()));
            const synthclone::Registration &registration =
                context->addMenuAction(action, synthclone::MENU_ADD_EFFECT,
                                       data->getSections());
            connect(&registration, SIGNAL(unregistered(QObject *)),
                    SLOT(handleActionUnregistration(QObject *)));
        }
    }
}

void
Participant::configureEffect(Effect *effect)
{
    // Set URIs for LV2 UI widget.
    const LV2Plugin &plugin = effect->getPlugin();
    EffectViewData *data = pluginUIMap.value(&plugin, 0);
    assert(data);
    effectView.setViewData(*data);

    // Setup effect state.
    int count = effect->getAudioInputPortCount();
    int i;
    for (i = 0; i < count; i++) {
        effectView.addAudioInputPort(effect->getAudioInputPortName(i));
    }
    count = effect->getAudioOutputPortCount();
    for (i = 0; i < count; i++) {
        effectView.addAudioOutputPort(effect->getAudioOutputPortName(i));
    }
    uint32_t index;
    float value;
    count = effect->getControlInputPortCount();
    for (i = 0; i < count; i++) {
        const LV2Port &port = plugin.getControlInputPort(i);
        bool success;
        float maximumValue = port.getMaximumValue().toFloat(&success);
        if (! success) {
            maximumValue = FLT_MAX;
        }
        float minimumValue = port.getMinimumValue().toFloat(&success);
        if (! success) {
            minimumValue = -FLT_MAX;
        }

        // This code is based on Dave Robilla's Jalv host:
        //
        //     http://drobilla.net/software/jalv/
        if (port.isSampleRatePort()) {
            synthclone::SampleRate sampleRate = context->getSampleRate();
            if (sampleRate == synthclone::SAMPLE_RATE_NOT_SET) {
                sampleRate = FALLBACK_SAMPLE_RATE;
            }
            maximumValue *= sampleRate;
            minimumValue *= sampleRate;
        }
        QMap<float, QString> scalePointsMap;
        for (int j = port.getScalePointCount() - 1; j >= 0; j--) {
            const LV2ScalePoint &point = port.getScalePoint(j);
            scalePointsMap.insert(point.getValue(), point.getLabel());
        }
        bool integerPort = port.isIntegerPort();
        ControlInputPortType type;
        if (port.isBooleanPort()) {
            type = CONTROLINPUTPORTTYPE_BOOLEAN;
        } else if (port.isEnumerationPort() ||
                   (integerPort && scalePointsMap.count())) {
            type = CONTROLINPUTPORTTYPE_ENUMERATION;
        } else if (integerPort) {
            type = CONTROLINPUTPORTTYPE_INTEGER;
        } else {
            type = CONTROLINPUTPORTTYPE_FLOAT;
        }
        index = effect->getControlInputPortIndex(i);
        value = effect->getControlInputPortValue(i);
        effectView.addControlInputPort(port.getName(), type, index, value,
                                       minimumValue, maximumValue,
                                       scalePointsMap);
        assert(! controlInputPortIndexMap.contains(index));
        controlInputPortIndexMap[index] = i;
    }
    count = effect->getControlOutputPortCount();
    for (i = 0; i < count; i++) {
        index = effect->getControlOutputPortIndex(i);
        value = effect->getControlOutputPortValue(i);
        effectView.setPortValue(index, sizeof(float), 0, &value);
        assert(! controlOutputPortIndexMap.contains(index));
        controlOutputPortIndexMap[index] = i;
    }
    synthclone::SampleChannelCount channels = context->getSampleChannelCount();
    for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
        effectView.setAudioInputChannel(i, effect->getAudioInputChannel(i));
        effectView.setAudioOutputChannel(i, effect->getAudioOutputChannel(i));
    }
    effectView.setInstanceCount(effect->getInstanceCount());
    effectView.setName(effect->getName());

    // Connect effect signals.
    connect(effect,
            SIGNAL(audioInputChannelChanged(synthclone::SampleChannelCount,
                                            int)),
            &effectView,
            SLOT(setAudioInputChannel(synthclone::SampleChannelCount, int)));
    connect(effect, SIGNAL(audioInputPortAdded(const QString &)),
            &effectView, SLOT(addAudioInputPort(const QString &)));
    connect(effect, SIGNAL(audioInputPortRemoved()),
            &effectView, SLOT(removeAudioInputPort()));
    connect(effect,
            SIGNAL(audioOutputChannelChanged(synthclone::SampleChannelCount,
                                             int)),
            &effectView,
            SLOT(setAudioOutputChannel(synthclone::SampleChannelCount, int)));
    connect(effect, SIGNAL(audioOutputPortAdded(const QString &)),
            &effectView, SLOT(addAudioOutputPort(const QString &)));
    connect(effect, SIGNAL(audioOutputPortRemoved()),
            &effectView, SLOT(removeAudioOutputPort()));
    connect(effect, SIGNAL(controlInputPortValueChanged(int, float)),
            SLOT(handleControlInputPortValueChange(int, float)));
    connect(effect, SIGNAL(controlOutputPortValueChanged(int, float)),
            SLOT(handleControlOutputPortValueChange(int, float)));
    connect(effect, SIGNAL(instanceCountChanged(int)),
            &effectView, SLOT(setInstanceCount(int)));
    connect(effect, SIGNAL(nameChanged(const QString &)),
            &effectView, SLOT(setName(const QString &)));

    // Connect effect view signals.
    connect(&effectView,
            SIGNAL(audioInputChannelChanged(synthclone::SampleChannelCount,
                                            int)),
            effect,
            SLOT(setAudioInputChannel(synthclone::SampleChannelCount, int)));
    connect(&effectView,
            SIGNAL(audioOutputChannelChanged(synthclone::SampleChannelCount,
                                             int)),
            effect,
            SLOT(setAudioOutputChannel(synthclone::SampleChannelCount, int)));
    connect(&effectView, SIGNAL(instanceCountChanged(int)),
            effect, SLOT(setInstanceCount(int)));
    connect(&effectView, SIGNAL(nameChanged(const QString &)),
            effect, SLOT(setName(const QString &)));
    connect(&effectView, SIGNAL(portValueChanged(uint32_t, uint32_t, uint32_t,
                                                 void const *)),
            SLOT(handlePortValueChange(uint32_t, uint32_t, uint32_t,
                                       void const *)));

    // Show the view.
    configuredEffect = effect;
    effectView.setVisible(true);
}

void
Participant::deactivate(synthclone::Context &context)
{
    configuredEffect = 0;
    disconnect(&context,
               SIGNAL(sampleChannelCountChanged
                      (synthclone::SampleChannelCount)),
               &effectView,
               SLOT(setSampleChannelCount(synthclone::SampleChannelCount)));
    for (int i = registeredEffects.count() - 1; i >= 0; i--) {
        context.removeEffect(registeredEffects[i]);
    }
    removePluginActions();
    this->context = 0;
    delete world;
}

QVariant
Participant::getState(const synthclone::Effect *effect) const
{
    const Effect *e = qobject_cast<const Effect *>(effect);
    assert(e);
    synthclone::SampleChannelCount channels = context->getSampleChannelCount();
    QVariantList inputChannels;
    QVariantList outputChannels;
    for (synthclone::SampleChannelCount i = 0; i < channels; i++) {
        inputChannels.append(static_cast<int>(e->getAudioInputChannel(i)));
        outputChannels.append(static_cast<int>(e->getAudioOutputChannel(i)));
    }
    QVariantMap map;
    map["inputChannels"] = inputChannels;
    map["instances"] = e->getInstanceCount();
    map["name"] = e->getName();
    map["outputChannels"] = outputChannels;
    map["state"] = e->getState();
    map["uri"] = e->getPlugin().getURI();
    return map;
}

void
Participant::handleActionUnregistration(QObject *obj)
{
    synthclone::MenuAction *action =
        qobject_cast<synthclone::MenuAction *>(obj);
    const LV2Plugin *plugin = actionPluginMap.take(action);
    uriPluginMap.remove(plugin->getURI());
    delete pluginUIMap.take(plugin);
    delete action;
}

void
Participant::handleControlInputPortValueChange(int index, float value)
{
    effectView.setPortValue(configuredEffect->getControlInputPortIndex(index),
                            sizeof(float), 0, &value);
}

void
Participant::handleControlOutputPortValueChange(int index, float value)
{
    effectView.setPortValue(configuredEffect->getControlOutputPortIndex(index),
                            sizeof(float), 0, &value);
}

void
Participant::handleEffectActionUnregistration(QObject *obj)
{
    delete obj;
}

void
Participant::handleEffectAddition()
{
    synthclone::MenuAction *action =
        qobject_cast<synthclone::MenuAction *>(sender());
    assert(action);
    const LV2Plugin *plugin = actionPluginMap.value(action, 0);
    assert(plugin);
    configureEffect(addEffect(plugin));
}

void
Participant::handleEffectConfiguration()
{
    synthclone::MenuAction *action =
        qobject_cast<synthclone::MenuAction *>(sender());
    assert(action);
    configureEffect(qobject_cast<Effect *>(action->parent()));
}

void
Participant::handleEffectUnregistration(QObject *obj)
{
    Effect *effect = qobject_cast<Effect *>(obj);
    assert(effect);
    bool removed = registeredEffects.removeOne(effect);
    assert(removed);
    delete effect;
}

void
Participant::handleEffectViewCloseRequest()
{
    effectView.setVisible(false);

    // Disconnect effect signals.
    disconnect(configuredEffect,
               SIGNAL(audioInputChannelChanged(synthclone::SampleChannelCount,
                                               int)),
               &effectView,
               SLOT(setAudioInputChannel(synthclone::SampleChannelCount, int)));
    disconnect(configuredEffect, SIGNAL(audioInputPortAdded(const QString &)),
               &effectView, SLOT(addAudioInputPort(const QString &)));
    disconnect(configuredEffect, SIGNAL(audioInputPortRemoved()),
               &effectView, SLOT(removeAudioInputPort()));
    disconnect(configuredEffect,
               SIGNAL(audioOutputChannelChanged(synthclone::SampleChannelCount,
                                                int)),
               &effectView,
               SLOT(setAudioOutputChannel(synthclone::SampleChannelCount,
                                          int)));
    disconnect(configuredEffect, SIGNAL(audioOutputPortAdded(const QString &)),
               &effectView, SLOT(addAudioOutputPort(const QString &)));
    disconnect(configuredEffect, SIGNAL(audioOutputPortRemoved()),
               &effectView, SLOT(removeAudioOutputPort()));
    disconnect(configuredEffect,
               SIGNAL(controlInputPortValueChanged(int, float)),
               this, SLOT(handleControlInputPortValueChange(int, float)));
    disconnect(configuredEffect,
               SIGNAL(controlOutputPortValueChanged(int, float)),
               this, SLOT(handleControlOutputPortValueChange(int, float)));
    disconnect(configuredEffect, SIGNAL(instanceCountChanged(int)),
               &effectView, SLOT(setInstanceCount(int)));
    disconnect(configuredEffect, SIGNAL(nameChanged(const QString &)),
               &effectView, SLOT(setName(const QString &)));

    // Disconnect effect view signals.
    disconnect(&effectView,
               SIGNAL(audioInputChannelChanged(synthclone::SampleChannelCount,
                                               int)),
               configuredEffect,
               SLOT(setAudioInputChannel(synthclone::SampleChannelCount, int)));
    disconnect(&effectView,
               SIGNAL(audioOutputChannelChanged(synthclone::SampleChannelCount,
                                                int)),
               configuredEffect,
               SLOT(setAudioOutputChannel(synthclone::SampleChannelCount,
                                          int)));
    disconnect(&effectView, SIGNAL(instanceCountChanged(int)),
               configuredEffect, SLOT(setInstanceCount(int)));
    disconnect(&effectView, SIGNAL(nameChanged(const QString &)),
               configuredEffect, SLOT(setName(const QString &)));
    disconnect(&effectView, SIGNAL(portValueChanged(uint32_t, uint32_t,
                                                    uint32_t, void const *)),
               this,
               SLOT(handlePortValueChange(uint32_t, uint32_t, uint32_t,
                                          void const *)));

    // Clear ports.
    effectView.clearAudioInputPorts();
    effectView.clearAudioOutputPorts();
    effectView.clearControlInputPorts();
    controlInputPortIndexMap.clear();
    controlOutputPortIndexMap.clear();

    configuredEffect = 0;
}

void
Participant::handlePortValueChange(uint32_t portIndex, uint32_t bufferSize,
                                   uint32_t protocol, void const *buffer)
{
    if (protocol) {
        qWarning() << tr("protocol '%1' isn't handled by plugin").arg(protocol);
        return;
    }
    if (bufferSize != sizeof(float)) {
        qWarning() << tr("size '%1' does not equal sizeof(float)").
            arg(bufferSize);
        return;
    }
    float value;
    memcpy(&value, buffer, bufferSize);
    int index = controlInputPortIndexMap.value(portIndex, -1);
    if (index != -1) {
        configuredEffect->setControlInputPortValue(index, value);
        return;
    }
    index = controlOutputPortIndexMap.value(portIndex, -1);
    if (index != -1) {
        configuredEffect->setControlOutputPortValue(index, value);
        return;
    }
    qWarning() << tr("port index '%1' is not a control input or output port").
        arg(portIndex);
}

void
Participant::handleSeparatorUnregistration(QObject *obj)
{
    delete obj;
}

void
Participant::removePluginActions()
{
    QList<synthclone::MenuAction *> actions = actionPluginMap.keys();
    for (int i = actions.count() - 1; i >= 0; i--) {
        context->removeMenuAction(actions[i]);
    }
}

void
Participant::restoreEffect(const QVariant &state)
{
    // Make sure we have valid plugin data
    const QVariantMap map = state.toMap();
    QVariant uriData = map.value("uri");
    if (! uriData.isValid()) {
        context->reportError(tr("LV2 effect state doesn't contain plugin URI"));
        return;
    }
    QString uri = uriData.toString();
    const LV2Plugin *plugin = uriPluginMap.value(uri, 0);
    if (! plugin) {
        context->reportError(tr("no loaded plugin with plugin URI '%1'").
                             arg(uri));
        return;
    }

    // Create effect
    Effect *effect = addEffect(plugin);
    effect->setName(map.value("name", plugin->getName()).toString());
    effect->setState(map.value("state", "").toByteArray());

    // Set instance count
    bool success;
    QVariant value = map.value("instances", 0);
    int instanceCount = value.toInt(&success);
    if (! success) {
        qWarning() << tr("state does not contain instance count");
        instanceCount = 1;
    } else if (instanceCount < 1) {
        qWarning() << tr("state has bad instance count '%1'").
            arg(instanceCount);
        instanceCount = 1;
    }
    effect->setInstanceCount(instanceCount);

    // Input channel map
    QVariantList channelMap = map.value("inputChannels", "").toList();
    synthclone::SampleChannelCount channels = context->getSampleChannelCount();
    int count = channelMap.count();
    synthclone::SampleChannelCount effectChannelCount;
    synthclone::SampleChannelCount index;
    QLocale locale;
    if (! count) {
        qWarning() << tr("input channel map not found");
        goto loadOutputChannelMap;
    }
    if (count != channels) {
        qWarning() << tr("channel count does not match input channel map "
                         "length");
        goto loadOutputChannelMap;
    }
    effectChannelCount = effect->getAudioInputPortCount();
    locale = QLocale::system();
    for (synthclone::SampleChannelCount i = 0; i < count; i++) {
        value = channelMap[i];
        index = value.toInt(&success);
        if (! success) {
            qWarning() << tr("sample channel '%1' has non-integer input "
                             "channel '%2'").
                arg(locale.toString(i), value.toString());
            continue;
        }
        if (index > effectChannelCount) {
            qWarning() << tr("sample channel '%1' has out of range input "
                             "channel '%2'").
                arg(locale.toString(i), locale.toString(index));
            continue;
        }
        effect->setAudioInputChannel
            (static_cast<synthclone::SampleChannelCount>(i),
             static_cast<synthclone::SampleChannelCount>(index));
    }

 loadOutputChannelMap:
    // Output channel map
    channelMap = map.value("outputChannels", "").toList();
    count = channelMap.count();
    if (! count) {
        qWarning() << tr("output channel map not found");
        return;
    }
    if (count != channels) {
        qWarning() << tr("channel count does not match output channel map "
                         "length");
        return;
    }
    effectChannelCount = effect->getAudioOutputPortCount();
    for (synthclone::SampleChannelCount i = 0; i < count; i++) {
        value = channelMap[i];
        index = value.toInt(&success);
        if (! success) {
            qWarning() << tr("sample channel '%1' has non-integer output "
                             "channel '%2'").
                arg(locale.toString(i), value.toString());
            continue;
        }
        if (index > effectChannelCount) {
            qWarning() << tr("sample channel '%1' has out of range output "
                             "channel '%2'").
                arg(locale.toString(i), locale.toString(index));
            continue;
        }
        effect->setAudioOutputChannel
            (static_cast<synthclone::SampleChannelCount>(i),
             static_cast<synthclone::SampleChannelCount>(index));
    }
}
