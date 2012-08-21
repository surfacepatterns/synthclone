include(../../synthclone.pri)

################################################################################
# Build
################################################################################

isEmpty(BUILDDIR) {
    BUILDDIR = ../../../build
}

isEmpty(MAKEDIR) {
    MAKEDIR = ../../../make
}

unix:!macx {
    # On some systems, `qmake` will reference a library installed in a system
    # library path instead of a library in an included library path.  I'm not
    # sure why this happens.  'speps' over at AUR sent in a patch that
    # references the library explicitly.
    LIB_BUILDDIR = $${BUILDDIR}/$${SYNTHCLONE_LIBRARY_SUFFIX}
    LIB_VERSION = $${MAJOR_VERSION}.$${MINOR_VERSION}.$${REVISION}
    LIBS += $${LIB_BUILDDIR}/libsynthclone.so.$${LIB_VERSION}
} else {
    LIBS += -L$${BUILDDIR}/$${SYNTHCLONE_LIBRARY_SUFFIX} -lsynthclone
}

CONFIG += plugin
DESTDIR = $${BUILDDIR}/$${SYNTHCLONE_PLUGIN_SUFFIX}
INCLUDEPATH += ../../include
TEMPLATE = lib

################################################################################
# Install
################################################################################

target.path = $${SYNTHCLONE_PLUGIN_INSTALL_PATH}
INSTALLS += target
