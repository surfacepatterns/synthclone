#include <synthclone/effect.h>

using synthclone::Effect;

Effect::Effect(const QString &name, QObject *parent):
    Component(name, parent)
{
    // Empty
}

Effect::~Effect()
{
    // Empty
}
