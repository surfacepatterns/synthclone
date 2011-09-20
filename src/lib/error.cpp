#include <synthclone/error.h>

using synthclone::Error;

Error::Error(const QString &message)
{
    this->message = message;
}

Error::~Error()
{
    // Empty
}

QString
Error::getMessage()
{
    return message;
}
