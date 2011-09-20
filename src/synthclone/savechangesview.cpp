#include <synthclone/util.h>

#include "savechangesview.h"

SaveChangesView::SaveChangesView(QObject *parent):
    DialogView(":/synthclone/savechangesview.ui", parent)
{
    cancelButton = synthclone::getChild<QPushButton>(dialog, "cancelButton");
    connect(cancelButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    discardButton = synthclone::getChild<QPushButton>(dialog, "discardButton");
    connect(discardButton, SIGNAL(clicked()), SIGNAL(discardRequest()));

    saveButton = synthclone::getChild<QPushButton>(dialog, "saveButton");
    connect(saveButton, SIGNAL(clicked()), SIGNAL(saveRequest()));
}

SaveChangesView::~SaveChangesView()
{
    // Empty
}
