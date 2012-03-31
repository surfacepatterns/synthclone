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

CONFIG += uitools
DESTDIR = $${BUILDDIR}/$${SYNTHCLONE_LIBRARY_SUFFIX}
HEADERS += closeeventfilter.h \
    ../include/synthclone/component.h \
    ../include/synthclone/context.h \
    ../include/synthclone/designerview.h \
    ../include/synthclone/effect.h \
    ../include/synthclone/effectjob.h \
    ../include/synthclone/error.h \
    ../include/synthclone/fileselectionview.h \
    ../include/synthclone/iplugin.h \
    ../include/synthclone/menuaction.h \
    ../include/synthclone/menuitem.h \
    ../include/synthclone/menuseparator.h \
    ../include/synthclone/participant.h \
    ../include/synthclone/registration.h \
    ../include/synthclone/sample.h \
    ../include/synthclone/samplecopier.h \
    ../include/synthclone/sampleinputstream.h \
    ../include/synthclone/sampleoutputstream.h \
    ../include/synthclone/sampler.h \
    ../include/synthclone/samplerjob.h \
    ../include/synthclone/samplestream.h \
    ../include/synthclone/semaphore.h \
    ../include/synthclone/target.h \
    ../include/synthclone/types.h \
    ../include/synthclone/util.h \
    ../include/synthclone/view.h \
    ../include/synthclone/zone.h \
    ../include/synthclone/zonecomparer.h
INCLUDEPATH += ../include
LIBS += -lsndfile
MOC_DIR = $${MAKEDIR}/lib
OBJECTS_DIR = $${MAKEDIR}/lib
RCC_DIR = $${MAKEDIR}/lib
RESOURCES += lib.qrc
SOURCES += closeeventfilter.cpp \
    component.cpp \
    context.cpp \
    designerview.cpp \
    effect.cpp \
    effectjob.cpp \
    error.cpp \
    fileselectionview.cpp \
    menuaction.cpp \
    menuitem.cpp \
    menuseparator.cpp \
    participant.cpp \
    registration.cpp \
    sample.cpp \
    samplecopier.cpp \
    sampleinputstream.cpp \
    sampleoutputstream.cpp \
    sampler.cpp \
    samplerjob.cpp \
    samplestream.cpp \
    semaphore.cpp \
    target.cpp \
    util.cpp \
    view.cpp \
    zone.cpp \
    zonecomparer.cpp
TARGET = synthclone
TEMPLATE = lib
VERSION = $${SYNTHCLONE_VERSION}
VER_MAJ = $${MAJOR_VERSION}
VER_MIN = $${MINOR_VERSION}
VER_PAT = $${REVISION}

################################################################################
# Install
################################################################################

headers.files = $${HEADERS}
headers.files -= closeeventfilter.h
exists(../include/synthclone/config.h) {
    headers.files += ../include/synthclone/config.h
}

target.path = $${SYNTHCLONE_LIBRARY_INSTALL_PATH}

macx {

    # Install library and headers as a framework on Mac OSX.
    CONFIG += lib_bundle x86 x86_64
    QMAKE_FRAMEWORK_BUNDLE_NAME = synthclone
    QMAKE_FRAMEWORK_VERSION = $${SYNTHCLONE_VERSION}
    headers.path = Headers
    headers.version = Versions
    isEmpty(SKIP_HEADERS) {
        QMAKE_BUNDLE_DATA += headers
    }

    # We can't guarantee that the shared libraries we use are going to be
    # available on all platforms.  So, we package them with the framework
    # bundle.  The libraries are installed via build.py.
    libraries.files = $${BUILDDIR}/$${SYNTHCLONE_LIBRARY_SUFFIX}/synthclone.framework/Libraries/*
    libraries.path = $${SYNTHCLONE_LIBRARY_INSTALL_PATH}/synthclone.framework/Libraries
    INSTALLS += libraries

} else {
    headers.path = $${SYNTHCLONE_HEADER_INSTALL_PATH}
    isEmpty(SKIP_HEADERS) {
        INSTALLS += headers
    }
}

INSTALLS += target
