#ifndef __SYNTHCLONE_SAMPLERJOB_H__
#define __SYNTHCLONE_SAMPLERJOB_H__

#include <synthclone/zone.h>

namespace synthclone {

    /**
     * Represents a job that will be sent to a Sampler.
     */

    class SamplerJob: public QObject {

        Q_OBJECT

    public:

        /**
         * The type of job that will be executed by the Sampler.
         */

        enum Type {
            TYPE_PLAY_DRY_SAMPLE=0,
            TYPE_PLAY_WET_SAMPLE=1,
            TYPE_SAMPLE=2
        };

        /**
         * Gets the SamplerJob::Type.
         *
         * @returns
         *   The SamplerJob::Type.
         */

        virtual Type
        getType() const = 0;

        /**
         * Gets the Zone that the Sampler will use to get information for this
         * SamplerJob.
         *
         * @returns
         *   The Zone.
         */

        virtual const Zone *
        getZone() const = 0;

    protected:

        /**
         * Constructs a new SamplerJob object.  SamplerJob objects cannot be
         * created directly.  SamplerJob objects are created by calling
         * Context::addSamplerJob().
         *
         * @param parent
         *   The parent object of the new SamplerJob.
         */

        explicit
        SamplerJob(QObject *parent=0);

        /**
         * Destroys a SamplerJob.  The application is responsible for managing
         * SamplerJob memory.
         */

        virtual
        ~SamplerJob();

    };

}

#endif
