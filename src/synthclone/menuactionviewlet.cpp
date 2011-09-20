#include "menuactionviewlet.h"

MenuActionViewlet::MenuActionViewlet(QObject *parent):
    MenuLeafViewlet(parent)
{
    connect(getAction(), SIGNAL(triggered()), SIGNAL(triggered()));
}

MenuActionViewlet::~MenuActionViewlet()
{
    // Empty
}

void
MenuActionViewlet::setDescription(const QString &description)
{
    getAction()->setStatusTip(description);
}

void
MenuActionViewlet::setEnabled(bool enabled)
{
    getAction()->setEnabled(enabled);
}

void
MenuActionViewlet::setText(const QString &text)
{
    getAction()->setText(text);
}
