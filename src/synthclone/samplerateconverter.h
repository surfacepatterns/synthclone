#ifndef __SAMPLERATECONVERTER_H__
#define __SAMPLERATECONVERTER_H__

#include <QtCore/QObject>

#include <samplerate.h>

class SampleRateConverter: public QObject {

    Q_OBJECT

public:

    SampleRateConverter(int channels, double ratio, QObject *parent=0);

    ~SampleRateConverter();

    long
    convert(float *input, long inputFrames, float *output, long outputFrames,
            long &outputFramesUsed, bool atEnd=false);

private:

    double ratio;
    SRC_STATE *state;

};

#endif
