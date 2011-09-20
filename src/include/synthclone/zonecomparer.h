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
