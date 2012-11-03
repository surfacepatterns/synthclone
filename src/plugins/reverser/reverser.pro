include(../plugins.pri)

################################################################################
# Build
################################################################################

HEADERS += effect.h \
    participant.h \
    plugin.h
MOC_DIR = $${MAKEDIR}/plugins/reverser
OBJECTS_DIR = $${MAKEDIR}/plugins/reverser
RCC_DIR = $${MAKEDIR}/plugins/reverser
SOURCES += effect.cpp \
    participant.cpp \
    plugin.cpp
TARGET = $$qtLibraryTarget(synthclone_reverser)
