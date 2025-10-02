project(synthclone.external.boost LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# library
###############################################################################

add_library(synthclone_external_boost OBJECT)

# module
file(GLOB synthclone-external-boost-sources src/*.cppm)
target_sources(
    synthclone_external_boost
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-external-boost-sources}
)

target_link_libraries(
    synthclone_external_boost
    PRIVATE
    boost_url
)

synthclone_configure_external_library_target(synthclone_external_boost)
