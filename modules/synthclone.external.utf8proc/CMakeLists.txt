project(synthclone.external.utf8proc LANGUAGES CXX)

include(../../common/cmake/config.cmake)

###############################################################################
# library
###############################################################################

add_library(synthclone_external_utf8proc OBJECT)

# module
file(GLOB synthclone-external-utf8proc-sources src/*.cppm)
target_sources(
    synthclone_external_utf8proc
    PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES ${synthclone-external-utf8proc-sources}
)

target_link_libraries(
    synthclone_external_utf8proc
    PRIVATE
    utf8proc
)

synthclone_configure_external_library_target(synthclone_external_utf8proc)
