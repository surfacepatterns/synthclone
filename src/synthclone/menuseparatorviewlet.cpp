#include "menuseparatorviewlet.h"

MenuSeparatorViewlet::MenuSeparatorViewlet(QObject *parent):
    MenuLeafViewlet(parent)
{
    getAction()->setSeparator(true);
}

MenuSeparatorViewlet::~MenuSeparatorViewlet()
{
    // Empty
}
