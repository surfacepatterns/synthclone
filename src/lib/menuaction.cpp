/*
 * libsynthclone - a plugin API for `synthclone`
 * Copyright (C) 2011 Devin Anderson
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <synthclone/menuaction.h>

using synthclone::MenuAction;

MenuAction::MenuAction(QObject *parent):
    MenuItem(parent)
{
    initialize("", "");
}

MenuAction::MenuAction(const QString &text, QObject *parent):
    MenuItem(parent)
{
    initialize(text, "");
}

MenuAction::MenuAction(const QString &text, const QString &description,
                       QObject *parent):
    MenuItem(parent)
{
    initialize(text, description);
}

MenuAction::~MenuAction()
{
    // Empty
}

QString
MenuAction::getDescription() const
{
    return description;
}

QString
MenuAction::getText() const
{
    return text;
}

bool
MenuAction::isEnabled() const
{
    return enabled;
}

void
MenuAction::initialize(const QString &text, const QString &description)
{
    this->description = description;
    this->text = text;
    enabled = true;
}

void
MenuAction::setDescription(const QString &description)
{
    if (description != this->description) {
        this->description = description;
        emit descriptionChanged(description);
    }
}

void
MenuAction::setEnabled(bool enabled)
{
    if (enabled != this->enabled) {
        this->enabled = enabled;
        emit enabledChanged(enabled);
    }
}

void
MenuAction::setText(const QString &text)
{
    if (text != this->text) {
        this->text = text;
        emit textChanged(text);
    }
}

void
MenuAction::trigger()
{
    emit triggered();
}
