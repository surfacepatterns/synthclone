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

#ifndef __SYNTHCLONE_MENUACTION_H__
#define __SYNTHCLONE_MENUACTION_H__

#include <QtCore/QString>

#include <synthclone/menuitem.h>

namespace synthclone {

    /**
     * MenuAction objects are triggerable choices in a menu.
     */

    class MenuAction: public MenuItem {

        Q_OBJECT

    public:

        /**
         * Constructs an new MenuAction object.
         *
         * @param parent
         *   The parent object of the new menu action.
         */

        explicit
        MenuAction(QObject *parent=0);

        /**
         * Constructs a new MenuAction object.
         *
         * @param text
         *   The default text for the new menu action.
         *
         * @param parent
         *   The parent object of the new menu action.
         */

        explicit
        MenuAction(const QString &text, QObject *parent=0);

        /**
         * Constructs a new MenuAction object.
         *
         * @param text
         *   The default text for the new menu action.
         *
         * @param description
         *   The default description for the new menu action.
         *
         * @param parent
         *   The parent object of the new menu action.
         */

        MenuAction(const QString &text, const QString &description,
                   QObject *parent=0);

        /**
         * Destroys the MenuAction object.
         */

        ~MenuAction();

        /**
         * Gets the action description.
         *
         * @returns
         *   The description.
         */

        QString
        getDescription() const;

        /**
         * Gets the action text.
         *
         * @returns
         *   The action text.
         */

        QString
        getText() const;

        /**
         * Gets a boolean indicating whether or not this action is able to be
         * triggered.
         *
         * @returns
         *   The specified boolean.
         */

        bool
        isEnabled() const;

    public slots:

        /**
         * Sets the description.
         *
         * @param description
         *   The description.
         */

        void
        setDescription(const QString &description);

        /**
         * Sets a boolean indicating whether or not this action is able to be
         * triggered.
         *
         * @param enabled
         *   The specified boolean.
         */

        void
        setEnabled(bool enabled);

        /**
         * Sets the action text.
         *
         * @param text
         *   The action text.
         */

        void
        setText(const QString &text);

        /**
         * Triggers the action.  This method is called from within `synthclone`.
         */

        void
        trigger();

    signals:

        /**
         * Emitted when the description is changed.
         *
         * @param description
         *   The new description.
         */

        void
        descriptionChanged(const QString &description);

        /**
         * Emitted when the 'enabled' flag is changed.
         *
         * @param enabled
         *   Whether or not the action is now enabled.
         */

        void
        enabledChanged(bool enabled);

        /**
         * Emitted when the action text is changed.
         *
         * @param text
         *   The new action text.
         */

        void
        textChanged(const QString &text);

        /**
         * Emitted when this action is triggered.
         */

        void
        triggered();

    private:

        void
        initialize(const QString &text, const QString &description);

        QString description;
        bool enabled;
        QString text;

    };

}

#endif
