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

#ifndef __SYNTHCLONE_COMPONENT_H__
#define __SYNTHCLONE_COMPONENT_H__

#include <QtCore/QObject>
#include <QtCore/QString>

namespace synthclone {

    /**
     * Base class for `synthclone` components.  This includes Effect, Sampler,
     * and Target objects.
     */

    class Component: public QObject {

        Q_OBJECT

    public:

        /**
         * Gets the Component name.
         *
         * @return
         *   The name of this component.
         */

        QString
        getName() const;

    public slots:

        /**
         * Sets the name of this Component.
         *
         * @param name
         *   The component name.
         */

        void
        setName(const QString &name);

    signals:

        /**
         * Emitted when the Component name is changed.  Subclasses do not need
         * to worry about this signal.
         *
         * @param name
         *   The new name.
         */

        void
        nameChanged(const QString &name);

        /**
         * Emitted by the Component to indicate progress in whatever operation
         * the Component is performing.
         *
         * @param progress
         *   The 'progress' amount, which must be in the range [0.0, 1.0], where
         *   0.0 and 1.0 are roughly equivalent to "just starting" and
         *   "finished".
         *
         * @sa
         *   Effect::process, Sampler::startJob, Target::save, Target::validate
         */

        void
        progressChanged(float progress);

        /**
         * Emitted when the Component object's status changes.
         *
         * @param status
         *   The new status.
         */

        void
        statusChanged(const QString &status);

    protected:

        /**
         * Constructs a new Component object.  This constructor shouldn't be
         * called directly; instead, one of the Component subclasses should be
         * used.
         *
         * @param name
         *   The initial component name.
         *
         * @param parent
         *   The parent object of the component.
         *
         * @sa
         *   Effect, Sampler, Target
         */

        explicit
        Component(const QString &name, QObject *parent=0);

        /**
         * Destroys a Component object.
         */

        virtual
        ~Component();

    private:

        QString name;

    };

}

#endif
