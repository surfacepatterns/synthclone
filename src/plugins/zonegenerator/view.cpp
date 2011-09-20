#include <synthclone/util.h>

#include "view.h"

View::View(QObject *parent):
    DesignerView(":/synthclone/plugins/zonegenerator/view.ui", parent)
{
    QWidget *widget = getRootWidget();

    aftertouchLayers =
        synthclone::getChild<QSpinBox>(widget, "aftertouchLayers");
    connect(aftertouchLayers, SIGNAL(valueChanged(int)),
            SLOT(handleAftertouchLayersChange(int)));

    cancelButton = synthclone::getChild<QPushButton>(widget, "cancelButton");
    connect(cancelButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    firstNote = synthclone::getChild<QComboBox>(widget, "firstNote");
    connect(firstNote, SIGNAL(currentIndexChanged(int)),
            SLOT(handleFirstNoteChange(int)));

    generateButton =
        synthclone::getChild<QPushButton>(widget, "generateButton");
    connect(generateButton, SIGNAL(clicked()), SIGNAL(generateRequest()));

    lastNote = synthclone::getChild<QComboBox>(widget, "lastNote");
    connect(lastNote, SIGNAL(currentIndexChanged(int)),
            SLOT(handleLastNoteChange(int)));

    for (synthclone::MIDIData i = 0; i < 0x80; i++) {
        QString noteString = synthclone::getMIDINoteString(i);
        firstNote->addItem(noteString);
        lastNote->addItem(noteString);
    }

    lastNoteLabel = synthclone::getChild<QLabel>(widget, "lastNoteLabel");

    midiChannel = synthclone::getChild<QSpinBox>(widget, "midiChannel");
    connect(midiChannel, SIGNAL(valueChanged(int)),
            SLOT(handleMIDIChannelChange(int)));

    releaseTime = synthclone::getChild<QDoubleSpinBox>(widget, "releaseTime");
    connect(releaseTime, SIGNAL(valueChanged(double)),
            SLOT(handleReleaseTimeChange(double)));

    sampleTime = synthclone::getChild<QDoubleSpinBox>(widget, "sampleTime");
    connect(sampleTime, SIGNAL(valueChanged(double)),
            SLOT(handleSampleTimeChange(double)));

    totalNotes = synthclone::getChild<QSpinBox>(widget, "totalNotes");
    connect(totalNotes, SIGNAL(valueChanged(int)),
            SLOT(handleTotalNotesChange(int)));

    velocityLayers = synthclone::getChild<QSpinBox>(widget, "velocityLayers");
    connect(velocityLayers, SIGNAL(valueChanged(int)),
            SLOT(handleVelocityLayersChange(int)));
}

View::~View()
{
    // Empty
}

void
View::handleAftertouchLayersChange(int layers)
{
    emit aftertouchLayersChanged(static_cast<synthclone::MIDIData>(layers));
}

void
View::handleFirstNoteChange(int firstNote)
{
    emit firstNoteChanged(static_cast<synthclone::MIDIData>(firstNote));
}

void
View::handleLastNoteChange(int lastNote)
{
    emit lastNoteChanged(static_cast<synthclone::MIDIData>(lastNote));
}

void
View::handleMIDIChannelChange(int midiChannel)
{
    emit midiChannelChanged(static_cast<synthclone::MIDIData>(midiChannel));
}

void
View::handleReleaseTimeChange(double releaseTime)
{
    emit releaseTimeChanged(static_cast<synthclone::SampleTime>(releaseTime));
}

void
View::handleSampleTimeChange(double sampleTime)
{
    emit sampleTimeChanged(static_cast<synthclone::SampleTime>(sampleTime));
}

void
View::handleTotalNotesChange(int totalNotes)
{
    emit totalNotesChanged(static_cast<synthclone::MIDIData>(totalNotes));
}

void
View::handleVelocityLayersChange(int layers)
{
    emit velocityLayersChanged(static_cast<synthclone::MIDIData>(layers));
}

void
View::setAftertouchLayers(synthclone::MIDIData layers)
{
    this->aftertouchLayers->setValue(layers);
}

void
View::setFirstNote(synthclone::MIDIData firstNote)
{
    this->firstNote->setCurrentIndex(static_cast<int>(firstNote));
}

void
View::setLastNote(synthclone::MIDIData lastNote)
{
    this->lastNote->setCurrentIndex(static_cast<int>(lastNote));
}

void
View::setLastNoteEditingEnabled(bool lastNoteEditingEnabled)
{
    lastNote->setEnabled(lastNoteEditingEnabled);
    lastNoteLabel->setEnabled(lastNoteEditingEnabled);
}

void
View::setMIDIChannel(synthclone::MIDIData midiChannel)
{
    this->midiChannel->setValue(static_cast<int>(midiChannel));
}

void
View::setReleaseTime(synthclone::SampleTime releaseTime)
{
    this->releaseTime->setValue(static_cast<double>(releaseTime));
}

void
View::setSampleTime(synthclone::SampleTime sampleTime)
{
    this->sampleTime->setValue(static_cast<double>(sampleTime));
}

void
View::setTotalNotes(synthclone::MIDIData totalNotes)
{
    this->totalNotes->setValue(static_cast<int>(totalNotes));
}

void
View::setVelocityLayers(synthclone::MIDIData layers)
{
    this->velocityLayers->setValue(static_cast<int>(layers));
}
