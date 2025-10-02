project(synthclone.external.qt LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

###############################################################################
# library
###############################################################################

add_library(synthclone_external_qt OBJECT)

# module
file(GLOB synthclone-external-qt-sources src/*.cppm)
target_sources(
    synthclone_external_qt
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-external-qt-sources}
)

target_link_libraries(
    synthclone_external_qt
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

synthclone_configure_external_library_target(synthclone_external_qt)
