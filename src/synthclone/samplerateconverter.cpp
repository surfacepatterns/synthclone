#include <synthclone/error.h>

#include "samplerateconverter.h"

SampleRateConverter::SampleRateConverter(int channels, double ratio,
                                         QObject *parent):
    QObject(parent)
{
    if (! src_is_valid_ratio(ratio)) {
        throw synthclone::Error(tr("'%1': invalid conversion ratio").arg(ratio));
    }
    int error;
    state = src_new(SRC_SINC_BEST_QUALITY, channels, &error);
    if (! state) {
        throw synthclone::Error(src_strerror(error));
    }
    this->ratio = ratio;
}

SampleRateConverter::~SampleRateConverter()
{
    src_delete(state);
}

long
SampleRateConverter::convert(float *input, long inputFrames, float *output,
                             long outputFrames, long &outputFramesUsed,
                             bool atEnd)
{
    SRC_DATA data;
    data.data_in = input;
    data.data_out = output;
    data.end_of_input = atEnd ? 1 : 0;
    data.input_frames = inputFrames;
    data.output_frames = outputFrames;
    data.src_ratio = ratio;
    int result = src_process(state, &data);
    if (result) {
        throw synthclone::Error(src_strerror(result));
    }
    outputFramesUsed = data.output_frames_gen;
    return data.input_frames_used;
}
