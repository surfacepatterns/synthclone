include(../plugins.pri)

################################################################################
# Build
################################################################################

HEADERS += archiveheader.h \
    archivereader.h \
    archivewriter.h \
    importer.h \
    participant.h \
    plugin.h \
    target.h \
    targetview.h \
    temporarydir.h \
    types.h \
    velocitycomparer.h \
    zonekey.h
LIBS += -larchive
MOC_DIR = $${MAKEDIR}/plugins/hydrogen
OBJECTS_DIR = $${MAKEDIR}/plugins/hydrogen
RCC_DIR = $${MAKEDIR}/plugins/hydrogen
RESOURCES += hydrogen.qrc
SOURCES += archiveheader.cpp \
    archivereader.cpp \
    archivewriter.cpp \
    importer.cpp \
    participant.cpp \
    plugin.cpp \
    target.cpp \
    targetview.cpp \
    temporarydir.cpp \
    velocitycomparer.cpp \
    zonekey.cpp
TARGET = $$qtLibraryTarget(synthclone_hydrogen)
