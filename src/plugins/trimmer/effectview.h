#ifndef __EFFECTVIEW_H__
#define __EFFECTVIEW_H__

#include <QtGui/QCheckBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

#include <synthclone/designerview.h>

class EffectView: public synthclone::DesignerView {

    Q_OBJECT

public:

    explicit
    EffectView(QObject *parent=0);

    ~EffectView();

public slots:

    void
    setName(const QString &name);

    void
    setSampleFloor(float sampleFloor);

    void
    setSampleFloorEditorEnabled(bool enabled);

    void
    setTrimEnd(bool trimEnd);

    void
    setTrimStart(bool trimStart);

signals:

    void
    nameChanged(const QString &name);

    void
    sampleFloorChanged(float sampleFloor);

    void
    trimEndChanged(bool trimEnd);

    void
    trimStartChanged(bool trimStart);

private slots:

    void
    handleSampleFloorValueChangedSignal(double value);

private:

    QPushButton *closeButton;
    QLineEdit *name;
    QDoubleSpinBox *sampleFloor;
    QCheckBox *trimEnd;
    QCheckBox *trimStart;

};

#endif
