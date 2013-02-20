include(../plugins.pri)

################################################################################
# Build
################################################################################

HEADERS += effect.h \
    effectview.h \
    participant.h \
    plugin.h
MOC_DIR = $${MAKEDIR}/plugins/fader
OBJECTS_DIR = $${MAKEDIR}/plugins/fader
RCC_DIR = $${MAKEDIR}/plugins/fader
RESOURCES += fader.qrc
SOURCES += effect.cpp \
    effectview.cpp \
    participant.cpp \
    plugin.cpp
TARGET = $$qtLibraryTarget(synthclone_fader)
