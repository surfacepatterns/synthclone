#ifndef __ABOUTVIEW_H__
#define __ABOUTVIEW_H__

#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include "dialogview.h"

class AboutView: public DialogView {

    Q_OBJECT

public:

    explicit
    AboutView(QObject *parent=0);

    ~AboutView();

    void
    setMajorVersion(int majorVersion);

    void
    setMinorVersion(int minorVersion);

    void
    setRevision(int revision);

private:

    void
    updateVersion();

    QPushButton *closeButton;
    int majorVersion;
    int minorVersion;
    int revision;
    QLabel *version;

};

#endif
