/*
 * synthclone - Synthesizer-cloning software
 * Copyright (C) 2011 Devin Anderson
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 * Ave, Cambridge, MA 02139, USA.
 */

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
    QStringList arguments = application.arguments();
    QString errorMessage;

    application.setApplicationName("synthclone");
    application.setOrganizationDomain("synthclone.googlecode.com");
    application.setOrganizationName("synthclone.googlecode.com");

    // Translations
    QString directory = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    QString language = QLocale::system().name();
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + language, directory);
    application.installTranslator(&qtTranslator);
    QTranslator translator;
    translator.load("synthclone_" + language);
    application.installTranslator(&translator);
    qDebug() << application.tr("Translations loaded.");

    // Command line arguments
    bool loadSession;
    int result;
    QDir sessionDirectory;
    switch (arguments.count()) {
    case 0:
    case 1:
        loadSession = false;
        break;
    case 2:
        if (sessionDirectory.cd(arguments[1])) {
            loadSession = true;
            break;
        }
        // Fallthrough on purpose
    default:
        QTextStream(stderr, QIODevice::WriteOnly) <<
            application.tr("Usage: synthclone [qt-args] [session-dir]\n");
        result = EXIT_FAILURE;
        goto unloadTranslations;
    }

    try {

        // Controller
        qDebug() << application.tr("Creating core application objects ...");
        Controller controller(application);
        qDebug() << application.tr("Core application objects created.");

        // Run the program
        qDebug() << application.tr("Running ...");
        controller.run(loadSession ? &sessionDirectory : 0);

    } catch (synthclone::Error &e) {
        errorMessage = e.getMessage();
    } catch (std::exception &e) {
        errorMessage = e.what();
    }

    // Deal with errors.
    if (errorMessage.isEmpty()) {
        qDebug() << application.tr("Exiting without errors ...");
        result = EXIT_SUCCESS;
    } else {
        QTextStream(stderr) << application.tr("Error: %1\n").arg(errorMessage);
        result = EXIT_FAILURE;
    }

unloadTranslations:
    qDebug() << application.tr("Unloading translations ...");
    application.removeTranslator(&translator);
    application.removeTranslator(&qtTranslator);

    return result;
}
