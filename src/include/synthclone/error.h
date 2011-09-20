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
