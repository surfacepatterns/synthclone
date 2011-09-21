#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__

#include <QtCore/QMap>

#include <synthclone/participant.h>

#include "directoryview.h"
#include "target.h"
#include "targetview.h"

class Participant: public synthclone::Participant {

    Q_OBJECT

public:

    explicit
    Participant(QObject *parent=0);

    ~Participant();

    void
    activate(synthclone::Context &context, const QVariant &state=QVariant());

    void
    deactivate(synthclone::Context &context);

    QVariant
    getState(const synthclone::Target *target) const;

    void
    restoreTarget(const QVariant &state);

private slots:

    void
    handleDirectoryViewCloseRequest();

    void
    handleTargetAddition();

    void
    handleTargetConfiguration();

    void
    handleTargetViewCloseRequest();

    void
    handleTargetViewPathLookupRequest();

private:

    Target *
    addTarget();

    void
    configureTarget(Target *target);

    synthclone::MenuAction addTargetAction;
    Target *configuredTarget;
    synthclone::Context *context;
    DirectoryView directoryView;
    TargetView targetView;

};

#endif
