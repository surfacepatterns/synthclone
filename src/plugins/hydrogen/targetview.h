#ifndef __TARGETVIEW_H__
#define __TARGETVIEW_H__

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>

#include <synthclone/designerview.h>

#include "layeralgorithm.h"
#include "sampleformat.h"

class TargetView: public synthclone::DesignerView {

    Q_OBJECT

public:

    explicit
    TargetView(QObject *parent=0);

    ~TargetView();

public slots:

    void
    setAuthor(const QString &author);

    void
    setInfo(const QString &info);

    void
    setLayerAlgorithm(LayerAlgorithm algorithm);

    void
    setLicense(const QString &license);

    void
    setName(const QString &name);

    void
    setPath(const QString &path);

    void
    setSampleFormat(SampleFormat format);

signals:

    void
    authorChangeRequest(const QString &author);

    void
    infoChangeRequest(const QString &info);

    void
    layerAlgorithmChangeRequest(LayerAlgorithm algorithm);

    void
    licenseChangeRequest(const QString &license);

    void
    nameChangeRequest(const QString &name);

    void
    pathChangeRequest(const QString &path);

    void
    pathLookupRequest();

    void
    sampleFormatChangeRequest(SampleFormat format);

private slots:

    void
    handleInfoChange();

    void
    handleLayerAlgorithmChange(int index);

    void
    handleSampleFormatChange(int index);

private:

    QLineEdit *author;
    QPushButton *closeButton;
    QPlainTextEdit *info;
    QComboBox *layerAlgorithmComboBox;
    QLineEdit *license;
    QLineEdit *name;
    QLineEdit *path;
    QPushButton *pathLookupButton;
    QComboBox *sampleFormatComboBox;

};

#endif
