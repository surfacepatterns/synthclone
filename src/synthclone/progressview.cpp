#include <cassert>

#include <synthclone/util.h>

#include "progressview.h"

ProgressView::ProgressView(QObject *parent):
    DialogView(":/synthclone/progressview.ui", parent)
{
    closeButton = synthclone::getChild<QPushButton>(dialog, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));
    connect(this, SIGNAL(closeEnabledChanged(bool)),
            closeButton, SLOT(setEnabled(bool)));

    messages = synthclone::getChild<QPlainTextEdit>(dialog, "messages");

    progressBar = synthclone::getChild<QProgressBar>(dialog, "progressBar");

    status = synthclone::getChild<QLabel>(dialog, "status");
}

ProgressView::~ProgressView()
{
    // Empty
}

void
ProgressView::addMessage(const QString &message)
{
    messages->appendPlainText(message);
}

void
ProgressView::clearMessages()
{
    messages->clear();
}

void
ProgressView::setProgress(float progress)
{
    assert((progress >= 0.0) && (progress <= 1.0));
    progressBar->setValue(static_cast<int>(progress * 10000.0));
}

void
ProgressView::setStatus(const QString &status)
{
    QLabel *label = this->status;
    label->setText(label->fontMetrics().elidedText(status, Qt::ElideRight,
                                                   label->width()));
}
