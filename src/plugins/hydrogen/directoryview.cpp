#include <cassert>

#include <QtCore/QUrl>

#include "directoryview.h"

DirectoryView::DirectoryView(QObject *parent):
    synthclone::View(new QFileDialog(), parent)
{
    QFileDialog *dialog = qobject_cast<QFileDialog *>(getRootWidget());
    connect(dialog, SIGNAL(accepted()), SLOT(handleDialogAccept()));
    connect(dialog, SIGNAL(rejected()), SIGNAL(closeRequest()));

    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::Dialog |
                           Qt::WindowCloseButtonHint);
    dialog->setModal(true);
    dialog->setFileMode(QFileDialog::Directory);
    dialog->setOption(QFileDialog::ShowDirsOnly, true);
}

DirectoryView::~DirectoryView()
{
    // Empty
}

void
DirectoryView::handleDialogAccept()
{
    QStringList paths =
        qobject_cast<QFileDialog *>(getRootWidget())->selectedFiles();
    assert(paths.count() == 1);
    QString path = paths[0];

    // Apparently, the current path can be a URL.  The Qt documentation doesn't
    // say anything about it, but it happens on Ubuntu 10.04.
    QUrl url(path);
    if (url.isValid() && (url.scheme() == "file")) {
        path = url.toLocalFile();
    }
    emit pathSelected(path);

    // Hack: The dialog emits a hide event instead of a close event.
    setVisible(true);
    emit closeRequest();
}

void
DirectoryView::setPath(const QString &path)
{
    qobject_cast<QFileDialog *>(getRootWidget())->setDirectory(path);
}
