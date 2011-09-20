#include <cerrno>
#include <cstring>

#include <synthclone/error.h>
#include <synthclone/sampleinputstream.h>
#include <synthclone/util.h>

using synthclone::SampleInputStream;

SampleInputStream::SampleInputStream(const Sample &sample, QObject *parent):
    SampleStream(parent)
{
    info.format = 0;
    handle = sf_open(sample.path.toLocal8Bit().data(), SFM_READ, &info);
    if (! handle) {
        throw Error(std::strerror(errno));
    }
}

SampleInputStream::~SampleInputStream()
{
    // Empty
}

synthclone::SampleFrameCount
SampleInputStream::read(float *buffer, SampleFrameCount frames)
{
    CONFIRM(buffer, tr("buffer is set to NULL"));
    CONFIRM(frames > 0, tr("'%1': invalid frames value").arg(frames));

    sf_count_t readFrames =
        sf_readf_float(handle, buffer, static_cast<sf_count_t>(frames));
    if (readFrames <= 0) {
        int errorNumber = sf_error(handle);
        if (errorNumber != SF_ERR_NO_ERROR) {
            throw synthclone::Error(sf_error_number(errorNumber));
        }
    }
    return static_cast<SampleFrameCount>(readFrames);
}
