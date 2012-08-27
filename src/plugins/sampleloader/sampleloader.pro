include(../plugins.pri)

################################################################################
# Build
################################################################################

HEADERS += participant.h \
    plugin.h
MOC_DIR = $${MAKEDIR}/plugins/sampleloader
OBJECTS_DIR = $${MAKEDIR}/plugins/sampleloader
RCC_DIR = $${MAKEDIR}/plugins/sampleloader
SOURCES += participant.cpp \
    plugin.cpp
TARGET = $$qtLibraryTarget(synthclone_sampleloader)
