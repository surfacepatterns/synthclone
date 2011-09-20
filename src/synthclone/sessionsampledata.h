#ifndef __SESSIONSAMPLEDATA_H__
#define __SESSIONSAMPLEDATA_H__

#include <QtCore/QDir>

#include <synthclone/sample.h>
#include <synthclone/types.h>

class SessionSampleData: public QObject {

    Q_OBJECT

public:

    explicit
    SessionSampleData(QObject *parent=0);

    ~SessionSampleData();

    synthclone::SampleChannelCount
    getSampleChannelCount() const;

    const QDir *
    getSampleDirectory() const;

    synthclone::SampleRate
    getSampleRate() const;

public slots:

    void
    setSampleChannelCount(synthclone::SampleChannelCount count);

    void
    setSampleDirectory(const QDir *directory);

    void
    setSampleRate(synthclone::SampleRate sampleRate);

    synthclone::Sample *
    updateSample(synthclone::Sample &sample, QObject *parent=0);

signals:

    void
    sampleChannelCountChanged(synthclone::SampleChannelCount count);

    void
    sampleDirectoryChanged(const QDir *directory);

    void
    sampleRateChanged(synthclone::SampleRate sampleRate);

private:

    enum ChannelConvertAlgorithm {
        CHANNELCONVERTALGORITHM_NONE,
        CHANNELCONVERTALGORITHM_FROM_MONO,
        CHANNELCONVERTALGORITHM_TO_MONO,
    };

    synthclone::SampleChannelCount sampleChannelCount;
    QDir *sampleDirectory;
    synthclone::SampleRate sampleRate;

};

#endif
