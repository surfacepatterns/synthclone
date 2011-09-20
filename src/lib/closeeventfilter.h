#ifndef __SYNTHCLONE_CLOSEEVENTFILTER_H__
#define __SYNTHCLONE_CLOSEEVENTFILTER_H__

#include <QtCore/QEvent>
#include <QtCore/QObject>

namespace synthclone {

    class CloseEventFilter: public QObject {

        Q_OBJECT

    public:

        explicit
        CloseEventFilter(QObject *parent=0);

        ~CloseEventFilter();

        bool
        eventFilter(QObject *obj, QEvent *event);

        bool
        isCloseEnabled() const;

        void
        setCloseEnabled(bool enabled);

    signals:

        void
        closeRequest();

    private:

        bool closeEnabled;

    };

}

#endif
