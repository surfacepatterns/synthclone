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

#ifndef __SYNTHCLONE_VIEW_H__
#define __SYNTHCLONE_VIEW_H__

#include <QtGui/QWidget>

namespace synthclone {

    class CloseEventFilter;

    /**
     * Utility view class.  Captures any close event and emits a signal instead
     * of requiring the subclassing of a widget.
     *
     * Views based on this class attempt to build on the 'passive view' design
     * pattern, meaning that the view knows nothing about the model, and is
     * subject to the will of the controller.
     */

    class View: public QObject {

        Q_OBJECT

    public:

        /**
         * Gets a boolean indicating whether or not closing of this view is
         * enabled.
         *
         * @returns
         *   The boolean.
         */

        bool
        isCloseEnabled() const;

        /**
         * Gets a boolean indicating whether or not the view is visible.
         *
         * @returns
         *   The boolean.
         */

        bool
        isVisible() const;

    public slots:

        /**
         * Enables/disables closing for this view.  If closing is disabled,
         * closeRequest() signals will not be raised when a user presses the
         * close button in the taskbar.  Closing is enabled by default.
         *
         * @param enabled
         *   Whether or not closing is enabled.
         */

        virtual void
        setCloseEnabled(bool enabled);

        /**
         * Sets the visibility of the root widget.
         *
         * @param visible
         *   Whether or not the root widget should be visible.
         */

        virtual void
        setVisible(bool visible);

    signals:

        /**
         * Emitted when closing is enabled/disabled.
         *
         * @param enabled
         *   Whether or not closing is enabled.
         */

        void
        closeEnabledChanged(bool enabled);

        /**
         * Emitted when a root widget's close button is pressed.
         */

        void
        closeRequest();

        /**
         * Emitted when the visibility flag for the view changes.
         *
         * @param visible
         *   The updated visibility flag.
         */

        void
        visibilityChanged(bool visible);

    protected:

        /**
         * Constructs a new View.  This class is not meant to be called
         * directly; instead, subclass constructors should call this
         * constructor.
         *
         * @param rootWidget
         *   The root widget for this View.
         *
         * @param parent
         *   The parent object of this View.
         */

        explicit
        View(QWidget *rootWidget, QObject *parent=0);

        /**
         * Destroys the View object.
         */

        virtual
        ~View();

        /**
         * Gets the root widget for this view.
         *
         * @returns
         *   The root widget.
         */

        const QWidget *
        getRootWidget() const;

        /**
         * Gets the root widget for this view.
         *
         * @returns
         *   The root widget.
         */

        QWidget *
        getRootWidget();

    private:

        CloseEventFilter *closeEventFilter;
        QWidget *rootWidget;

    };

}

#endif
