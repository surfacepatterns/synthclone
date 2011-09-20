#include <cassert>

#include <QtCore/QPluginLoader>

#include <synthclone/error.h>

#include "pluginmanager.h"

PluginManager::PluginManager(QObject *parent):
    QObject(parent)
{
    // Empty
}

PluginManager::~PluginManager()
{
    for (int i = plugins.count() - 1; i >= 0; i--) {
        unloadPlugin(plugins[i]);
    }
}

synthclone::IPlugin *
PluginManager::loadPlugin(const QString &path)
{
    QString pluginPath = QFileInfo(path).absoluteFilePath();
    QPluginLoader pluginLoader(pluginPath);
    QObject *obj = pluginLoader.instance();
    if (! obj) {
        throw synthclone::Error(pluginLoader.errorString());
    }
    synthclone::IPlugin *plugin = qobject_cast<synthclone::IPlugin *>(obj);
    if (! plugin) {
        pluginLoader.unload();
        QString message = tr("'%1' does not contain a synthclone plugin").
            arg(pluginPath);
        throw synthclone::Error(message);
    }
    if (! plugins.contains(plugin)) {
        plugins.append(plugin);
    }
    return plugin;
}

void
PluginManager::unloadPlugin(synthclone::IPlugin *plugin)
{
    bool removed = plugins.removeOne(plugin);
    assert(removed);
    delete plugin;
}
