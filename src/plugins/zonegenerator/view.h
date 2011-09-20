#ifndef __VIEW_H__
#define __VIEW_H__

#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

#include <synthclone/designerview.h>
#include <synthclone/types.h>

class View: public synthclone::DesignerView {

    Q_OBJECT

public:

    explicit
    View(QObject *parent=0);

    ~View();

public slots:

    void
    setAftertouchLayers(synthclone::MIDIData layers);

    void
    setFirstNote(synthclone::MIDIData firstNote);

    void
    setLastNote(synthclone::MIDIData lastNote);

    void
    setLastNoteEditingEnabled(bool lastNoteEditingEnabled);

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
    generateRequest();

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

private slots:

    void
    handleAftertouchLayersChange(int layers);

    void
    handleFirstNoteChange(int firstNoteIndex);

    void
    handleLastNoteChange(int lastNoteIndex);

    void
    handleMIDIChannelChange(int midiChannel);

    void
    handleReleaseTimeChange(double releaseTime);

    void
    handleSampleTimeChange(double sampleTime);

    void
    handleTotalNotesChange(int totalNotes);

    void
    handleVelocityLayersChange(int layers);

private:

    QSpinBox *aftertouchLayers;
    QPushButton *cancelButton;
    QComboBox *firstNote;
    QPushButton *generateButton;
    QComboBox *lastNote;
    QLabel *lastNoteLabel;
    QSpinBox *midiChannel;
    QDoubleSpinBox *releaseTime;
    QDoubleSpinBox *sampleTime;
    QSpinBox *totalNotes;
    QSpinBox *velocityLayers;

};

#endif
