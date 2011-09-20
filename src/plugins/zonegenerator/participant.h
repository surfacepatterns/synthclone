#ifndef __PARTICIPANT_H__
#define __PARTICIPANT_H__

#include <synthclone/participant.h>

#include "data.h"
#include "view.h"

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

private slots:

    void
    handleCloseRequest();

    void
    handleGenerateRequest();

    void
    handleTotalNotesChange(synthclone::MIDIData totalNotes);

    void
    handleViewShow();

private:

    synthclone::Context *context;
    Data data;
    synthclone::MenuAction generatorAction;
    View view;

};

#endif
