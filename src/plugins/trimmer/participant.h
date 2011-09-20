#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__

#include <synthclone/participant.h>

#include "effect.h"
#include "effectview.h"

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
    getState(const synthclone::Effect *effect) const;

    void
    restoreEffect(const QVariant &state);

private slots:

    void
    handleConfiguredEffectTrimEndChange(bool trimEnd);

    void
    handleConfiguredEffectTrimStartChange(bool trimStart);

    void
    handleEffectAddition();

    void
    handleEffectConfiguration();

    void
    handleEffectUnregistration(QObject *obj);

    void
    handleEffectViewCloseRequest();

private:

    Effect *
    addEffect();

    void
    configureEffect(Effect *effect);

    synthclone::MenuAction addEffectAction;
    synthclone::Context *context;
    Effect *configuredEffect;
    EffectView effectView;

};

#endif
