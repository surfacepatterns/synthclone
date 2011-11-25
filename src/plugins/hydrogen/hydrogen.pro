include(../plugins.pri)

################################################################################
# Build
################################################################################

HEADERS += archivewriter.h \
    participant.h \
    plugin.h \
    target.h \
    targetview.h \
    types.h \
    velocitycomparer.h \
    zonekey.h
LIBS += -larchive
MOC_DIR = $${MAKEDIR}/plugins/hydrogen
OBJECTS_DIR = $${MAKEDIR}/plugins/hydrogen
RCC_DIR = $${MAKEDIR}/plugins/hydrogen
RESOURCES += hydrogen.qrc
SOURCES += archivewriter.cpp \
    participant.cpp \
    plugin.cpp \
    target.cpp \
    targetview.cpp \
    velocitycomparer.cpp \
    zonekey.cpp
TARGET = $$qtLibraryTarget(synthclone_hydrogen)
