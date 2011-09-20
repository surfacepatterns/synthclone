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
         * Constructs a new DesignerView.  This constructor should not be called
         * directly; instead, subclass constructors should call this
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
