#include <cstdio>
#include <cstdlib>
#include <exception>

#include <QtCore/QDebug>
#include <QtCore/QLibraryInfo>
#include <QtCore/QLocale>
#include <QtCore/QTextStream>
#include <QtCore/QTranslator>

#include <synthclone/error.h>

#include "controller.h"

int
main(int argc, char **argv)
{
    Application application(argc, argv);
    QString errorMessage;
    try {
        application.setApplicationName("synthclone");
        application.setOrganizationDomain("synthclone.googlecode.com");
        application.setOrganizationName("synthclone.googlecode.com");

        // Translations
        QString directory =
            QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        QString language = QLocale::system().name();
        QTranslator qtTranslator;
        qtTranslator.load("qt_" + language, directory);
        application.installTranslator(&qtTranslator);
        QTranslator translator;
        translator.load("synthclone_" + language);
        application.installTranslator(&translator);
        qDebug() << application.tr("Translations loaded.");

        // Command line arguments
        qDebug() << application.tr("Scanning command-line arguments ...");
        QStringList arguments = application.arguments();
        bool loadSession;
        QDir sessionDirectory;
        switch (arguments.count()) {
        case 0:
            loadSession = false;
            break;
        case 1:
            if (sessionDirectory.cd(arguments[0])) {
                loadSession = true;
                break;
            }
            // Fallthrough on purpose
        default:
            QTextStream(stderr, QIODevice::WriteOnly) <<
                application.tr("Usage: synthclone [qt-args] [session-dir]");
            return EXIT_FAILURE;
        }

        // Controller
        qDebug() << application.tr("Creating core application objects ...");
        Controller controller(application);
        qDebug() << application.tr("Core application objects created.");

        // Run the program
        qDebug() << application.tr("Running ...");
        controller.run(loadSession ? &sessionDirectory : 0);

        qDebug() << application.tr("Exiting without errors ...");
        return EXIT_SUCCESS;
    } catch (synthclone::Error &e) {
        errorMessage = e.getMessage();
    } catch (std::exception &e) {
        errorMessage = e.what();
    }
    QTextStream(stderr) << application.tr("Error: %1").arg(errorMessage);
    return EXIT_FAILURE;
}
