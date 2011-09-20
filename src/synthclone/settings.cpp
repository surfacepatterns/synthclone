#include <cassert>

#include <synthclone/error.h>

#include "settings.h"

Settings::Settings(Session &session, QObject *parent):
    QObject(parent),
    settings("synthclone.googlecode.com", "synthclone")
{
    verifyReadStatus();
    settings.beginGroup("synthclone");
    connect(&session,
            SIGNAL(stateChanged(synthclone::SessionState, const QDir *)),
            SLOT(handleStateChange(synthclone::SessionState, const QDir *)));
}

Settings::~Settings()
{
    settings.endGroup();
    settings.sync();
}

void
Settings::addPluginPath(const QString &path)
{
    QDir directory(path);
    QString pluginPath = directory.absolutePath();
    QStringList pluginPaths = getPluginPaths();
    if (! pluginPaths.contains(pluginPath)) {
        pluginPaths.append(pluginPath);
        write("pluginPaths", pluginPaths);
    }
}

QStringList
Settings::getPluginPaths()
{
    return read("pluginPaths").toStringList();
}

QStringList
Settings::getRecentSessionPaths()
{
    return read("recentSessionPaths").toStringList();
}

void
Settings::handleStateChange(synthclone::SessionState state,
                            const QDir *directory)
{
    if ((state == synthclone::SESSIONSTATE_CURRENT) && directory) {
        QString path = directory->absolutePath();
        QStringList sessionPaths = getRecentSessionPaths();
        if (sessionPaths.contains(path)) {
            sessionPaths.removeOne(path);
        }
        sessionPaths.prepend(path);
        write("recentSessionPaths", sessionPaths);
    }
}

QVariant
Settings::read(const QString &key, const QVariant &defaultValue)
{
    settings.sync();
    QVariant value = settings.value(key, defaultValue);
    verifyReadStatus();
    return value;
}

void
Settings::removePluginPath(const QString &path)
{
    QString pluginPath = QDir(path).absolutePath();
    QStringList pluginPaths = getPluginPaths();
    if (pluginPaths.contains(pluginPath)) {
        pluginPaths.removeOne(pluginPath);
        write("pluginPaths", pluginPaths);
    }
}

void
Settings::verifyReadStatus() const
{
    switch (settings.status()) {
    case QSettings::AccessError:
        throw synthclone::Error(tr("Failed to read application settings"));
    case QSettings::FormatError:
        throw synthclone::Error(tr("Settings storage is corrupted"));
    default:
        ;
    }
}

void
Settings::write(const QString &key, const QVariant &value)
{
    settings.setValue(key, value);
    settings.sync();
    switch (settings.status()) {
    case QSettings::AccessError:
        throw synthclone::Error(tr("Failed to write application settings"));
    case QSettings::FormatError:
        // This shouldn't happen.
        assert(false);
    default:
        ;
    }
}
