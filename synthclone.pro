include(synthclone.pri)

################################################################################
# Build
################################################################################

SUBDIRS = src
TEMPLATE = subdirs

################################################################################
# Install
################################################################################

documentation.files = $$BUILDDIR/share/doc/synthclone/*
documentation.path = $$PREFIX/share/doc/synthclone

INSTALLS += documentation
