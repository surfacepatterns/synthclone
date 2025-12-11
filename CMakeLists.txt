cmake_minimum_required(VERSION 4.0.0)

# Set experimental flag to enable `import std` support from CMake.  This must
# be enabled before C++ language support.
#
# This specific value changes as experimental support evolves.
#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
#set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

set(CMAKE_CXX_COMPILER /usr/bin/clang++-20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#" -Weverything -Werror -pedantic"
set(SYNTHCLONE_CXX_FLAGS "--stdlib=libstdc++")
if(NOT DEFINED CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS ${SYNTHCLONE_CXX_FLAGS})
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SYNTHCLONE_CXX_FLAGS}")
endif()

set(SYNTHCLONE_BUILD_DIR "${CMAKE_BINARY_DIR}/build")
set(SYNTHCLONE_HEADER_OUTPUT_DIRECTORY "${SYNTHCLONE_BUILD_DIR}/include")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${SYNTHCLONE_BUILD_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${SYNTHCLONE_BUILD_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SYNTHCLONE_BUILD_DIR}/bin")

set(CMAKE_COMPILE_WARNING_AS_ERROR 1)

set(CTEST_OUTPUT_ON_FAILURE 1)

###############################################################################
# Root project
###############################################################################

project(
    synthclone
    VERSION 0.5.0
    HOMEPAGE_URL https://github.com/surfacepatterns/synthclone
    LANGUAGES CXX
)

include(CTest)
include(GNUInstallDirs)

include(common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core Qml)

qt_standard_project_setup(REQUIRES 6.10)

# "Enabling this policy ensures that your QML module is placed under a default
# import path, and its types can be found without manual calls to
# QQmlEngine::addImportPath.":
#
#     https://doc.qt.io/qt-6/qt-cmake-policy-qtp0001.html
qt_policy(SET QTP0001 NEW)

enable_testing()

add_subdirectory(apps)
add_subdirectory(modules)
add_subdirectory(plugins)

synthclone_configure_project()
