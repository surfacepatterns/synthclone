#include <synthclone/util.h>

#include "sampleratechangeview.h"

SampleRateChangeView::SampleRateChangeView(QObject *parent):
    synthclone::DesignerView
    (":/synthclone/plugins/jack/sampleratechangeview.ui", parent)
{
    QWidget *widget = getRootWidget();

    cancelButton = synthclone::getChild<QPushButton>(widget, "cancelButton");
    connect(cancelButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    changeSampleRateButton = synthclone::getChild<QPushButton>
        (widget, "changeSampleRateButton");
    connect(changeSampleRateButton, SIGNAL(clicked()),
            SIGNAL(sampleRateChangeRequest()));
}

SampleRateChangeView::~SampleRateChangeView()
{
    // Empty
}
