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

#ifndef __SYNTHCLONE_DESIGNERVIEW_H__
#define __SYNTHCLONE_DESIGNERVIEW_H__

#include <synthclone/view.h>

namespace synthclone {

    /**
     * Utility class for loading views created with QtDesigner.
     */

    class DesignerView: public View {

        Q_OBJECT

    protected:

        /**
         * Constructs a new DesignerView.  This constructor should not be
         * called directly; instead, subclass constructors should call this
         * constructor.
         *
         * @param path
         *   The path to the QtDesigner file.
         *
         * @param parent
         *   The parent object of the new DesignerView.
         */

        explicit
        DesignerView(const QString &path, QObject *parent=0);

        /**
         * Destroys a DesignerView object.
         */

        virtual
        ~DesignerView();

    };

}

#endif
