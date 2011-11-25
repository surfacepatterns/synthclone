include(../plugins.pri)

################################################################################
# Build
################################################################################

HEADERS += eventthread.h \
    participant.h \
    plugin.h \
    sampler.h \
    sampleratechangeview.h \
    semaphore.h
LIBS += -ljack
MOC_DIR = $${MAKEDIR}/plugins/jack
OBJECTS_DIR = $${MAKEDIR}/plugins/jack
RCC_DIR = $${MAKEDIR}/plugins/jack
RESOURCES += jack.qrc
SOURCES += eventthread.cpp \
    participant.cpp \
    plugin.cpp \
    sampler.cpp \
    sampleratechangeview.cpp \
    semaphore.cpp
TARGET = $$qtLibraryTarget(synthclone_jack)
