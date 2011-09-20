#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__

#include <synthclone/participant.h>

#include "sampler.h"
#include "sampleratechangeview.h"

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
    getState(const synthclone::Sampler *sampler) const;

    void
    restoreSampler(const QVariant &state);

private slots:

    void
    addSampler(bool convertSampleRate);

    void
    handleFatalError(const QString &message);

    void
    handleJACKSampleRateChange();

    void
    handleSamplerAdditionRequest();

    void
    handleSampleRateChangeViewChangeRequest();

    void
    handleSampleRateChangeViewCloseRequest();

    void
    handleSamplerUnregistration(QObject *obj);

    void
    handleSessionEvent(jack_client_t *client, jack_session_event_t *event);

private:

    static void
    handleError(const char *message);

    static void
    handleInfo(const char *message);

    static void
    ignoreMessage(const char *message);

    synthclone::MenuAction addSamplerAction;
    synthclone::Context *context;
    SampleRateChangeView sampleRateChangeView;
    QByteArray sessionId;

};

#endif
