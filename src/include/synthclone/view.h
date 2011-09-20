#ifndef __SYNTHCLONE_VIEW_H__
#define __SYNTHCLONE_VIEW_H__

#include <QtGui/QWidget>

namespace synthclone {

    class CloseEventFilter;

    /**
     * Utility view class.  Captures any close event and emits a signal instead
     * of requiring the subclassing of a widget.
     */

    class View: public QObject {

        Q_OBJECT

    public:

        /**
         * Gets a boolean indicating whether or not closing of this view is
         * enabled.
         *
         * @returns
         *   The boolean.
         */

        bool
        isCloseEnabled() const;

        /**
         * Gets a boolean indicating whether or not the view is visible.
         *
         * @returns
         *   The boolean.
         */

        bool
        isVisible() const;

    public slots:

        /**
         * Enables/disables closing for this view.  If closing is disabled,
         * closeRequest() signals will not be raised when a user presses the
         * close button in the taskbar.  Closing is enabled by default.
         *
         * @param enabled
         *   Whether or not closing is enabled.
         */

        virtual void
        setCloseEnabled(bool enabled);

        /**
         * Sets the visibility of the root widget.
         *
         * @param visible
         *   Whether or not the root widget should be visible.
         */

        virtual void
        setVisible(bool visible);

    signals:

        /**
         * Emitted when closing is enabled/disabled.
         *
         * @param enabled
         *   Whether or not closing is enabled.
         */

        void
        closeEnabledChanged(bool enabled);

        /**
         * Emitted when a root widget's close button is pressed.
         */

        void
        closeRequest();

        /**
         * Emitted when the visibility flag for the view changes.
         *
         * @param visible
         *   The updated visibility flag.
         */

        void
        visibilityChanged(bool visible);

    protected:

        /**
         * Constructs a new View.  This class is not meant to be called
         * directly; instead, subclass constructors should call this
         * constructor.
         *
         * @param rootWidget
         *   The root widget for this View.
         *
         * @param parent
         *   The parent object of this View.
         */

        explicit
        View(QWidget *rootWidget, QObject *parent=0);

        /**
         * Destroys the View object.
         */

        virtual
        ~View();

        /**
         * Gets the root widget for this view.
         *
         * @returns
         *   The root widget.
         */

        QWidget *
        getRootWidget();

    private:

        CloseEventFilter *closeEventFilter;
        QWidget *rootWidget;

    };

}

#endif
