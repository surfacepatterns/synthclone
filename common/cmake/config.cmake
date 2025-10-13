include_guard()

###############################################################################
# SYNTHCLONE_COVERAGE_ENABLED
###############################################################################

option(SYNTHCLONE_COVERAGE_ENABLED, "enable code coverage")

###############################################################################
# synthclone_configure_app()
###############################################################################

function(_synthclone_add_coverage target)
    target_compile_options(
        ${target}
        PRIVATE
        -fprofile-instr-generate
        -fcoverage-mapping
        -fcoverage-mcdc
    )
    target_link_options(
        ${target}
        PRIVATE
        -fprofile-instr-generate
        -fcoverage-mapping
        -fcoverage-mcdc
    )
endfunction()

function(_synthclone_configure_coverage target)
    if(SYNTHCLONE_COVERAGE_ENABLED)
        _synthclone_add_coverage(${target})
    endif()
endfunction()

function(_synthclone_verify_target target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR, "${target}: invalid target name")
    endif()
endfunction()

function(synthclone_configure_app target)
    _synthclone_verify_target(${target})
    _synthclone_configure_coverage(${target})
endfunction()

###############################################################################
# synthclone_configure_module()
###############################################################################

function(_synthclone_configure_shared_element target)
    target_compile_options(${target} PRIVATE -fPIC)
    target_link_options(${target} PRIVATE -fPIC)
endfunction()

function(synthclone_configure_module target build_type)
    _synthclone_verify_target(${target})
    if(build_type STREQUAL "shared")
        _synthclone_configure_shared_element(${target})
    elseif(NOT (build_type STREQUAL "object"))
        message(FATAL_ERROR, "${build_type}: invalid module build type")
    endif()
    _synthclone_configure_coverage(${target})
endfunction()

###############################################################################
# synthclone_configure_plugin()
###############################################################################

function(synthclone_configure_plugin target)
    _synthclone_verify_target(${target})
    _synthclone_configure_coverage(${target})
    _synthclone_configure_shared_element(${target})
endfunction()

###############################################################################
# synthclone_configure_project()
###############################################################################

macro(_synthclone_populate_tests tests dir)
    get_property(subdirs DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirs})
        _synthclone_populate_tests(${tests} ${subdir})
    endforeach()
    get_property(dir_tests DIRECTORY ${dir} PROPERTY TESTS)
    list(APPEND tests ${dir_tests})
endmacro()

function(_synthclone_extract_tests tests_var)
    set(tests)
    _synthclone_populate_tests(tests ${CMAKE_SOURCE_DIR})
    set(${tests_var} ${tests} PARENT_SCOPE)
endfunction()

function(synthclone_configure_project)
    if(SYNTHCLONE_COVERAGE_ENABLED)
        find_program(LLVM_COV llvm-cov-20 REQUIRED)
        find_program(LLVM_PROFDATA llvm-profdata-20 REQUIRED)

        # Collect the tests throughout the project, and prepend an `-object`
        # argument to each test after the first test to satisfy the weirdness
        # of `llvm-cov`.
        _synthclone_extract_tests(test_args)
        list(TRANSFORM test_args PREPEND "${CMAKE_BINARY_DIR}/build/bin/")
        list(LENGTH test_args tests_length)
        if (${tests_length} GREATER_EQUAL "2")
            math(EXPR end_index "${tests_length} - 2")
            foreach(i RANGE ${end_index})
                math(EXPR insert_index "(${i} * 2) + 1")
                list(INSERT test_args ${insert_index} "-object")
            endforeach()
        endif()

        add_custom_target(
            coverage

            ${LLVM_PROFDATA}
            merge
            -sparse "${_SYNTHCLONE_RAW_COVERAGE_DIR}/*.profraw"
            -o ${_SYNTHCLONE_MERGED_COVERAGE_FILE}

            COMMAND
            ${LLVM_COV}
            show
            -show-branches=count
            -show-directory-coverage=1
            -show-instantiations=0
            -show-mcdc=1
            -format=html
            -output-dir=${_SYNTHCLONE_COVERAGE_DIR}/report
            -instr-profile=${_SYNTHCLONE_MERGED_COVERAGE_FILE}
            ${test_args}
            -sources
            ${CMAKE_SOURCE_DIR}/apps/*/src/*.cpp
            ${CMAKE_SOURCE_DIR}/modules/*/src/*.cppm
            ${CMAKE_SOURCE_DIR}/modules/*/tests/*.cpp
            ${CMAKE_SOURCE_DIR}/plugins/*/src/*.cpp
            ${CMAKE_BINARY_DIR}/modules/*/modules/*.cppm

            COMMAND
            ${LLVM_COV}
            report
            -show-region-summary
            -show-branch-summary
            -show-mcdc-summary
            -use-color=1
            -instr-profile=${_SYNTHCLONE_MERGED_COVERAGE_FILE}
            ${test_args}

            COMMAND_EXPAND_LISTS
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

        file(MAKE_DIRECTORY ${_SYNTHCLONE_COVERAGE_DIR})
        file(MAKE_DIRECTORY ${_SYNTHCLONE_RAW_COVERAGE_DIR})

    endif()
endfunction()

###############################################################################
# synthclone_configure_test()
###############################################################################

set(_SYNTHCLONE_COVERAGE_DIR "${CMAKE_BINARY_DIR}/coverage")
set(
    _SYNTHCLONE_MERGED_COVERAGE_FILE
    "${_SYNTHCLONE_COVERAGE_DIR}/merged.profdata"
)
set(_SYNTHCLONE_RAW_COVERAGE_DIR "${_SYNTHCLONE_COVERAGE_DIR}/raw")

function(synthclone_configure_test target)
    _synthclone_verify_target(${target})
    if(SYNTHCLONE_COVERAGE_ENABLED)
        _synthclone_add_coverage(${target})
        set(profile_file "${_SYNTHCLONE_RAW_COVERAGE_DIR}/${target}.profraw")
        set_tests_properties(
            "${target}"
            PROPERTIES
            ENVIRONMENT
            "LLVM_PROFILE_FILE=${profile_file}"
        )
    endif()

    list(APPEND _SYNTHCLONE_TEST_LIST "${target}")
    set(_SYNTHCLONE_TEST_LIST _SYNTHCLONE_TEST_LIST PARENT_SCOPE)

endfunction()
