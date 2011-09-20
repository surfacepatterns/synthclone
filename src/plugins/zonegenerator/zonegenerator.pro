include(../plugins.pri)

################################################################################
# Build
################################################################################

HEADERS += data.h \
    participant.h \
    plugin.h \
    view.h
MOC_DIR = $$MAKEDIR/plugins/zonegenerator
OBJECTS_DIR = $$MAKEDIR/plugins/zonegenerator
RCC_DIR = $$MAKEDIR/plugins/zonegenerator
RESOURCES += resources.qrc
SOURCES += data.cpp \
    participant.cpp \
    plugin.cpp \
    view.cpp
TARGET = $$qtLibraryTarget(synthclone_zonegenerator)
