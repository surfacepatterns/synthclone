#include <synthclone/error.h>
#include <synthclone/participant.h>

using synthclone::Participant;

Participant::Participant(const QString &name, int majorVersion,
                         int minorVersion, int revision, const QString &author,
                         const QString &summary, QObject *parent):
    QObject(parent)
{
    if (majorVersion < 0) {
        throw Error(tr("'%1' is not a valid major version").arg(majorVersion));
    }
    if (minorVersion < 0) {
        throw Error(tr("'%1' is not a valid minor version").arg(minorVersion));
    }
    if (name.isEmpty()) {
        throw Error(tr("the plugin name is empty"));
    }
    if (revision < 0) {
        throw Error(tr("'%1' is not a valid revision number").arg(revision));
    }
    this->author = author;
    this->majorVersion = majorVersion;
    this->minorVersion = minorVersion;
    this->name = name;
    this->revision = revision;
    this->summary = summary;
}

Participant::~Participant()
{
    // Empty
}

void
Participant::activate(Context &/*context*/, const QVariant &/*state*/)
{
    // Empty
}

void
Participant::deactivate(Context &/*context*/)
{
    // Empty
}

QString
Participant::getAuthor() const
{
    return author;
}

int
Participant::getMajorVersion() const
{
    return majorVersion;
}

int
Participant::getMinorVersion() const
{
    return minorVersion;
}

QString
Participant::getName() const
{
    return name;
}

int
Participant::getRevision() const
{
    return revision;
}

QVariant
Participant::getState() const
{
    return QVariant();
}

QVariant
Participant::getState(const Effect */*effect*/) const
{
    throw Error(tr("This plugin does not support the saving of effects"));
}

QVariant
Participant::getState(const Sampler */*sampler*/) const
{
    throw Error(tr("This plugin does not support the saving of samplers"));
}

QVariant
Participant::getState(const Target */*target*/) const
{
    throw Error(tr("This plugin does not support the saving of targets"));
}

QString
Participant::getSummary() const
{
    return summary;
}

void
Participant::restoreEffect(const QVariant &/*state*/)
{
    throw Error(tr("This plugin does not support the loading of effects"));
}

void
Participant::restoreSampler(const QVariant &/*state*/)
{
    throw Error(tr("This plugin does not support the loading of samplers"));
}

void
Participant::restoreTarget(const QVariant &/*state*/)
{
    throw Error(tr("This plugin does not support the loading of targets"));
}
