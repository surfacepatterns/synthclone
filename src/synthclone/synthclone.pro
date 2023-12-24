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

unix:!macx {
    # On some systems, `qmake` will reference a library installed in a system
    # library path instead of a library in an included library path.  I'm not
    # sure why this happens.  'speps' over at AUR sent in a patch that
    # references the library explicitly.
    LIB_BUILDDIR = $${BUILDDIR}/$${SYNTHCLONE_LIBRARY_SUFFIX}
    LIB_VERSION = $${MAJOR_VERSION}.$${MINOR_VERSION}.$${REVISION}
    LIBS += -lsamplerate $${LIB_BUILDDIR}/libsynthclone.so.$${LIB_VERSION}
} else {
    LIBS += -L$${BUILDDIR}/$${SYNTHCLONE_LIBRARY_SUFFIX} -lsamplerate \
        -lsynthclone
}

CONFIG += console
DEFINES += SYNTHCLONE_MAJOR_VERSION=$${MAJOR_VERSION} \
    SYNTHCLONE_MINOR_VERSION=$${MINOR_VERSION} \
    SYNTHCLONE_PLUGIN_PATH=$${SYNTHCLONE_PLUGIN_INSTALL_PATH} \
    SYNTHCLONE_REVISION=$${REVISION}
DESTDIR = $${BUILDDIR}/$${SYNTHCLONE_APP_SUFFIX}
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
    menumanager.h \
    menuseparatorviewlet.h \
    menuviewlet.h \
    participantmanager.h \
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
    standarditem.h \
    toolviewlet.h \
    types.h \
    util.h \
    viewviewlet.h \
    zone.h \
    zonecomparer.h \
    zonecomparerproxy.h \
    zoneindexcomparer.h \
    zonelistloader.h \
    zonetabledelegate.h \
    zonetablemodel.h \
    zoneviewlet.h
INCLUDEPATH += ../include
MOC_DIR = $${MAKEDIR}/synthclone
OBJECTS_DIR = $${MAKEDIR}/synthclone
QT += uitools xml
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
    menumanager.cpp \
    menuseparatorviewlet.cpp \
    menuviewlet.cpp \
    participantmanager.cpp \
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
VERSION = $${SYNTHCLONE_VERSION}

################################################################################
# Install
################################################################################

target.path = $${SYNTHCLONE_APP_INSTALL_PATH}
INSTALLS += target
