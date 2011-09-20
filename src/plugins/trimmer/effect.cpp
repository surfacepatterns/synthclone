#include <cassert>
#include <cfloat>
#include <cmath>

#include <QtCore/QScopedArrayPointer>

#include <synthclone/samplecopier.h>

#include "effect.h"

Effect::Effect(const QString &name, QObject *parent):
    synthclone::Effect(name, parent)
{
    sampleFloor = -70.0;
    trimEnd = true;
    trimStart = true;
}

Effect::~Effect()
{
    // Empty
}

float
Effect::getDBFS(float sample) const
{
    return (sample == 0.0) ? -FLT_MAX : 20.0 * std::log10(std::fabs(sample));
}

float
Effect::getSampleFloor() const
{
    return sampleFloor;
}

bool
Effect::getTrimEnd() const
{
    return trimEnd;
}

bool
Effect::getTrimStart() const
{
    return trimStart;
}

void
Effect::handleCopyProgress(synthclone::SampleFrameCount current,
                           synthclone::SampleFrameCount total)
{
    emit progressChanged(static_cast<float>(current) / total);
}

void
Effect::process(const synthclone::Zone &/*zone*/,
                synthclone::SampleInputStream &inputStream,
                synthclone::SampleOutputStream &outputStream)
{
    synthclone::SampleChannelCount channels = inputStream.getChannels();
    QScopedArrayPointer<float> audioDataPtr(new float[channels]);
    float *audioData = audioDataPtr.data();
    synthclone::SampleFrameCount frames = inputStream.getFrames();
    synthclone::SampleFrameCount framesRead;
    synthclone::SampleFrameCount end = frames - 1;
    synthclone::SampleFrameCount start = 0;
    if (trimStart) {
        emit statusChanged(tr("Trimming start of sample ..."));
        for (; start < end; start++) {
            framesRead = inputStream.read(audioData, 1);
            assert(framesRead == 1);
            for (int i = 0; i < channels; i++) {
                if (getDBFS(audioData[i]) >= sampleFloor) {
                    goto findEnd;
                }
            }
        }
    }
 findEnd:
    if (trimEnd) {
        emit statusChanged(tr("Trimming end of sample ..."));
        for (; end >= start; end--) {
            inputStream.seek(end, synthclone::SampleStream::OFFSET_START);
            framesRead = inputStream.read(audioData, 1);
            assert(framesRead == 1);
            for (int i = 0; i < channels; i++) {
                if (getDBFS(audioData[i]) >= sampleFloor) {
                    goto writeSample;
                }
            }
        }
    }
 writeSample:
    inputStream.seek(start, synthclone::SampleStream::OFFSET_START);
    synthclone::SampleFrameCount newFrameCount = (end - start) + 1;
    emit progressChanged(0.0);
    emit statusChanged(tr("Writing sample ..."));
    synthclone::SampleCopier copier;
    connect(&copier,
            SIGNAL(copyProgress(synthclone::SampleFrameCount,
                                synthclone::SampleFrameCount)),
            SLOT(handleCopyProgress(synthclone::SampleFrameCount,
                                    synthclone::SampleFrameCount)),
            Qt::DirectConnection);
    copier.copy(inputStream, outputStream, newFrameCount);
    emit progressChanged(0.0);
    emit statusChanged("");
}

void
Effect::setSampleFloor(float sampleFloor)
{
    assert(sampleFloor <= 0.0);
    if (this->sampleFloor != sampleFloor) {
        this->sampleFloor = sampleFloor;
        emit sampleFloorChanged(sampleFloor);
    }
}

void
Effect::setTrimEnd(bool trimEnd)
{
    if (this->trimEnd != trimEnd) {
        this->trimEnd = trimEnd;
        emit trimEndChanged(trimEnd);
    }
}

void
Effect::setTrimStart(bool trimStart)
{
    if (this->trimStart != trimStart) {
        this->trimStart = trimStart;
        emit trimStartChanged(trimStart);
    }
}
