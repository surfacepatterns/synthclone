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

#include "lv2plugin.h"

LV2Plugin::LV2Plugin(const LilvPlugin *plugin, LilvWorld *world,
                     QObject *parent):
    QObject(parent)
{
    assert(plugin);
    const LilvNode *node;

    // Plugin classes
    const LilvPluginClasses *classes = lilv_world_get_plugin_classes(world);
    const LilvPluginClass *pluginClass = lilv_plugin_get_class(plugin);
    while (pluginClass) {
        node = lilv_plugin_class_get_label(pluginClass);
        QString className(lilv_node_as_string(node));
        classList.prepend(className);
        node = lilv_plugin_class_get_parent_uri(pluginClass);
        if (! node) {
            break;
        }
        pluginClass = lilv_plugin_classes_get_by_uri(classes, node);
    }

    // UIs
    LilvUIs *uis = lilv_plugin_get_uis(plugin);
    LILV_FOREACH(uis, iter, uis) {
        uiDataList.append(new LV2UIData(lilv_uis_get(uis, iter), this));
    }
    lilv_uis_free(uis);

    // Required features
    LilvNodes *nodes = lilv_plugin_get_required_features(plugin);
    LILV_FOREACH(nodes, iter, nodes) {
        node = lilv_nodes_get(nodes, iter);
        assert(node);
        requiredFeatures.append(lilv_node_as_string(node));
    }
    lilv_nodes_free(nodes);

    // Ports
    uint32_t portCount = lilv_plugin_get_num_ports(plugin);
    for (uint32_t i = 0; i < portCount; i++) {
        const LilvPort *lilvPort = lilv_plugin_get_port_by_index(plugin, i);
        LV2Port *port = new LV2Port(lilvPort, plugin, world, this);
        ports.append(port);
        if (port->isAudioPort()) {
            if (port->isInputPort()) {
                audioInputPorts.append(port);
            } else if (port->isOutputPort()) {
                audioOutputPorts.append(port);
            }
        } else if (port->isControlPort()) {
            if (port->isInputPort()) {
                controlInputPorts.append(port);
            } else if (port->isOutputPort()) {
                controlOutputPorts.append(port);
            }
        }
    }

    this->plugin = plugin;
    this->world = world;
}

LV2Plugin::~LV2Plugin()
{
    for (int i = uiDataList.count() - 1; i >= 0; i--) {
        delete uiDataList[i];
    }
    for (int i = ports.count() - 1; i >= 0; i--) {
        delete ports[i];
    }
}

const LV2Port &
LV2Plugin::getAudioInputPort(int index) const
{
    return *(audioInputPorts[index]);
}

int
LV2Plugin::getAudioInputPortCount() const
{
    return audioInputPorts.count();
}

const LV2Port &
LV2Plugin::getAudioOutputPort(int index) const
{
    return *(audioOutputPorts[index]);
}

int
LV2Plugin::getAudioOutputPortCount() const
{
    return audioOutputPorts.count();
}

QString
LV2Plugin::getClass(int index) const
{
    assert((index >= 0) && (index < classList.count()));
    return classList[index];
}

int
LV2Plugin::getClassCount() const
{
    return classList.count();
}

const LV2Port &
LV2Plugin::getControlInputPort(int index) const
{
    return *(controlInputPorts[index]);
}

int
LV2Plugin::getControlInputPortCount() const
{
    return controlInputPorts.count();
}

const LV2Port &
LV2Plugin::getControlOutputPort(int index) const
{
    return *(controlOutputPorts[index]);
}

int
LV2Plugin::getControlOutputPortCount() const
{
    return controlOutputPorts.count();
}

QString
LV2Plugin::getName() const
{
    LilvNode *node = lilv_plugin_get_name(plugin);
    assert(node);
    return lilv_node_as_string(node);
}

const LV2Port &
LV2Plugin::getPort(int index) const
{
    return *(ports[index]);
}

int
LV2Plugin::getPortCount() const
{
    return ports.count();
}

QString
LV2Plugin::getRequiredFeature(int index) const
{
    assert((index >= 0) && (index < requiredFeatures.count()));
    return requiredFeatures[index];
}

int
LV2Plugin::getRequiredFeatureCount() const
{
    return requiredFeatures.count();
}

const LV2UIData &
LV2Plugin::getUIData(int index) const
{
    assert((index >= 0) && (index < uiDataList.count()));
    return *(uiDataList.at(index));
}

int
LV2Plugin::getUIDataCount() const
{
    return uiDataList.count();
}

QString
LV2Plugin::getURI() const
{
    const LilvNode *node = lilv_plugin_get_uri(plugin);
    assert(node);
    return lilv_node_as_uri(node);
}

LV2Instance *
LV2Plugin::instantiate(double sampleRate) const
{
    return new LV2Instance(plugin, world, sampleRate);
}

bool
LV2Plugin::isReplaced() const
{
    return static_cast<bool>(lilv_plugin_is_replaced(plugin));
}
