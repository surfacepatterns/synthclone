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

#ifndef __SYNTHCLONE_UTIL_H__
#define __SYNTHCLONE_UTIL_H__

#include <QtCore/QCoreApplication>
#include <QtGui/QWidget>

#include <synthclone/sampleinputstream.h>
#include <synthclone/sampleoutputstream.h>

/**
 * Confirms that a given condition is true, regardless of whether or not NDEBUG
 * is set.  If the condition does not evaluate to true, then an error
 * message is printed that includes the file, line, and function where the error
 * occurred, along with an error message.  The program is then terminated using
 * abort().
 *
 * @param condition
 *   The condition to evaluate.
 *
 * @param message
 *   An error message that describes the potential problem.
 */

#define CONFIRM(condition, message) \
    if (! (condition)) { \
        synthclone::_die(__FILE__, Q_FUNC_INFO, __LINE__, message); \
    }

namespace synthclone {

    /**
     * @private
     */

    void
    _die(const char *path, const char *func, int line, const QString &message);

    /**
     * Finds a child object of an object.  If the child object is not found,
     * then an error message is printed and the program is aborted.
     *
     * @param object
     *   The parent object.
     *
     * @param name
     *   The name of the child object to find.
     *
     * @returns
     *   The child object.
     */

    template<typename T>
    inline T *
    getChild(const QObject *object, const QString &name=QString())
    {
        T *child = qFindChild<T *>(object, name);
        CONFIRM(child,
                qApp->tr("object has no child with name '%1' and base type "
                         "'%2'").arg(name, T::staticMetaObject.className()));
        return child;
    }

    /**
     * Gets a string representation of a MIDI control.
     *
     * @param control
     *   The MIDI control.
     *
     * @returns
     *   The string representation of the MIDI control.
     */

    QString
    getMIDIControlString(MIDIData control);

    /**
     * Gets a string representation of a MIDI note.
     *
     * @param note
     *   The MIDI note.
     *
     * @returns
     *   The string representation of the MIDI note.
     */

    QString
    getMIDINoteString(MIDIData note);

    /**
     * Gets an appropriate filename extension for a sample type.
     *
     * @param type
     *   The SampleStream::Type.
     *
     * @returns
     *   An appropriate filename extension for the given type.
     */

    QString
    getSampleFilenameExtension(SampleStream::Type type);

    /**
     * Loads the main widget from a QtDesigner form.  If the widget can't be
     * loaded, then an error message is printed and the program is aborted.
     *
     * @param path
     *   The path to the form.
     *
     * @param parent
     *   The optional parent widget of the widget being loaded.
     *
     * @returns
     *   The loaded widget.
     */

    QWidget *
    loadForm(const QString &path, QWidget *parent=0);

}

#endif
