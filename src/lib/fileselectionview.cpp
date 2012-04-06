/*
 * libsynthclone - a plugin API for `synthclone`
 * Copyright (C) 2011-2012 Devin Anderson
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <cassert>

#include <QtCore/QUrl>
#include <QtGui/QFileDialog>

#include <synthclone/fileselectionview.h>

using synthclone::FileSelectionView;

FileSelectionView::FileSelectionView(QObject *parent):
    View(new QFileDialog(), parent)
{
    QFileDialog *dialog = qobject_cast<QFileDialog *>(getRootWidget());

#ifdef SYNTHCLONE_PLATFORM_MACX
    // Native file dialogs on Mac OSX behave strangely.  Sometimes, they don't
    // become visible when trying to make them visible.  Sometimes, they don't
    // become modal correctly when a modal window is already present.  I don't
    // quite understand.  Maybe I'm not using the static QFileDialog functions
    // or the exec() function, which is why this bug bit me.
    dialog->setOption(QFileDialog::DontUseNativeDialog, true);
#endif

    dialog->setModal(true);
    connect(dialog, SIGNAL(accepted()), SLOT(handleDialogAccept()));
    connect(dialog, SIGNAL(rejected()), SLOT(handleDialogReject()));
}

FileSelectionView::~FileSelectionView()
{
    // Empty
}

bool
FileSelectionView::areFilesVisible() const
{
    return qobject_cast<const QFileDialog *>(getRootWidget())->
        testOption(QFileDialog::ShowDirsOnly);
}

QString
FileSelectionView::getDirectory() const
{
    return qobject_cast<const QFileDialog *>(getRootWidget())->
        directory().absolutePath();
}

FileSelectionView::Operation
FileSelectionView::getOperation() const
{
    Operation operation;
    switch (qobject_cast<const QFileDialog *>(getRootWidget())->acceptMode()) {
    case QFileDialog::AcceptOpen:
        operation = OPERATION_OPEN;
        break;
    case QFileDialog::AcceptSave:
        operation = OPERATION_SAVE;
        break;
    default:
        assert(false);
    }
    return operation;
}

FileSelectionView::SelectionFilter
FileSelectionView::getSelectionFilter() const
{
    SelectionFilter filter;
    switch (qobject_cast<const QFileDialog *>(getRootWidget())->fileMode()) {
    case QFileDialog::AnyFile:
        filter = SELECTIONFILTER_ANY_FILE;
        break;
    case QFileDialog::Directory:
        filter = SELECTIONFILTER_DIRECTORY;
        break;
    case QFileDialog::ExistingFile:
        filter = SELECTIONFILTER_EXISTING_FILE;
        break;
    case QFileDialog::ExistingFiles:
        filter = SELECTIONFILTER_EXISTING_FILES;
        break;
    default:
        assert(false);
    }
    return filter;
}

QString
FileSelectionView::getTitle() const
{
    return qobject_cast<const QFileDialog *>(getRootWidget())->windowTitle();
}

void
FileSelectionView::handleDialogAccept()
{
    QStringList paths =
        qobject_cast<const QFileDialog *>(getRootWidget())->selectedFiles();

    // Apparently, the paths can be URLs.  The Qt documentation doesn't say
    // anything about it, but it happens on Ubuntu 10.04.
    for (int i = paths.count() - 1; i >= 0; i--) {
        QUrl url(paths[i]);
        if (url.isValid() && (url.scheme() == "file")) {
            paths[i] = url.toLocalFile();
        }
    }

    // Hack: Keep the dialog visible.  The dialog wants to close without any
    // prompting, but that violates passive view design.
    setVisible(true);

    emit pathsSelected(paths);
}

void
FileSelectionView::handleDialogReject()
{
    // Hack: Keep the dialog visible.  The dialog wants to close without any
    // prompting, but that violates passive view design.
    setVisible(true);

    emit closeRequest();
}

void
FileSelectionView::setDirectory(const QString &directory)
{
    qobject_cast<QFileDialog *>(getRootWidget())->setDirectory(directory);
}

void
FileSelectionView::setFilesVisible(bool visible)
{
    QFileDialog *dialog = qobject_cast<QFileDialog *>(getRootWidget());
    if (! visible) {
        dialog->setFileMode(QFileDialog::Directory);
    }
    dialog->setOption(QFileDialog::ShowDirsOnly, ! visible);
}

void
FileSelectionView::setOperation(Operation operation)
{
    QFileDialog::AcceptMode mode;
    switch (operation) {
    case OPERATION_OPEN:
        mode = QFileDialog::AcceptOpen;
        break;
    case OPERATION_SAVE:
        mode = QFileDialog::AcceptSave;
        break;
    default:
        assert(false);
    }
    qobject_cast<QFileDialog *>(getRootWidget())->setAcceptMode(mode);
}

void
FileSelectionView::setSelectionFilter(SelectionFilter filter)
{
    QFileDialog::FileMode mode;
    switch (filter) {
    case SELECTIONFILTER_ANY_FILE:
        mode = QFileDialog::AnyFile;
        break;
    case SELECTIONFILTER_DIRECTORY:
        mode = QFileDialog::Directory;
        break;
    case SELECTIONFILTER_EXISTING_FILE:
        mode = QFileDialog::ExistingFile;
        break;
    case SELECTIONFILTER_EXISTING_FILES:
        mode = QFileDialog::ExistingFiles;
        break;
    default:
        assert(false);
    }
    qobject_cast<QFileDialog *>(getRootWidget())->setFileMode(mode);
}

void
FileSelectionView::setTitle(const QString &title)
{
    qobject_cast<QFileDialog *>(getRootWidget())->setWindowTitle(title);
}
