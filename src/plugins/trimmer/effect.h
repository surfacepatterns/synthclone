#ifndef __EFFECT_H__
#define __EFFECT_H__

#include <synthclone/effect.h>

class Effect: public synthclone::Effect {

    Q_OBJECT

public:

    explicit
    Effect(const QString &name, QObject *parent=0);

    ~Effect();

    float
    getSampleFloor() const;

    bool
    getTrimEnd() const;

    bool
    getTrimStart() const;

    void
    process(const synthclone::Zone &zone,
            synthclone::SampleInputStream &inputStream,
            synthclone::SampleOutputStream &outputStream);

public slots:

    void
    setSampleFloor(float sampleFloor);

    void
    setTrimEnd(bool trimEnd);

    void
    setTrimStart(bool trimStart);

signals:

    void
    sampleFloorChanged(float sampleFloor);

    void
    trimEndChanged(bool trimEnd);

    void
    trimStartChanged(bool trimStart);

private slots:

    void
    handleCopyProgress(synthclone::SampleFrameCount current,
                       synthclone::SampleFrameCount total);

private:

    float
    getDBFS(float sample) const;

    float sampleFloor;
    bool trimEnd;
    bool trimStart;

};

#endif
