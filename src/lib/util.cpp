#include <cstdlib>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QLocale>
#include <QtUiTools/QUiLoader>

#include <synthclone/util.h>

void
synthclone::_die(const char *path, const char *func, int line,
                 const QString &message)
{
    qCritical() << qApp->tr("Fatal error: %1\n\tFile: %2\n\tFunction: "
                            "%3\n\tLine: %4\n").
        arg(message, path, func, QLocale::system().toString(line));
    abort();
}

QString
synthclone::getMIDIControlString(MIDIData control)
{
    CONFIRM(control < 0x80,
            qApp->tr("'%1': invalid control index").arg(control));
    QString name;
    switch (control) {
    case 0x0:
        name = qApp->tr("Bank Select");
        break;
    case 0x1:
        name = qApp->tr("Modulation Wheel");
        break;
    case 0x2:
        name = qApp->tr("Breath Controller");
        break;
    case 0x4:
        name = qApp->tr("Foot Controller");
        break;
    case 0x5:
        name = qApp->tr("Portamento Time");
        break;
    case 0x6:
        name = qApp->tr("Data Entry MSB");
        break;
    case 0x7:
        name = qApp->tr("Channel Volume");
        break;
    case 0x8:
        name = qApp->tr("Balance");
        break;
    case 0xa:
        name = qApp->tr("Pan");
        break;
    case 0xb:
        name = qApp->tr("Expression Controller");
        break;
    case 0xc:
        name = qApp->tr("Effect Control 1");
        break;
    case 0xd:
        name = qApp->tr("Effect Control 2");
        break;
    case 0x10:
        name = qApp->tr("General Purpose Controller 1");
        break;
    case 0x11:
        name = qApp->tr("General Purpose Controller 2");
        break;
    case 0x12:
        name = qApp->tr("General Purpose Controller 3");
        break;
    case 0x13:
        name = qApp->tr("General Purpose Controller 4");
        break;
    case 0x20:
        name = qApp->tr("Bank Select LSB");
        break;
    case 0x21:
        name = qApp->tr("Modulation Wheel LSB");
        break;
    case 0x22:
        name = qApp->tr("Breath Controller LSB");
        break;
    case 0x23:
        name = qApp->tr("Control 3 LSB");
        break;
    case 0x24:
        name = qApp->tr("Foot Controller LSB");
        break;
    case 0x25:
        name = qApp->tr("Portamento Time LSB");
        break;
    case 0x26:
        name = qApp->tr("Data Entry LSB");
        break;
    case 0x27:
        name = qApp->tr("Channel Volume LSB");
        break;
    case 0x28:
        name = qApp->tr("Balance LSB");
        break;
    case 0x29:
        name = qApp->tr("Control 9 LSB");
        break;
    case 0x2a:
        name = qApp->tr("Pan LSB");
        break;
    case 0x2b:
        name = qApp->tr("Expression Controller LSB");
        break;
    case 0x2c:
        name = qApp->tr("Effect Control 1 LSB");
        break;
    case 0x2d:
        name = qApp->tr("Effect Control 2 LSB");
        break;
    case 0x2e:
        name = qApp->tr("Control 14 LSB");
        break;
    case 0x2f:
        name = qApp->tr("Control 15 LSB");
        break;
    case 0x30:
        name = qApp->tr("General Purpose Controller 1 LSB");
        break;
    case 0x31:
        name = qApp->tr("General Purpose Controller 2 LSB");
        break;
    case 0x32:
        name = qApp->tr("General Purpose Controller 3 LSB");
        break;
    case 0x33:
        name = qApp->tr("General Purpose Controller 4 LSB");
        break;
    case 0x34:
        name = qApp->tr("Control 20 LSB");
        break;
    case 0x35:
        name = qApp->tr("Control 21 LSB");
        break;
    case 0x36:
        name = qApp->tr("Control 22 LSB");
        break;
    case 0x37:
        name = qApp->tr("Control 23 LSB");
        break;
    case 0x38:
        name = qApp->tr("Control 24 LSB");
        break;
    case 0x39:
        name = qApp->tr("Control 25 LSB");
        break;
    case 0x3a:
        name = qApp->tr("Control 26 LSB");
        break;
    case 0x3b:
        name = qApp->tr("Control 27 LSB");
        break;
    case 0x3c:
        name = qApp->tr("Control 28 LSB");
        break;
    case 0x3d:
        name = qApp->tr("Control 29 LSB");
        break;
    case 0x3e:
        name = qApp->tr("Control 30 LSB");
        break;
    case 0x3f:
        name = qApp->tr("Control 31 LSB");
        break;
    case 0x40:
        name = qApp->tr("Damper Pedal On/Off");
        break;
    case 0x41:
        name = qApp->tr("Portamento On/Off");
        break;
    case 0x42:
        name = qApp->tr("Sostenuto On/Off");
        break;
    case 0x43:
        name = qApp->tr("Soft Pedal On/Off");
        break;
    case 0x44:
        name = qApp->tr("Legato Footswitch");
        break;
    case 0x45:
        name = qApp->tr("Hold 2");
        break;
    case 0x46:
        name = qApp->tr("Sound Controller 1 (Default: Sound Variation)");
        break;
    case 0x47:
        name = qApp->tr("Sound Controller 2 (Default: Timbre/Harmonic "
                        "Intensity)");
        break;
    case 0x48:
        name = qApp->tr("Sound Controller 3 (Default: Release Time)");
        break;
    case 0x49:
        name = qApp->tr("Sound Controller 4 (Default: Attack Time)");
        break;
    case 0x4a:
        name = qApp->tr("Sound Controller 5 (Default: Brightness)");
        break;
    case 0x4b:
        name = qApp->tr("Sound Controller 6 (Default: Decay Time)");
        break;
    case 0x4c:
        name = qApp->tr("Sound Controller 7 (Default: Vibrato Rate)");
        break;
    case 0x4d:
        name = qApp->tr("Sound Controller 8 (Default: Vibrato Depth)");
        break;
    case 0x4e:
        name = qApp->tr("Sound Controller 9 (Default: Vibrato Delay)");
        break;
    case 0x4f:
        name = qApp->tr("Sound Controller 10");
        break;
    case 0x50:
        name = qApp->tr("General Purpose Controller 5");
        break;
    case 0x51:
        name = qApp->tr("General Purpose Controller 6");
        break;
    case 0x52:
        name = qApp->tr("General Purpose Controller 7");
        break;
    case 0x53:
        name = qApp->tr("General Purpose Controller 8");
        break;
    case 0x5b:
        name = qApp->tr("Effects 1 Depth (Default: Reverb Send Level)");
        break;
    case 0x5c:
        name = qApp->tr("Effects 2 Depth");
        break;
    case 0x5d:
        name = qApp->tr("Effects 3 Depth (Default: Chorus Send Level");
        break;
    case 0x5e:
        name = qApp->tr("Effects 4 Depth");
        break;
    case 0x5f:
        name = qApp->tr("Effects 5 Depth");
        break;
    case 0x60:
        name = qApp->tr("Data Increment");
        break;
    case 0x61:
        name = qApp->tr("Data Decrement");
        break;
    case 0x62:
        name = qApp->tr("Non-Registered Parameter Number LSB");
        break;
    case 0x63:
        name = qApp->tr("Non-Registered Parameter Number MSB");
        break;
    case 0x64:
        name = qApp->tr("Registered Parameter Number LSB");
        break;
    case 0x65:
        name = qApp->tr("Registered Parameter Number MSB");
        break;
    case 0x78:
        name = qApp->tr("All Sound Off");
        break;
    case 0x79:
        name = qApp->tr("Reset All Controllers");
        break;
    case 0x7a:
        name = qApp->tr("Local Control On/Off");
        break;
    case 0x7b:
        name = qApp->tr("All Notes Off");
        break;
    case 0x7c:
        name = qApp->tr("Omni Mode Off");
        break;
    case 0x7d:
        name = qApp->tr("Omni Mode On");
        break;
    case 0x7e:
        name = qApp->tr("Mono Mode On");
        break;
    case 0x7f:
        name = qApp->tr("Poly Mode On");
        break;
    default:
        name = qApp->tr("Undefined");
    }
    return qApp->translate("midiControlFormat", "Control #%1 - %2").
        arg(QLocale::system().toString(control), name);
}

QString
synthclone::getMIDINoteString(MIDIData note)
{
    CONFIRM(note < 0x80, qApp->tr("'%1': invalid MIDI note value").arg(note));
    QString noteStr;
    switch (note % 12) {
    case 0:
        noteStr = qApp->translate("cNote", "C");
        break;
    case 1:
        noteStr = qApp->translate("cSharpNote", "C#");
        break;
    case 2:
        noteStr = qApp->translate("dNote", "D");
        break;
    case 3:
        noteStr = qApp->translate("dSharpNote", "D#");
        break;
    case 4:
        noteStr = qApp->translate("eNote", "E");
        break;
    case 5:
        noteStr = qApp->translate("fNote", "F");
        break;
    case 6:
        noteStr = qApp->translate("fSharpNote", "F#");
        break;
    case 7:
        noteStr = qApp->translate("gNote", "G");
        break;
    case 8:
        noteStr = qApp->translate("gSharpNote", "G#");
        break;
    case 9:
        noteStr = qApp->translate("aNote", "A");
        break;
    case 10:
        noteStr = qApp->translate("aSharpNote", "A#");
        break;
    case 11:
        noteStr = qApp->translate("bNote", "B");
    }
    QLocale locale = QLocale::system();
    return qApp->translate("midiNoteFormat", "%1 (%2%3)").
        arg(locale.toString(static_cast<int>(note)), noteStr,
            locale.toString(static_cast<int>(note / 12) - 1));
}

QString
synthclone::getSampleFilenameExtension(SampleStream::Type type)
{
    QString extension;
    switch (type) {
    case SampleStream::TYPE_AIFF:
        extension = "aiff";
        break;
    case SampleStream::TYPE_AU:
        extension = "au";
        break;
    case SampleStream::TYPE_AVR:
        extension = "avr";
        break;
    case SampleStream::TYPE_CAF:
        extension = "caf";
        break;
    case SampleStream::TYPE_FLAC:
        extension = "flac";
        break;
    case SampleStream::TYPE_HTK:
        extension = "htk";
        break;
    case SampleStream::TYPE_IRCAM:
        extension = "sf";
        break;
    case SampleStream::TYPE_MAT4:
    case SampleStream::TYPE_MAT5:
        extension = "mat";
        break;
    case SampleStream::TYPE_MPC2K:
        extension = "snd";
        break;
    case SampleStream::TYPE_NIST:
    case SampleStream::TYPE_WAV:
    case SampleStream::TYPE_WAVEX:
        extension = "wav";
        break;
    case SampleStream::TYPE_OGG:
        extension = "ogg";
        break;
    case SampleStream::TYPE_PAF:
        extension = "paf";
        break;
    case SampleStream::TYPE_PVF:
        extension = "pvf";
        break;
    case SampleStream::TYPE_RAW:
        extension = "raw";
        break;
    case SampleStream::TYPE_RF64:
        extension = "rf64";
        break;
    case SampleStream::TYPE_SD2:
        extension = "sd2";
        break;
    case SampleStream::TYPE_SDS:
        extension = "sds";
        break;
    case SampleStream::TYPE_SVX:
        extension = "svx";
        break;
    case SampleStream::TYPE_VOC:
        extension = "voc";
        break;
    case SampleStream::TYPE_W64:
        extension = "w64";
        break;
    case SampleStream::TYPE_WVE:
        extension = "wve";
        break;
    case SampleStream::TYPE_XI:
        extension = "xi";
        break;
    default:
        CONFIRM(false, qApp->tr("invalid sample stream type"));
    }
    return extension;
}

QWidget *
synthclone::loadForm(const QString &path, QWidget *parent)
{
    QFile file(path);
    file.open(QFile::ReadOnly);
    QWidget *widget = QUiLoader().load(&file, parent);

    CONFIRM(widget, qApp->tr("failed to load widget from '%1'").arg(path));

    file.close();
    return widget;
}
