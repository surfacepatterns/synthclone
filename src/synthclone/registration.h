#ifndef __REGISTRATION_H__
#define __REGISTRATION_H__

#include <synthclone/registration.h>

class Registration: public synthclone::Registration {

    Q_OBJECT

public:

    explicit
    Registration(QObject *object, QObject *parent=0);

    ~Registration();

private:

    QObject *object;

};

#endif
