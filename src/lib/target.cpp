#include <synthclone/target.h>

using synthclone::Target;

Target::Target(const QString &name, QObject *parent):
    Component(name, parent)
{
    // Empty
}

Target::~Target()
{
    // Empty
}

void
Target::validate(const QList<Zone *> &/*zones*/) const
{
    // Empty
}
