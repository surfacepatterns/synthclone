export module synthclone.qt:qwidget;

import synthclone.external.qt.widgets;
import synthclone.util;

///////////////////////////////////////////////////////////////////////////////
// synthclone::qwidget_visibility_guard
///////////////////////////////////////////////////////////////////////////////

namespace synthclone {

    bool
    update_visibility(::QWidget* widget, bool visible)
    {
        verify(widget != nullptr, "widget is set to NULL");

        auto old_flag = widget->isVisible();
        widget->setVisible(visible);
        return old_flag;
    }

    export
    class qwidget_visibility_guard final: private nonmovable {

    public:

        inline explicit
        qwidget_visibility_guard(::QWidget* widget, bool visible = true):
            widget_(widget),
            visible_(update_visibility(widget, visible))
        {
            // empty
        }

        inline
        ~qwidget_visibility_guard()
        {
            widget_->setVisible(visible_);
        }

    private:

        ::QWidget* widget_;
        bool visible_;

    };

}
