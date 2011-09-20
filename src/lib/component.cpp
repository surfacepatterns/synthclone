#include <synthclone/component.h>

using synthclone::Component;

Component::Component(const QString &name, QObject *parent):
    QObject(parent)
{
    this->name = name;
}

Component::~Component()
{
    // Empty
}

QString
Component::getName() const
{
    return name;
}

void
Component::setName(const QString &name)
{
    if (this->name != name) {
        this->name = name;
        emit nameChanged(name);
    }
}
