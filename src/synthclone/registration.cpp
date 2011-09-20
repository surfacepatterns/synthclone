#include "registration.h"

Registration::Registration(QObject *object, QObject *parent):
    synthclone::Registration(parent)
{
    this->object = object;
}

Registration::~Registration()
{
    emit unregistered(object);
}
