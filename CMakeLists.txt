cmake_minimum_required(VERSION 4.0.0)

# Set experimental flag to enable `import std` support from CMake.  This must
# be enabled before C++ language support.
#
# This specific value changes as experimental support evolves.
if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.3.0")
    set(
        CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
        "451f2fe2-a8a2-47c3-bc32-94786d8fc91b"
    )
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0.3")
    set(
        CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
        "d0edc3af-4c50-42ea-a356-e2862fe7a444"
    )
else()
    set(
        CMAKE_EXPERIMENTAL_CXX_IMPORT_STD
        "a9e1cf81-9932-4810-974b-6eccaf14e457"
    )
endif()

set(CMAKE_CXX_COMPILER /usr/bin/clang++-20)
#set(CMAKE_CXX_COMPILER /usr/bin/clang++-21)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD 1)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# XXX: " -Weverything -Werror -pedantic"
set(SYNTHCLONE_CXX_FLAGS "--stdlib=libstdc++")

if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.3.0")
    # XXX: https://gitlab.kitware.com/cmake/cmake/-/work_items/27601
    #
    # XXX: For whatever reason, `cmake` 4.3.0 started using `-c` in places
    # where it's unused.  There are still warnings that aren't suppressed when
    # doing module scanning.
    string(
        CONCAT SYNTHCLONE_CXX_FLAGS
        "${SYNTHCLONE_CXX_FLAGS} -Wno-reserved-module-identifier "
        "-Wno-unused-command-line-argument"
    )
endif()

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
