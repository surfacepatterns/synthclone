project(synthclone.app LANGUAGES CXX)

include(../../common/cmake/config.cmake)

#find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

###############################################################################
# library
###############################################################################

qt_add_library(synthclone_app OBJECT)
add_dependencies(
    synthclone_app
    synthclone_core
    synthclone_external_qt
    synthclone_qt
    synthclone_util
)

# cmake build information submodule
configure_file(
    "data/templates/cmake.cppm.tmpl"
    "${CMAKE_CURRENT_BINARY_DIR}/modules/cmake.cppm"
)

# ui files
qt_add_ui(
    synthclone_app
    SOURCES
    data/ui/about_dialog.ui
)

# resource files
qt_add_resources(
    synthclone_app "synthclone.app"
    PREFIX "/synthclone.app"
    BASE "../.."
    FILES
    "../../LICENSE.md"
    "../../README.md"
)

# module
file(
    GLOB
    synthclone-app-sources
    src/*.cppm
    ${CMAKE_CURRENT_BINARY_DIR}/modules/*.cppm
)
target_sources(
    synthclone_app
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    BASE_DIRS
    src
    ${CMAKE_CURRENT_BINARY_DIR}/modules
    FILES ${synthclone-app-sources}
)

target_include_directories(
    synthclone_app
    PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

target_link_libraries(
    synthclone_app
    PRIVATE
    synthclone_core
    synthclone_external_boost
    synthclone_external_posix
    synthclone_external_qt
    synthclone_external_samplerate
    synthclone_external_sndfile
    synthclone_external_utf8proc
    synthclone_qt
    synthclone_util
    Qt6::Core
    Qt6::Widgets
)

synthclone_configure_internal_library_target(synthclone_app)
