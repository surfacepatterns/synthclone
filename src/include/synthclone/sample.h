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

#ifndef __SYNTHCLONE_SAMPLE_H__
#define __SYNTHCLONE_SAMPLE_H__

#include <QtCore/QObject>
#include <QtCore/QString>

namespace synthclone {

    class SampleInputStream;
    class SampleOutputStream;

    /**
     * Contains sample data.  Sample data is kept in a file to conserve memory.
     * To access the sample data, use the SampleInputStream class.  To write
     * sample data, use the SampleOutputStream class.
     */

    class Sample: public QObject {

        Q_OBJECT

        friend class SampleInputStream;
        friend class SampleOutputStream;

    public:

        /**
         * Initializes an empty sample object.  Sample contents are stored in a
         * file in the system's temporary directory.  This constructor should
         * be used when a SampleOutputStream object will be used to write data
         * to this sample.
         *
         * @param temporary
         *   Whether or not the file associated with the sample should be
         *   removed when the sample object is deleted.
         *
         * @param parent
         *   The parent object of the new sample.
         */

        explicit
        Sample(bool temporary=true, QObject *parent=0);

        /**
         * Initializes a sample object.  This constructor should be used when a
         * SampleOutputStream object will be used to write data to this sample,
         * or when there is already sample data available at the given path.
         *
         * @param path
         *   The file path this sample object should use.
         *
         * @param temporary
         *   Whether or not the file associated with the sample should be
         *   removed when the sample object is deleted.
         *
         * @param parent
         *   The parent object of the new sample.
         */

        explicit
        Sample(const QString &path, bool temporary=false, QObject *parent=0);

        /**
         * Initializes a sample object.  Sample contents are stored in a file
         * in the system's temporary directory.
         *
         * @param sample
         *   A sample object.  Sample contents will be copied from this object
         *   to the new sample object.
         *
         * @param temporary
         *   Whether or not the file associated with the sample should be
         *   removed when the sample object is deleted.
         *
         * @param parent
         *   The parent object of the new sample.
         */

        explicit
        Sample(const Sample &sample, bool temporary=true, QObject *parent=0);

        /**
         * Initializes a sample object.
         *
         * @param sample
         *   A sample object.  Sample contents will be copied from this object
         *   to the new sample object.
         *
         * @param path
         *   The file path this sample object should use.
         *
         * @param temporary
         *   Whether or not the file associated with the sample should be
         *   removed when the sample object is deleted.
         *
         * @param parent
         *   The parent object of the new sample.
         */

        Sample(const Sample &sample, const QString &path, bool temporary=false,
               QObject *parent=0);

        /**
         * If the file linked to this sample object is a temporary file, then
         * the temporary file is deleted.  Otherwise, nothing happens.
         */

        ~Sample();

        /**
         * Gets the path to the file holding this sample.
         *
         * @returns
         *   The path.
         */

        QString
        getPath() const;

        /**
         * Returns a boolean indicating whether or not the file referenced by
         * this object will be deleted when this object is destroyed.
         *
         * @returns
         *   The specified boolean.
         */

        bool
        isTemporary() const;

    public slots:

        /**
         * Sets a boolean indicating whether or not the file referenced by this
         * object will be deleted when this object is destroyed.
         *
         * @param temporary
         *   The specified boolean.
         */

        void
        setTemporary(bool temporary);

    signals:

        /**
         * Emitted when the temporary flag changes.
         *
         * @param temporary
         *   The specified flag.
         */

        void
        temporaryChanged(bool temporary);

    private:

        void
        initializeData(const Sample &sample);

        void
        initializeTemporaryPath();

        QString path;
        bool temporary;

    };

}

#endif
