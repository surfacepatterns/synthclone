project(synthclone.external.kissfft LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# library
###############################################################################

add_library(synthclone_external_kissfft OBJECT)

# module
file(GLOB synthclone-external-kissfft-sources src/*.cppm)
target_sources(
    synthclone_external_kissfft
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-external-kissfft-sources}
)

target_link_libraries(
    synthclone_external_kissfft
    PRIVATE
    kissfft-float
)

synthclone_configure_external_library_target(synthclone_external_kissfft)
