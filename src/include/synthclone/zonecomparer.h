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

#ifndef __SYNTHCLONE_ZONECOMPARER_H__
#define __SYNTHCLONE_ZONECOMPARER_H__

#include <synthclone/zone.h>

namespace synthclone {

    /**
     * Subclasses of 'ZoneComparer' are used to sort zones, generally by a
     * specific property.
     *
     * @sa
     *   Context::sortZones()
     */

    class ZoneComparer: public QObject {

        Q_OBJECT

    public:

        /**
         * Indicates whether one Zone object should come before a second Zone
         * object.
         *
         * @param zone1
         *   The first Zone object.
         *
         * @param zone2
         *   The second Zone object.
         *
         * @returns
         *   A boolean indicating the above.
         */

        virtual bool
        isLessThan(const Zone *zone1, const Zone *zone2) const = 0;

    protected:

        /**
         * Constructs a new ZoneComparer object.  This constructor should not
         * be called directly; instead, constructors in subclasses should call
         * this constructor when they're invoked.
         *
         * @param parent
         *   The parent object of the new ZoneComparer object.
         */

        explicit
        ZoneComparer(QObject *parent=0);

        virtual
        ~ZoneComparer();

    };

}

#endif
