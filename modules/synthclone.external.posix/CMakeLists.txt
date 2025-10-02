project(synthclone.external.posix LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# library
###############################################################################

add_library(synthclone_external_posix OBJECT)

# module
file(GLOB synthclone-external-posix-sources src/*.cppm)
target_sources(
    synthclone_external_posix
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-external-posix-sources}
)

synthclone_configure_external_library_target(synthclone_external_posix)
