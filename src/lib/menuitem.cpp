#include <synthclone/menuitem.h>

using synthclone::MenuItem;

MenuItem::MenuItem(QObject *parent):
    QObject(parent)
{
    visible = true;
}

MenuItem::~MenuItem()
{
    // Empty
}

bool
MenuItem::isVisible() const
{
    return visible;
}

void
MenuItem::setVisible(bool visible)
{
    if (visible != this->visible) {
        this->visible = visible;
        emit visibilityChanged(visible);
    }
}
