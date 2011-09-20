#ifndef __SYNTHCLONE_EFFECTJOB_H__
#define __SYNTHCLONE_EFFECTJOB_H__

#include <synthclone/zone.h>

namespace synthclone {

    /**
     * Represents a job that will be sent to registered Effect objects.
     */

    class EffectJob: public QObject {

        Q_OBJECT

    public:

        /**
         * Gets the Zone that the Effect will use to get information for this
         * EffectJob.
         *
         * @returns
         *   The Zone.
         */

        virtual const Zone *
        getZone() const = 0;

    protected:

        /**
         * Constructs a new EffectJob object.  EffectJob objects cannot be
         * created directly.  EffectJob objects are created by calling
         * Context::addEffectJob().
         *
         * @param parent
         *   The parent object of the new EffectJob.
         */

        explicit
        EffectJob(QObject *parent=0);

        /**
         * Destroys an EffectJob.  The application is responsible for managing
         * EffectJob memory.
         */

        virtual
        ~EffectJob();

    };

}

#endif
