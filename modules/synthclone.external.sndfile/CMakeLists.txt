project(synthclone.external.sndfile LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# library
###############################################################################

add_library(synthclone_external_sndfile OBJECT)

# module
file(GLOB synthclone-external-sndfile-sources src/*.cppm)
target_sources(
    synthclone_external_sndfile
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-external-sndfile-sources}
)

target_link_libraries(
    synthclone_external_sndfile
    PRIVATE
    sndfile
)

synthclone_configure_external_library_target(synthclone_external_sndfile)
