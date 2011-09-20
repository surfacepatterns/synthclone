#ifndef __SYNTHCLONE_MENUSEPARATOR_H__
#define __SYNTHCLONE_MENUSEPARATOR_H__

#include <synthclone/menuitem.h>

namespace synthclone {

    /**
     * MenuSeparator objects are used to create a sense of grouping with
     * MenuAction objects.
     */

    class MenuSeparator: public MenuItem {

        Q_OBJECT

    public:

        /**
         * Creates a MenuSeparator object.
         *
         * @param parent
         *   The parent object of the new menu separator.
         */

        explicit
        MenuSeparator(QObject *parent=0);

        /**
         * Destroys the MenuSeparator object.
         */

        ~MenuSeparator();

    };

}

#endif
