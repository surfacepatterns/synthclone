#ifndef __PROGRESSVIEW_H__
#define __PROGRESSVIEW_H__

#include <QtGui/QLabel>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>

#include "dialogview.h"

class ProgressView: public DialogView {

    Q_OBJECT

public:

    explicit
    ProgressView(QObject *parent=0);

    ~ProgressView();

    void
    addMessage(const QString &message);

    void
    clearMessages();

    void
    setProgress(float progress);

    void
    setStatus(const QString &status);

private:

    QPushButton *closeButton;
    QPlainTextEdit *messages;
    QProgressBar *progressBar;
    QLabel *status;

};

#endif
