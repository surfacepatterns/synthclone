#include <synthclone/error.h>

#include "application.h"

Application::Application(int argc, char **argv):
    QApplication(argc, argv)
{
    // Empty
}

Application::~Application()
{
    // Empty
}

bool
Application::notify(QObject *receiver, QEvent *event)
{
    try {
        return QApplication::notify(receiver, event);
    } catch (synthclone::Error &e) {
        emit eventError(e.getMessage());
    }
    return false;
}
