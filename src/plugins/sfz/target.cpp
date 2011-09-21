#include <cassert>

#include <QtCore/QLocale>

#include <synthclone/error.h>
#include <synthclone/samplecopier.h>
#include <synthclone/util.h>

#include "target.h"
#include "velocitycomparer.h"

Target::Target(const QString &name, QObject *parent):
    synthclone::Target(name, parent)
{
    layerAlgorithm = LAYERALGORITHM_LINEAR_INTERPOLATION;
    sampleFormat = SAMPLEFORMAT_FLAC_24BIT;
}

Target::~Target()
{
    // Empty
}

int
Target::buildZoneMap(ZoneMap &map, const QList<synthclone::Zone *> &zones)
{
    emit progressChanged(0.0);
    emit statusChanged(tr("Building zone map ..."));
    int missingSampleCount = 0;
    int zoneCount = zones.count();
    for (int i = 0; i < zoneCount; i++) {
        emit progressChanged((static_cast<float>(i) / zoneCount) * 0.5);
        synthclone::Zone *zone = zones[i];
        const synthclone::Sample *sample = zone->getWetSample();
        if (! sample) {
            sample = zone->getDrySample();
            if (! sample) {
                missingSampleCount++;
                continue;
            }
        }
        map.insert(ZoneKey(*zone), zone);
    }
    emit progressChanged(0.5);
    return missingSampleCount;
}

QString
Target::getAuthor() const
{
    return author;
}

QString
Target::getInfo() const
{
    return info;
}

LayerAlgorithm
Target::getLayerAlgorithm() const
{
    return layerAlgorithm;
}

QString
Target::getLicense() const
{
    return license;
}

QString
Target::getPath() const
{
    return path;
}

SampleFormat
Target::getSampleFormat() const
{
    return sampleFormat;
}

void
Target::save(const QList<synthclone::Zone *> &zones)
{
    ZoneMap map;
    buildZoneMap(map, zones);

    QDir directory(path);
    if (! directory.exists()) {
        throw synthclone::Error(tr("'%1' does not point to an existing "
                                   "directory").arg(path));
    }

    emit statusChanged(tr("Initializing drumkit file ..."));

    QFile file(directory.absoluteFilePath("drumkit.xml"));
    if (! file.open(QIODevice::WriteOnly)) {
        throw synthclone::Error(file.errorString());
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(4);
    writer.writeStartElement("drumkit_info");

    writeElement(writer, "name", getName());
    writeElement(writer, "author", author);
    writeElement(writer, "info", info);
    writeElement(writer, "license", license);

    QList<ZoneKey> keys = map.uniqueKeys();
    int instrumentCount = keys.count();
    if (instrumentCount > 1000) {
        instrumentCount = 1000;
    }

    emit statusChanged(tr("Writing instrument list ..."));

    QLocale locale = QLocale::system();

    writer.writeStartElement("instrumentList");
    for (int i = 0; i < instrumentCount; i++) {
        float startProgress = ((static_cast<float>(i) / instrumentCount) *
                               0.5) + 0.5;
        float endProgress = ((static_cast<float>(i + 1) / instrumentCount) *
                             0.5) + 0.5;
        float difference = endProgress - startProgress;

        emit progressChanged(startProgress);
        emit statusChanged(tr("Writing instrument %1 of %2 ...").
                           arg(locale.toString(i + 1),
                               locale.toString(instrumentCount)));

        writer.writeStartElement("instrument");
        writeElement(writer, "id", QString::number(i));
        writeElement(writer, "name",
                     tr("Instrument #%1").arg(locale.toString(i + 1)));
        writeElement(writer, "Attack", "0.0");
        writeElement(writer, "Decay", "0.0");
        writeElement(writer, "Sustain", "1.0");
        writeElement(writer, "Release", "1000.0");
        writeElement(writer, "filterActive", "false");
        writeElement(writer, "filterCutoff", "0.0");
        writeElement(writer, "filterResonance", "0.0");
        writeElement(writer, "gain", "1.0");
        writeElement(writer, "isMuted", "false");
        writeElement(writer, "muteGroup", "-1");
        writeElement(writer, "pan_L", "1.0");
        writeElement(writer, "pan_R", "1.0");
        writeElement(writer, "randomPitchFactor", "0.0");
        writeElement(writer, "isStopNote", "false");

        QList<const synthclone::Zone *> zones = map.values(keys[i]);
        int layerCount = zones.count();
        assert(layerCount);
        if (layerCount > 16) {
            layerCount = 16;
        }

        qStableSort(zones.begin(), zones.end(), VelocityComparer());

        float lowVelocity = 0.0;
        for (int j = 0; j < layerCount - 1; j++) {
            emit progressChanged(((static_cast<float>(j) / layerCount) *
                                  difference) + startProgress);
            emit statusChanged(tr("Writing layer %1 of %2 for instrument %3 of "
                                  "%4 ...").
                               arg(locale.toString(j + 1),
                                   locale.toString(layerCount),
                                   locale.toString(i + 1),
                                   locale.toString(instrumentCount)));

            const synthclone::Zone *currentZone = zones[j];
            const synthclone::Zone *nextZone = zones[j + 1];

            float highVelocity;
            switch (layerAlgorithm) {
            case LAYERALGORITHM_LINEAR_INTERPOLATION:
                // 127.0 * 2.0 = 254.0
                highVelocity = (static_cast<float>(nextZone->getVelocity()) +
                                currentZone->getVelocity()) / 254.0;
                break;
            case LAYERALGORITHM_MAXIMUM:
                highVelocity = currentZone->getVelocity() / 127.0;
                break;
            case LAYERALGORITHM_MINIMUM:
                highVelocity = nextZone->getVelocity() / 127.0;
                break;
            default:
                assert(false);
            }

            writeLayer(writer, i, j, lowVelocity, highVelocity, currentZone,
                       directory);
            lowVelocity = highVelocity;
        }
        emit progressChanged(((static_cast<float>(layerCount - 1) /
                               layerCount) * difference) + startProgress);
        emit statusChanged(tr("Writing layer %1 of %2 for instrument %3 of "
                              "%4 ...").
                           arg(locale.toString(layerCount),
                               locale.toString(layerCount),
                               locale.toString(i + 1),
                               locale.toString(instrumentCount)));

        writeLayer(writer, i, layerCount - 1, lowVelocity, 1.0,
                   zones[layerCount - 1], directory);
        writer.writeEndElement();
    }
    writer.writeEndElement();

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();

    emit progressChanged(0.0);
    emit statusChanged("Idle.");
}

void
Target::setAuthor(const QString &author)
{
    if (this->author != author) {
        this->author = author;
        emit authorChanged(author);
    }
}

void
Target::setInfo(const QString &info)
{
    if (this->info != info) {
        this->info = info;
        emit infoChanged(info);
    }
}

void
Target::setLayerAlgorithm(LayerAlgorithm algorithm)
{
    if (layerAlgorithm != algorithm) {
        layerAlgorithm = algorithm;
        emit layerAlgorithmChanged(algorithm);
    }
}

void
Target::setLicense(const QString &license)
{
    if (this->license != license) {
        this->license = license;
        emit licenseChanged(license);
    }
}

void
Target::setPath(const QString &path)
{
    if (this->path != path) {
        this->path = path;
        emit pathChanged(path);
    }
}

void
Target::setSampleFormat(SampleFormat format)
{
    if (sampleFormat != format) {
        sampleFormat = format;
        emit sampleFormatChanged(format);
    }
}

void
Target::validate(const QList<synthclone::Zone *> &zones)
{
    QString message;
    ZoneMap zoneMap;
    int missingSampleCount = buildZoneMap(zoneMap, zones);

    emit statusChanged(tr("Validating ..."));

    QLocale locale = QLocale::system();
    if (missingSampleCount) {
        message = tr("%1 zones don't contain samples.  They will be skipped.").
            arg(locale.toString(missingSampleCount));
        emit validationWarning(message);
    }
    QList<ZoneKey> keys = zoneMap.uniqueKeys();
    int instrumentCount = keys.count();
    if (instrumentCount > 1000) {
        message = tr("The current zone list contains %1 potential drum kit "
                     "instruments.  Hydrogen only supports %2 instruments per "
                     "drum kit.  Some instruments will not be generated.").
            arg(locale.toString(instrumentCount), locale.toString(1000));
        emit validationWarning(message);
        instrumentCount = 1000;
    }
    int layerOverflows = 0;
    for (int i = 0; i < instrumentCount; i++) {

        emit progressChanged(((static_cast<float>(i) / instrumentCount) * 0.5) +
                             0.5);

        if (zoneMap.count(keys[i]) > 16) {
            layerOverflows++;
        }
    }
    if (layerOverflows) {
        message = tr("%1 of the %2 drum kit instruments contain more than %3 "
                     "layers.  Hydrogen only supports %3 layers per "
                     "instrument.  Some layers will not be generated.").
            arg(locale.toString(layerOverflows),
                locale.toString(instrumentCount), locale.toString(16));
        emit validationWarning(message);
    }

    if (! path.length()) {
        message = tr("You must set a path for the Hydrogen kit.");
        emit validationError(message);
    } else {
        QDir directory(path);
        if (! directory.exists()) {
            message = tr("The path '%1' does not point to an existing "
                         "directory.").arg(path);
            emit validationError(message);
        }
    }

    emit progressChanged(0.0);
    emit statusChanged("Idle.");
}

void
Target::writeElement(QXmlStreamWriter &writer, const QString &name,
                     const QString &value)
{
    writer.writeStartElement(name);
    writer.writeCharacters(value);
    writer.writeEndElement();
}

void
Target::writeLayer(QXmlStreamWriter &writer, int instrument, int layer,
                   float lowVelocity, float highVelocity,
                   const synthclone::Zone *zone, const QDir &directory)
{
    synthclone::SampleStream::SubType sampleStreamSubType;
    synthclone::SampleStream::Type sampleStreamType;
    switch (sampleFormat) {
    case SAMPLEFORMAT_AIFF_8BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_S8;
        sampleStreamType = synthclone::SampleStream::TYPE_AIFF;
        break;
    case SAMPLEFORMAT_AIFF_16BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_16;
        sampleStreamType = synthclone::SampleStream::TYPE_AIFF;
        break;
    case SAMPLEFORMAT_AIFF_24BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_24;
        sampleStreamType = synthclone::SampleStream::TYPE_AIFF;
        break;
    case SAMPLEFORMAT_AIFF_32BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_32;
        sampleStreamType = synthclone::SampleStream::TYPE_AIFF;
        break;
    case SAMPLEFORMAT_AIFF_32BIT_FLOAT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_FLOAT;
        sampleStreamType = synthclone::SampleStream::TYPE_AIFF;
        break;
    case SAMPLEFORMAT_AU_8BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_S8;
        sampleStreamType = synthclone::SampleStream::TYPE_AU;
        break;
    case SAMPLEFORMAT_AU_16BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_16;
        sampleStreamType = synthclone::SampleStream::TYPE_AU;
        break;
    case SAMPLEFORMAT_AU_24BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_24;
        sampleStreamType = synthclone::SampleStream::TYPE_AU;
        break;
    case SAMPLEFORMAT_AU_32BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_32;
        sampleStreamType = synthclone::SampleStream::TYPE_AU;
        break;
    case SAMPLEFORMAT_AU_32BIT_FLOAT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_FLOAT;
        sampleStreamType = synthclone::SampleStream::TYPE_AU;
        break;
    case SAMPLEFORMAT_FLAC_8BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_S8;
        sampleStreamType = synthclone::SampleStream::TYPE_FLAC;
        break;
    case SAMPLEFORMAT_FLAC_16BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_16;
        sampleStreamType = synthclone::SampleStream::TYPE_FLAC;
        break;
    case SAMPLEFORMAT_FLAC_24BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_24;
        sampleStreamType = synthclone::SampleStream::TYPE_FLAC;
        break;
    case SAMPLEFORMAT_WAV_8BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_S8;
        sampleStreamType = synthclone::SampleStream::TYPE_WAV;
        break;
    case SAMPLEFORMAT_WAV_16BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_16;
        sampleStreamType = synthclone::SampleStream::TYPE_WAV;
        break;
    case SAMPLEFORMAT_WAV_24BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_24;
        sampleStreamType = synthclone::SampleStream::TYPE_WAV;
        break;
    case SAMPLEFORMAT_WAV_32BIT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_PCM_32;
        sampleStreamType = synthclone::SampleStream::TYPE_WAV;
        break;
    case SAMPLEFORMAT_WAV_32BIT_FLOAT:
        sampleStreamSubType = synthclone::SampleStream::SUBTYPE_FLOAT;
        sampleStreamType = synthclone::SampleStream::TYPE_WAV;
        break;
    default:
        assert(false);
    }

    QString sampleName = tr("instrument%1-layer%2.%3").
        arg(instrument + 1).arg(layer + 1).
        arg(synthclone::getSampleFilenameExtension(sampleStreamType));
    const synthclone::Sample *sample = zone->getWetSample();
    if (! sample) {
        sample = zone->getDrySample();
        assert(sample);
    }

    synthclone::Sample outSample(directory.absoluteFilePath(sampleName));
    synthclone::SampleInputStream inputStream(*sample);
    synthclone::SampleChannelCount channels = inputStream.getChannels();
    synthclone::SampleOutputStream outputStream
        (outSample, inputStream.getSampleRate(), channels, sampleStreamType,
         sampleStreamSubType);
    synthclone::SampleCopier copier;
    copier.copy(inputStream, outputStream, inputStream.getFrames());

    writer.writeStartElement("layer");
    writeElement(writer, "filename", sampleName);
    writeElement(writer, "min", QString::number(lowVelocity));
    writeElement(writer, "max", QString::number(highVelocity));
    writeElement(writer, "gain", "1.0");
    writeElement(writer, "pitch", "0.0");
    writer.writeEndElement();
}
