#ifndef __SAMPLERATECHANGEVIEW_H__
#define __SAMPLERATECHANGEVIEW_H__

#include <QtGui/QDialog>
#include <QtGui/QPushButton>

#include <synthclone/designerview.h>

class SampleRateChangeView: public synthclone::DesignerView {

    Q_OBJECT

public:

    explicit
    SampleRateChangeView(QObject *parent=0);

    ~SampleRateChangeView();

signals:

    void
    sampleRateChangeRequest();

private:

    QPushButton *cancelButton;
    QPushButton *changeSampleRateButton;
    QDialog *dialog;

};

#endif
