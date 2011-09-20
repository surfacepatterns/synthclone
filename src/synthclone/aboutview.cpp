#include <QtCore/QLocale>

#include <synthclone/util.h>

#include "aboutview.h"

AboutView::AboutView(QObject *parent):
    DialogView(":/synthclone/aboutview.ui", parent)
{
    closeButton = synthclone::getChild<QPushButton>(dialog, "closeButton");
    connect(closeButton, SIGNAL(clicked()), SIGNAL(closeRequest()));

    majorVersion = 0;
    minorVersion = 0;
    revision = 0;

    version = synthclone::getChild<QLabel>(dialog, "version");

    updateVersion();
}

AboutView::~AboutView()
{
    // Empty
}

void
AboutView::setMajorVersion(int majorVersion)
{
    this->majorVersion = majorVersion;
    updateVersion();
}

void
AboutView::setMinorVersion(int minorVersion)
{
    this->minorVersion = minorVersion;
    updateVersion();
}

void
AboutView::setRevision(int revision)
{
    this->revision = revision;
    updateVersion();
}

void
AboutView::updateVersion()
{
    QLocale locale = QLocale::system();
    version->setText(tr("%1.%2.%3", "versionFormat").
                     arg(locale.toString(majorVersion)).
                     arg(locale.toString(minorVersion)).
                     arg(locale.toString(revision)));
}
