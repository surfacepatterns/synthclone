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

#include "lv2port.h"

LV2Port::LV2Port(const LilvPort *port, const LilvPlugin *plugin,
                 LilvWorld *world, QObject *parent):
    QObject(parent)
{
    LilvNode *defaultNode;
    LilvNode *maximumNode;
    LilvNode *minimumNode;
    lilv_port_get_range(plugin, port, &defaultNode, &minimumNode, &maximumNode);
    defaultValue = getNodeValue(defaultNode);
    maximumValue = getNodeValue(maximumNode);
    minimumValue = getNodeValue(minimumNode);
    if (defaultNode) {
        lilv_node_free(defaultNode);
    }
    if (maximumNode) {
        lilv_node_free(maximumNode);
    }
    if (minimumNode) {
        lilv_node_free(minimumNode);
    }
    this->plugin = plugin;
    this->port = port;
    this->world = world;
}

LV2Port::~LV2Port()
{
    // Empty
}

QVariant
LV2Port::getDefaultValue() const
{
    return defaultValue;
}

uint32_t
LV2Port::getIndex() const
{
    return lilv_port_get_index(plugin, port);
}

QVariant
LV2Port::getMaximumValue() const
{
    return maximumValue;
}

QVariant
LV2Port::getMinimumValue() const
{
    return minimumValue;
}

QString
LV2Port::getName() const
{
    LilvNode *node = lilv_port_get_name(plugin, port);
    assert(node);
    QString name(lilv_node_as_string(node));
    lilv_node_free(node);
    return name;
}

QVariant
LV2Port::getNodeValue(const LilvNode *node) const
{
    QVariant value;
    if (node) {
        if (lilv_node_is_bool(node)) {
            value = lilv_node_as_bool(node);
        } else if (lilv_node_is_float(node)) {
            value = lilv_node_as_float(node);
        } else if (lilv_node_is_int(node)) {
            value = lilv_node_as_int(node);
        } else if (lilv_node_is_string(node)) {
            value = lilv_node_as_string(node);
        } else if (lilv_node_is_uri(node)) {
            value = lilv_node_as_uri(node);
        } else if (lilv_node_is_blank(node)) {
            value = lilv_node_as_blank(node);
        } else {
            assert(false);
        }
    }
    return value;
}

QString
LV2Port::getSymbol() const
{
    const LilvNode *node = lilv_port_get_symbol(plugin, port);
    assert(node);
    return lilv_node_as_string(node);
}

bool
LV2Port::isAudioPort() const
{
    return isType(LILV_URI_AUDIO_PORT);
}

bool
LV2Port::isConnectionOptional() const
{
    LilvNode *uriNode =
        lilv_new_uri(world, "http://lv2plug.in/ns/lv2core#connectionOptional");
    assert(uriNode);
    bool result = lilv_port_has_property(plugin, port, uriNode);
    lilv_node_free(uriNode);
    return result;
}

bool
LV2Port::isControlPort() const
{
    return isType(LILV_URI_CONTROL_PORT);
}

bool
LV2Port::isInputPort() const
{
    return isType(LILV_URI_INPUT_PORT);
}

bool
LV2Port::isOutputPort() const
{
    return isType(LILV_URI_OUTPUT_PORT);
}

bool
LV2Port::isType(const QString &typeURI) const
{
    QByteArray typeURIBytes = typeURI.toAscii();
    LilvNode *node = lilv_new_uri(world, typeURIBytes.constData());
    assert(node);
    bool result = lilv_port_is_a(plugin, port, node);
    lilv_node_free(node);
    return result;
}
