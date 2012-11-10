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
LIBS += -lzip
MOC_DIR = $${MAKEDIR}/plugins/renoise
OBJECTS_DIR = $${MAKEDIR}/plugins/renoise
RCC_DIR = $${MAKEDIR}/plugins/renoise
RESOURCES += renoise.qrc
SOURCES += archivewriter.cpp \
    participant.cpp \
    plugin.cpp \
    target.cpp \
    targetview.cpp \
    velocitycomparer.cpp \
    zonekey.cpp
TARGET = $$qtLibraryTarget(synthclone_renoise)
