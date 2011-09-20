#ifndef __ERRORVIEW_H__
#define __ERRORVIEW_H__

#include <QtGui/QLabel>
#include <QtGui/QPushButton>

#include "dialogview.h"

class ErrorView: public DialogView {

    Q_OBJECT

public:

    explicit
    ErrorView(QObject *parent=0);

    ~ErrorView();

    void
    setMessage(const QString &message);

private:

    QPushButton *closeButton;
    QLabel *message;

};

#endif
