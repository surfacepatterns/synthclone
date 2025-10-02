project(synthclone.external.samplerate LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# library
###############################################################################

add_library(synthclone_external_samplerate OBJECT)

# module
file(GLOB synthclone-external-samplerate-sources src/*.cppm)
target_sources(
    synthclone_external_samplerate
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-external-samplerate-sources}
)

target_link_libraries(
    synthclone_external_samplerate
    PRIVATE
    samplerate
)

synthclone_configure_external_library_target(synthclone_external_samplerate)
