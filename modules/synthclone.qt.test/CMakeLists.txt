project(synthclone.qt.test LANGUAGES CXX)

include(../../common/cmake/config.cmake)

find_package(Qt6 REQUIRED COMPONENTS Core)

###############################################################################
# library
###############################################################################

# You might ask yourself why this "library" contains a single resource and no
# real code.  The answer is that `qt_add_executable`, which is required to add
# resources to an executable using `qt_add_resources`, *breaks* module imports.

qt_add_library(synthclone_qt_test OBJECT)
add_dependencies(
    synthclone_qt_test
    synthclone_external_qt
)

# resource files
qt_add_resources(
    synthclone_qt_test "synthclone.qt.test"
    PREFIX "/synthclone.qt.test"
    BASE "data"
    FILES
    "data/qresource/foo"
)

target_link_libraries(
    synthclone_qt_test
    PRIVATE
    synthclone_external_qt
    Qt6::Core
)

synthclone_configure_internal_library_target(synthclone_qt_test)
