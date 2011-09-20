#include <synthclone/sampler.h>

using synthclone::Sampler;

Sampler::Sampler(const QString &name, QObject *parent):
    Component(name, parent)
{
    // Empty
}

Sampler::~Sampler()
{
    // Empty
}
