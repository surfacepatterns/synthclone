#include <synthclone/designerview.h>
#include <synthclone/util.h>

using synthclone::DesignerView;

DesignerView::DesignerView(const QString &path, QObject *parent):
    View(loadForm(path), parent)
{
    // Empty
}

DesignerView::~DesignerView()
{
    // Empty
}
