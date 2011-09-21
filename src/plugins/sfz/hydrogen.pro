include(../plugins.pri)

################################################################################
# Build
################################################################################

HEADERS += directoryview.h \
    layeralgorithm.h \
    participant.h \
    plugin.h \
    sampleformat.h \
    target.h \
    targetview.h \
    velocitycomparer.h \
    zonekey.h
MOC_DIR = $$MAKEDIR/plugins/hydrogen
OBJECTS_DIR = $$MAKEDIR/plugins/hydrogen
RCC_DIR = $$MAKEDIR/plugins/hydrogen
RESOURCES += resources.qrc
SOURCES += directoryview.cpp \
    participant.cpp \
    plugin.cpp \
    target.cpp \
    targetview.cpp \
    velocitycomparer.cpp \
    zonekey.cpp
TARGET = $$qtLibraryTarget(synthclone_hydrogen)
