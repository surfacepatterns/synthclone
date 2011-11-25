include(../plugins.pri)

################################################################################
# Build
################################################################################

HEADERS += controllayer.h \
    controllayerdelegate.h \
    participant.h \
    plugin.h \
    target.h \
    targetview.h \
    types.h
MOC_DIR = $${MAKEDIR}/plugins/sfz
OBJECTS_DIR = $${MAKEDIR}/plugins/sfz
RCC_DIR = $${MAKEDIR}/plugins/sfz
RESOURCES += sfz.qrc
SOURCES += controllayer.cpp \
    controllayerdelegate.cpp \
    participant.cpp \
    plugin.cpp \
    target.cpp \
    targetview.cpp
TARGET = $$qtLibraryTarget(synthclone_sfz)
