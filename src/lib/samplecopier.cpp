#include <cassert>

#include <synthclone/samplecopier.h>
#include <synthclone/util.h>

using synthclone::SampleCopier;

SampleCopier::SampleCopier(QObject *parent):
    QObject(parent)
{
    // Empty
}

SampleCopier::~SampleCopier()
{
    // Empty
}

synthclone::SampleFrameCount
SampleCopier::copy(SampleInputStream &inputStream,
                   SampleOutputStream &outputStream, SampleFrameCount frames)
{
    SampleChannelCount channels = inputStream.getChannels();

    CONFIRM(channels == outputStream.getChannels(),
            qApp->tr("the channel counts of the streams are not equal"));
    CONFIRM(inputStream.getSampleRate() == outputStream.getSampleRate(),
            qApp->tr("the sample rates of the streams are not equal"));
    CONFIRM(frames >= 0, tr("'%1': invalid frame count").arg(frames));

    SampleFrameCount framesRead;
    SampleFrameCount readSize = static_cast<SampleFrameCount>(65536 / channels);
    assert(readSize >= 1);
    SampleFrameCount framesLeft = frames;
    SampleFrameCount totalFramesProcessed = 0;
    for (; framesLeft > readSize; framesLeft -= readSize) {
        framesRead = inputStream.read(buffer, readSize);
        if (! framesRead) {
            return totalFramesProcessed;
        }
        outputStream.write(buffer, framesRead);
        totalFramesProcessed += framesRead;
        emit copyProgress(totalFramesProcessed, frames);
        if (framesRead != readSize) {
            return totalFramesProcessed;
        }
    }
    if (framesLeft) {
        framesRead = inputStream.read(buffer, framesLeft);
        if (framesRead) {
            outputStream.write(buffer, framesRead);
            totalFramesProcessed += framesRead;
            emit copyProgress(totalFramesProcessed, frames);
        }
    }
    return totalFramesProcessed;
}
