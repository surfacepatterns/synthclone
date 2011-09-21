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

#ifndef __SYNTHCLONE_ERROR_H__
#define __SYNTHCLONE_ERROR_H__

#include <QtCore/QString>

namespace synthclone {

    /**
     * An exception class used with `synthclone` specific errors.
     */

    class Error {

    public:

        /**
         * Constructs a new Error object.
         *
         * @param message
         *   The error message.
         */

        explicit
        Error(const QString &message);

        /**
         * Destroys the Error object.
         */

        ~Error();

        /**
         * Gets the error message.
         *
         * @return
         *   The error message.
         */

        QString
        getMessage();

    private:

        QString message;

    };

}

#endif
