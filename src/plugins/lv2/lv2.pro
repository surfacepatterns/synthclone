include(../plugins.pri)

################################################################################
# Build
################################################################################

CONFIG += link_pkgconfig
HEADERS += channelmapdelegate.h \
    effect.h \
    effectview.h \
    effectviewdata.h \
    lv2instance.h \
    lv2plugin.h \
    lv2port.h \
    lv2scalepoint.h \
    lv2state.h \
    lv2uidata.h \
    lv2urimap.h \
    lv2world.h \
    menuactiondata.h \
    menuactiondatacomparer.h \
    participant.h \
    plugin.h \
    types.h
MOC_DIR = $${MAKEDIR}/plugins/lv2
OBJECTS_DIR = $${MAKEDIR}/plugins/lv2
PKGCONFIG += lilv-0 suil-0
RCC_DIR = $${MAKEDIR}/plugins/lv2
RESOURCES += lv2.qrc
SOURCES += channelmapdelegate.cpp \
    effect.cpp \
    effectview.cpp \
    effectviewdata.cpp \
    lv2instance.cpp \
    lv2plugin.cpp \
    lv2port.cpp \
    lv2scalepoint.cpp \
    lv2state.cpp \
    lv2uidata.cpp \
    lv2urimap.cpp \
    lv2world.cpp \
    menuactiondata.cpp \
    menuactiondatacomparer.cpp \
    participant.cpp \
    plugin.cpp
TARGET = $$qtLibraryTarget(synthclone_lv2)
