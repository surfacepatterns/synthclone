#include <cassert>

#include <QtGui/QLayout>

#include "dialogview.h"

DialogView::DialogView(const QString &path, QObject *parent):
    synthclone::DesignerView(path, parent)
{
    dialog = qobject_cast<QDialog *>(getRootWidget());
    assert(dialog);

    // XX: If the following line is uncommented, the size of the dialog set in
    // the .ui file changes.
    // dialog->layout()->setSizeConstraint(QLayout::SetFixedSize);

    dialog->setWindowFlags(Qt::CustomizeWindowHint | Qt::Dialog |
                           Qt::WindowCloseButtonHint);
    dialog->setModal(true);
}

DialogView::~DialogView()
{
    // Empty
}
