#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <QtGui/QApplication>

class Application: public QApplication {

    Q_OBJECT

public:

    Application(int argc, char **argv);

    ~Application();

    bool
    notify(QObject *receiver, QEvent *event);

signals:

    void
    eventError(const QString &message);

};

#endif
