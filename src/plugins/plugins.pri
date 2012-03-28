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

macx {
    LIBS += -F$${BUILDDIR}/$${SYNTHCLONE_LIBRARY_SUFFIX} -framework synthclone
} else {
    unix {
        # On some systems, `qmake` will reference a library installed in a
        # system library path instead of a library in an included library path.
        # I'm not sure why this happens.  'speps' over at AUR sent in a patch
        # that references the library explicitly.
        LIBS += $${BUILDDIR}/$${SYNTHCLONE_LIBRARY_SUFFIX}/libsynthclone.so.$${MAJOR_VERSION}.$${MINOR_VERSION}.$${REVISION}
    } else {
        LIBS += -L$${BUILDDIR}/$${SYNTHCLONE_LIBRARY_SUFFIX} -lsynthclone
    }
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
