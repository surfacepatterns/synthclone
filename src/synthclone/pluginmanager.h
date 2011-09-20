#ifndef __PLUGINMANAGER_H__
#define __PLUGINMANAGER_H__

#include <QtCore/QList>
#include <QtCore/QMap>

#include <synthclone/iplugin.h>

class PluginManager: public QObject {

    Q_OBJECT

public:

    explicit
    PluginManager(QObject *parent=0);

    ~PluginManager();

    synthclone::IPlugin *
    loadPlugin(const QString &path);

    void
    unloadPlugin(synthclone::IPlugin *plugin);

private:

    QList<synthclone::IPlugin *> plugins;

};

#endif
