#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <synthclone/iplugin.h>

#include "participant.h"

class Plugin: public QObject, public synthclone::IPlugin {

    Q_OBJECT
    Q_INTERFACES(synthclone::IPlugin)

public:

    explicit
    Plugin(QObject *parent=0);

    ~Plugin();

    QByteArray
    getId() const;

    synthclone::Participant *
    getParticipant();

private:

    Participant participant;

};

#endif
