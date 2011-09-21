#ifndef __TARGET_H__
#define __TARGET_H__

#include <QtCore/QDir>
#include <QtCore/QXmlStreamWriter>

#include <synthclone/target.h>

#include "layeralgorithm.h"
#include "sampleformat.h"
#include "zonekey.h"

class Target: public synthclone::Target {

    Q_OBJECT

public:

    explicit
    Target(const QString &name, QObject *parent=0);

    ~Target();

    QString
    getAuthor() const;

    QString
    getInfo() const;

    LayerAlgorithm
    getLayerAlgorithm() const;

    QString
    getLicense() const;

    QString
    getPath() const;

    SampleFormat
    getSampleFormat() const;

public slots:

    void
    save(const QList<synthclone::Zone *> &zones);

    void
    setAuthor(const QString &author);

    void
    setInfo(const QString &info);

    void
    setLayerAlgorithm(LayerAlgorithm algorithm);

    void
    setLicense(const QString &license);

    void
    setPath(const QString &path);

    void
    setSampleFormat(SampleFormat format);

    void
    validate(const QList<synthclone::Zone *> &zones);

signals:

    void
    authorChanged(const QString &author);

    void
    infoChanged(const QString &info);

    void
    layerAlgorithmChanged(LayerAlgorithm algorithm);

    void
    licenseChanged(const QString &license);

    void
    pathChanged(const QString &path);

    void
    sampleFormatChanged(SampleFormat format);

private:

    typedef QMultiMap<ZoneKey, const synthclone::Zone *> ZoneMap;

    int
    buildZoneMap(ZoneMap &map, const QList<synthclone::Zone *> &zones);

    void
    writeElement(QXmlStreamWriter &writer, const QString &name,
                 const QString &value);

    void
    writeLayer(QXmlStreamWriter &writer, int instrument, int layer,
               float lowVelocity, float highVelocity,
               const synthclone::Zone *zone, const QDir &directory);

    QString author;
    QString info;
    LayerAlgorithm layerAlgorithm;
    QString license;
    QString path;
    SampleFormat sampleFormat;

};

#endif
