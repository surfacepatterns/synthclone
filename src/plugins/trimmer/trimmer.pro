include(../plugins.pri)

################################################################################
# Build
################################################################################

HEADERS += effect.h \
    effectview.h \
    participant.h \
    plugin.h
MOC_DIR = $$MAKEDIR/plugins/trimmer
OBJECTS_DIR = $$MAKEDIR/plugins/trimmer
RCC_DIR = $$MAKEDIR/plugins/trimmer
RESOURCES += trimmer.qrc
SOURCES += effect.cpp \
    effectview.cpp \
    participant.cpp \
    plugin.cpp
TARGET = $$qtLibraryTarget(synthclone_trimmer)
