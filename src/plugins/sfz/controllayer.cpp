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

#include <synthclone/util.h>

#include "controllayer.h"

ControlLayer::ControlLayer(synthclone::MIDIData control, QObject *parent):
    QObject(parent)
{
    assert((control < 0x80) || (control == 0x81) || (control == 0x82));
    continuous = ((control == 0x81) || (control == 0x82)) ? true :
        synthclone::getMIDIControlType(control) !=
        synthclone::CONTROLTYPE_SWITCH;
    this->control = control;
    defaultValue = 0;
}

ControlLayer::~ControlLayer()
{
    // Empty
}

synthclone::MIDIData
ControlLayer::getControl() const
{
    return control;
}

synthclone::MIDIData
ControlLayer::getDefaultValue() const
{
    return defaultValue;
}

synthclone::ControlType
ControlLayer::getType() const
{
    return continuous ? synthclone::CONTROLTYPE_CONTINUOUS :
        synthclone::CONTROLTYPE_SWITCH;
}

bool
ControlLayer::isCrossfadingEnabled() const
{
    return crossfadingEnabled;
}

void
ControlLayer::setCrossfadingEnabled(bool enabled)
{
    if (crossfadingEnabled != enabled) {
        crossfadingEnabled = enabled;
        emit crossfadingEnabledChanged(enabled);
        if (enabled) {
            setType(synthclone::CONTROLTYPE_CONTINUOUS);
        }
    }
}

void
ControlLayer::setDefaultValue(synthclone::MIDIData defaultValue)
{
    assert(defaultValue < 0x80);
    if (this->defaultValue != defaultValue) {
        this->defaultValue = defaultValue;
        emit defaultValueChanged(defaultValue);
    }
}

void
ControlLayer::setType(synthclone::ControlType type)
{
    bool continuous;
    switch (type) {
    case synthclone::CONTROLTYPE_CONTINUOUS:
        continuous = true;
        break;
    case synthclone::CONTROLTYPE_SWITCH:
        continuous = false;
        break;
    default:
        assert(false);
    }
    if (this->continuous != continuous) {
        this->continuous = continuous;
        emit typeChanged(type);
        if (! continuous) {
            setCrossfadingEnabled(false);
        }
    }
}
