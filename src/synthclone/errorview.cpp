#include <synthclone/util.h>

#include "errorview.h"

ErrorView::ErrorView(QObject *parent):
    DialogView(":/synthclone/errorview.ui", parent)
{
    closeButton = synthclone::getChild<QPushButton>(dialog, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    message = synthclone::getChild<QLabel>(dialog, "message");
}

ErrorView::~ErrorView()
{
    // Empty
}

void
ErrorView::setMessage(const QString &message)
{
    this->message->setText(message);
}
