#ifndef __DATA_H__
#define __DATA_H__

#include <QtCore/QObject>

#include <synthclone/types.h>

class Data: public QObject {

    Q_OBJECT

public:

    explicit
    Data(QObject *parent=0);

    ~Data();

    synthclone::MIDIData
    getAftertouchLayers() const;

    synthclone::MIDIData
    getFirstNote() const;

    synthclone::MIDIData
    getLastNote() const;

    synthclone::MIDIData
    getMIDIChannel() const;

    synthclone::SampleTime
    getReleaseTime() const;

    synthclone::SampleTime
    getSampleTime() const;

    synthclone::MIDIData
    getTotalNotes() const;

    synthclone::MIDIData
    getVelocityLayers() const;

public slots:

    void
    reset();

    void
    setAftertouchLayers(synthclone::MIDIData layers);

    void
    setFirstNote(synthclone::MIDIData firstNote);

    void
    setLastNote(synthclone::MIDIData lastNote);

    void
    setMIDIChannel(synthclone::MIDIData midiChannel);

    void
    setReleaseTime(synthclone::SampleTime releaseTime);

    void
    setSampleTime(synthclone::SampleTime sampleTime);

    void
    setTotalNotes(synthclone::MIDIData totalNotes);

    void
    setVelocityLayers(synthclone::MIDIData layers);

signals:

    void
    aftertouchLayersChanged(synthclone::MIDIData layers);

    void
    firstNoteChanged(synthclone::MIDIData firstNote);

    void
    lastNoteChanged(synthclone::MIDIData lastNote);

    void
    midiChannelChanged(synthclone::MIDIData midiChannel);

    void
    releaseTimeChanged(synthclone::SampleTime releaseTime);

    void
    sampleTimeChanged(synthclone::SampleTime sampleTime);

    void
    totalNotesChanged(synthclone::MIDIData totalNotes);

    void
    velocityLayersChanged(synthclone::MIDIData layers);

private:

    void
    updateTotalNotes();

    synthclone::MIDIData aftertouchLayers;
    synthclone::MIDIData firstNote;
    synthclone::MIDIData lastNote;
    synthclone::MIDIData midiChannel;
    synthclone::SampleTime releaseTime;
    synthclone::SampleTime sampleTime;
    synthclone::MIDIData totalNotes;
    synthclone::MIDIData velocityLayers;

};

#endif
