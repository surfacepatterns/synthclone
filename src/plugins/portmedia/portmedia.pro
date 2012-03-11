include(../plugins.pri)

################################################################################
# Build
################################################################################

HEADERS += channelmapdelegate.h \
    eventthread.h \
    midithread.h \
    participant.h \
    plugin.h \
    ringbuffer.h \
    sampler.h \
    samplerview.h \
    portaudio/pa_memorybarrier.h \
    portaudio/pa_ringbuffer.h
LIBS += -lportaudio -lportmidi
MOC_DIR = $${MAKEDIR}/plugins/portmedia
OBJECTS_DIR = $${MAKEDIR}/plugins/portmedia
RCC_DIR = $${MAKEDIR}/plugins/portmedia
RESOURCES += portmedia.qrc
SOURCES += channelmapdelegate.cpp \
    eventthread.cpp \
    midithread.cpp \
    participant.cpp \
    plugin.cpp \
    sampler.cpp \
    samplerview.cpp \
    portaudio/pa_ringbuffer.c
TARGET = $$qtLibraryTarget(synthclone_portmedia)
