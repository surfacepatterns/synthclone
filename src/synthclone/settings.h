#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <QtCore/QDir>
#include <QtCore/QSettings>

#include "session.h"

class Settings: public QObject {

    Q_OBJECT

public:

    explicit
    Settings(Session &session, QObject *parent=0);

    ~Settings();

    void
    addPluginPath(const QString &path);

    QStringList
    getPluginPaths();

    QStringList
    getRecentSessionPaths();

    void
    removePluginPath(const QString &path);

private slots:

    void
    handleStateChange(synthclone::SessionState state, const QDir *directory);

private:

    QVariant
    read(const QString &key, const QVariant &defaultValue=QVariant());

    void
    verifyReadStatus() const;

    void
    write(const QString &key, const QVariant &value);

    QSettings settings;

};

#endif
