#ifndef __SAVEWARNINGVIEW_H__
#define __SAVEWARNINGVIEW_H__

#include <QtGui/QPushButton>

#include "dialogview.h"

class SaveWarningView: public DialogView {

    Q_OBJECT

public:

    explicit
    SaveWarningView(QObject *parent=0);

    ~SaveWarningView();

signals:

    void
    saveRequest();

private:

    QPushButton *cancelButton;
    QPushButton *saveButton;

};

#endif
