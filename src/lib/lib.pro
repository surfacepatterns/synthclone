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

DESTDIR = $$BUILDDIR/lib
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
    ../include/synthclone/target.h \
    ../include/synthclone/types.h \
    ../include/synthclone/util.h \
    ../include/synthclone/view.h \
    ../include/synthclone/zone.h \
    ../include/synthclone/zonecomparer.h
INCLUDEPATH += ../include
LIBS += -lsndfile
MOC_DIR = $$MAKEDIR/lib
OBJECTS_DIR = $$MAKEDIR/lib
RCC_DIR = $$MAKEDIR/lib
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
    target.cpp \
    util.cpp \
    view.cpp \
    zone.cpp \
    zonecomparer.cpp
TARGET = synthclone
TEMPLATE = lib
VERSION = $$MAJOR_VERSION.$$MINOR_VERSION.$$REVISION
VER_MAJ = $$MAJOR_VERSION
VER_MIN = $$MINOR_VERSION
VER_PAT = $$REVISION

################################################################################
# Install
################################################################################

headers.files = $$HEADERS
headers.files += $$BUILDDIR/include/synthclone/config.h
headers.files -= closeeventfilter.h

target.path = $$SYNTHCLONE_LIBRARY_INSTALL_PATH

macx {
    # XXX: Does this work?
    headers.path = Headers
    headers.version = Versions
    CONFIG += lib_bundle ppc x86
    isEmpty(SKIP_HEADERS) {
        QMAKE_BUNDLE_DATA += headers
    }
} else {
    headers.path = $$SYNTHCLONE_HEADER_INSTALL_PATH/synthclone
}

INSTALLS += target
isEmpty(SKIP_HEADERS) {
    INSTALLS += headers
}
