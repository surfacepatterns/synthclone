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

set(
    CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} --stdlib=libstdc++" #" -Weverything -Werror -pedantic"
)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

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

include(cmake/config.cmake)

enable_testing()

synthclone_initialize_config()

#add_subdirectory(app)
add_subdirectory(lib)
