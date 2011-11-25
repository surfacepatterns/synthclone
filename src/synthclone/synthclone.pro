include(../../synthclone.pri)

################################################################################
# Build
################################################################################

isEmpty(BUILDDIR) {
    BUILDDIR = ../../build
}

isEmpty(MAKEDIR) {
    MAKEDIR = ../../make
}

CONFIG += console uitools
DEFINES += SYNTHCLONE_MAJOR_VERSION=$${MAJOR_VERSION} \
    SYNTHCLONE_MINOR_VERSION=$${MINOR_VERSION} \
    SYNTHCLONE_REVISION=$${REVISION}
DESTDIR = $${BUILDDIR}/bin
HEADERS += aboutview.h \
    application.h \
    componentviewlet.h \
    context.h \
    contextmenueventfilter.h \
    controller.h \
    dialogview.h \
    effectjob.h \
    effectjobthread.h \
    errorview.h \
    helpviewlet.h \
    mainview.h \
    menuactionviewlet.h \
    menuitemviewlet.h \
    menuleafviewlet.h \
    menuseparatorviewlet.h \
    menuviewlet.h \
    participanttreecolumn.h \
    participantview.h \
    participantviewlet.h \
    pluginmanager.h \
    progressbardelegate.h \
    progressview.h \
    registration.h \
    sampleprofile.h \
    samplerateconverter.h \
    samplerjob.h \
    savechangesview.h \
    savewarningview.h \
    session.h \
    sessionloadview.h \
    sessionsampledata.h \
    sessionviewlet.h \
    settings.h \
    signalmap.h \
    signalpair.h \
    standarditem.h \
    toolviewlet.h \
    util.h \
    viewviewlet.h \
    zone.h \
    zonecomparer.h \
    zonecomparerproxy.h \
    zoneindexcomparer.h \
    zonelistloader.h \
    zonetablecolumn.h \
    zonetabledelegate.h \
    zonetablemodel.h \
    zoneviewlet.h
INCLUDEPATH += ../include
LIBS += -L$${BUILDDIR}/lib -lsamplerate -lsynthclone
MOC_DIR = $${MAKEDIR}/synthclone
OBJECTS_DIR = $${MAKEDIR}/synthclone
RCC_DIR = $${MAKEDIR}/synthclone
RESOURCES += synthclone.qrc
SOURCES += aboutview.cpp \
    application.cpp \
    componentviewlet.cpp \
    context.cpp \
    contextmenueventfilter.cpp \
    controller.cpp \
    dialogview.cpp \
    effectjob.cpp \
    effectjobthread.cpp \
    errorview.cpp \
    helpviewlet.cpp \
    main.cpp \
    mainview.cpp \
    menuactionviewlet.cpp \
    menuitemviewlet.cpp \
    menuleafviewlet.cpp \
    menuseparatorviewlet.cpp \
    menuviewlet.cpp \
    participantview.cpp \
    participantviewlet.cpp \
    pluginmanager.cpp \
    progressbardelegate.cpp \
    progressview.cpp \
    registration.cpp \
    sampleprofile.cpp \
    samplerateconverter.cpp \
    samplerjob.cpp \
    savechangesview.cpp \
    savewarningview.cpp \
    session.cpp \
    sessionloadview.cpp \
    sessionsampledata.cpp \
    sessionviewlet.cpp \
    settings.cpp \
    signalmap.cpp \
    standarditem.cpp \
    toolviewlet.cpp \
    util.cpp \
    viewviewlet.cpp \
    zone.cpp \
    zonecomparer.cpp \
    zonecomparerproxy.cpp \
    zoneindexcomparer.cpp \
    zonelistloader.cpp \
    zonetabledelegate.cpp \
    zonetablemodel.cpp \
    zoneviewlet.cpp
TARGET = synthclone
TEMPLATE = app
VERSION = $${MAJOR_VERSION}.$${MINOR_VERSION}.$${REVISION}

################################################################################
# Install
################################################################################

unix {
    desktop.files += $${BUILDDIR}/share/applications/synthclone.desktop
    desktop.path = $${PREFIX}/share/applications
    icon.files += ../lib/images/32x32/synthclone.png
    icon.path = $${PREFIX}/share/icons/
    INSTALLS += desktop icon
}

target.path = $${SYNTHCLONE_APP_INSTALL_PATH}
INSTALLS += target
