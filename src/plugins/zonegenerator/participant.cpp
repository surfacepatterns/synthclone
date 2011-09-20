#include <cmath>

#include "participant.h"

Participant::Participant(QObject *parent):
    synthclone::Participant(tr("Zone Generator"), 0, 0, 1, "Devin Anderson",
                            tr("Assists in generating ranged zone data"),
                            parent),
    generatorAction(tr("Zone Generator"))
{
    connect(&generatorAction, SIGNAL(triggered()),
            SLOT(handleViewShow()));

    connect(&view, SIGNAL(aftertouchLayersChanged(synthclone::MIDIData)),
            &data, SLOT(setAftertouchLayers(synthclone::MIDIData)));
    connect(&view, SIGNAL(firstNoteChanged(synthclone::MIDIData)),
            &data, SLOT(setFirstNote(synthclone::MIDIData)));
    connect(&view, SIGNAL(lastNoteChanged(synthclone::MIDIData)),
            &data, SLOT(setLastNote(synthclone::MIDIData)));
    connect(&view, SIGNAL(midiChannelChanged(synthclone::MIDIData)),
            &data, SLOT(setMIDIChannel(synthclone::MIDIData)));
    connect(&view, SIGNAL(releaseTimeChanged(synthclone::SampleTime)),
            &data, SLOT(setReleaseTime(synthclone::SampleTime)));
    connect(&view, SIGNAL(sampleTimeChanged(synthclone::SampleTime)),
            &data, SLOT(setSampleTime(synthclone::SampleTime)));
    connect(&view, SIGNAL(totalNotesChanged(synthclone::MIDIData)),
            &data, SLOT(setTotalNotes(synthclone::MIDIData)));
    connect(&view, SIGNAL(velocityLayersChanged(synthclone::MIDIData)),
            &data, SLOT(setVelocityLayers(synthclone::MIDIData)));

    connect(&view, SIGNAL(closeRequest()), SLOT(handleCloseRequest()));
    connect(&view, SIGNAL(generateRequest()), SLOT(handleGenerateRequest()));

    connect(&data, SIGNAL(aftertouchLayersChanged(synthclone::MIDIData)),
            &view, SLOT(setAftertouchLayers(synthclone::MIDIData)));
    connect(&data, SIGNAL(firstNoteChanged(synthclone::MIDIData)),
            &view, SLOT(setFirstNote(synthclone::MIDIData)));
    connect(&data, SIGNAL(lastNoteChanged(synthclone::MIDIData)),
            &view, SLOT(setLastNote(synthclone::MIDIData)));
    connect(&data, SIGNAL(midiChannelChanged(synthclone::MIDIData)),
            &view, SLOT(setMIDIChannel(synthclone::MIDIData)));
    connect(&data, SIGNAL(releaseTimeChanged(synthclone::SampleTime)),
            &view, SLOT(setReleaseTime(synthclone::SampleTime)));
    connect(&data, SIGNAL(sampleTimeChanged(synthclone::SampleTime)),
            &view, SLOT(setSampleTime(synthclone::SampleTime)));
    connect(&data, SIGNAL(velocityLayersChanged(synthclone::MIDIData)),
            &view, SLOT(setVelocityLayers(synthclone::MIDIData)));

    connect(&data, SIGNAL(totalNotesChanged(synthclone::MIDIData)),
            SLOT(handleTotalNotesChange(synthclone::MIDIData)));

    context = 0;
}

Participant::~Participant()
{
    // Empty
}

void
Participant::activate(synthclone::Context &context, const QVariant &/*state*/)
{
    data.reset();
    synthclone::MIDIData totalNotes = data.getTotalNotes();
    view.setAftertouchLayers(data.getAftertouchLayers());
    view.setFirstNote(data.getFirstNote());
    view.setLastNote(data.getLastNote());
    view.setLastNoteEditingEnabled(totalNotes != 1);
    view.setMIDIChannel(data.getMIDIChannel());
    view.setReleaseTime(data.getReleaseTime());
    view.setSampleTime(data.getSampleTime());
    view.setTotalNotes(totalNotes);
    view.setVelocityLayers(data.getVelocityLayers());
    context.addMenuAction(&generatorAction, synthclone::MENU_TOOLS);
    this->context = &context;
}

void
Participant::deactivate(synthclone::Context &/*context*/)
{
    this->context = 0;
}

void
Participant::handleCloseRequest()
{
    view.setVisible(false);
}

void
Participant::handleGenerateRequest()
{
    view.setVisible(false);
    int insertIndex = context->getSelectedZoneCount() ?
        context->getZoneIndex(context->getSelectedZone(0)) :
        context->getZoneCount();
    synthclone::MIDIData aftertouchLayers = data.getAftertouchLayers();
    synthclone::MIDIData firstNote = data.getFirstNote();
    synthclone::MIDIData midiChannel = data.getMIDIChannel();
    synthclone::MIDIData noteDifference = data.getLastNote() - firstNote;
    synthclone::SampleTime releaseTime = data.getReleaseTime();
    synthclone::SampleTime sampleTime = data.getSampleTime();
    synthclone::MIDIData totalNotes = data.getTotalNotes();
    synthclone::MIDIData velocityLayers = data.getVelocityLayers();
    for (int noteIndex = static_cast<int>(totalNotes - 1); noteIndex >= 0;
         noteIndex--) {
        synthclone::MIDIData note = static_cast<synthclone::MIDIData>
            (std::ceil(firstNote + (noteDifference *
                                    (static_cast<float>(noteIndex) /
                                     (totalNotes - 1)))));
        for (synthclone::MIDIData velocityIndex = velocityLayers;
             velocityIndex > 0; velocityIndex--) {
            synthclone::MIDIData velocity = static_cast<synthclone::MIDIData>
                (std::ceil(0x7f * (static_cast<float>(velocityIndex) /
                                   velocityLayers)));

            // Generate zones with aftertouch (if requested).
            synthclone::Zone *zone;
            for (synthclone::MIDIData aftertouchIndex = aftertouchLayers;
                 aftertouchIndex > 0; aftertouchIndex--) {
                synthclone::MIDIData aftertouch =
                    static_cast<synthclone::MIDIData>
                    (std::ceil(0x7f * (static_cast<float>(aftertouchIndex) /
                                       aftertouchLayers)));
                zone = context->addZone(insertIndex);
                zone->setAftertouch(aftertouch);
                zone->setChannel(midiChannel);
                zone->setNote(note);
                zone->setReleaseTime(releaseTime);
                zone->setSampleTime(sampleTime);
                zone->setVelocity(velocity);
            }

            // Generate zone without aftertouch.
            zone = context->addZone(insertIndex);
            zone->setChannel(midiChannel);
            zone->setNote(note);
            zone->setReleaseTime(releaseTime);
            zone->setSampleTime(sampleTime);
            zone->setVelocity(velocity);
        }
    }
}

void
Participant::handleTotalNotesChange(synthclone::MIDIData totalNotes)
{
    view.setTotalNotes(totalNotes);
    view.setLastNoteEditingEnabled(totalNotes != 1);
}

void
Participant::handleViewShow()
{
    view.setVisible(true);
}
