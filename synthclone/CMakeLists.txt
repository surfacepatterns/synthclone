project(synthclone.main LANGUAGES CXX)

include(../common/cmake/config.cmake)

###############################################################################
# binary
###############################################################################

add_executable(synthclone)
add_dependencies(synthclone synthclone_app)
file(GLOB synthclone-sources src/*.cpp)
target_sources(
    synthclone
    PUBLIC
    ${synthclone-sources}
)

target_link_libraries(
    synthclone
    synthclone_app
    synthclone_core
    synthclone_external_boost
    synthclone_external_posix
    synthclone_external_qt
    synthclone_external_samplerate
    synthclone_external_sndfile
    synthclone_external_utf8proc
    synthclone_qt
    synthclone_util
    stdc++exp
)

synthclone_configure_application_target(synthclone)
