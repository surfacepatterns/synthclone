#include <synthclone/util.h>

#include "savewarningview.h"

SaveWarningView::SaveWarningView(QObject *parent):
    DialogView(":/synthclone/savewarningview.ui", parent)
{
    cancelButton = synthclone::getChild<QPushButton>(dialog, "cancelButton");
    connect(cancelButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    saveButton = synthclone::getChild<QPushButton>(dialog, "saveButton");
    connect(saveButton, SIGNAL(clicked()), SIGNAL(saveRequest()));
}

SaveWarningView::~SaveWarningView()
{
    // Empty
}
