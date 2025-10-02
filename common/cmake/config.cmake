include_guard()

option(SYNTHCLONE_COVERAGE_ENABLED, "enable code coverage")

function(_synthclone_add_target_coverage target_name)
    target_compile_options(
        ${target_name}
        PRIVATE
        -fprofile-instr-generate
        -fcoverage-mapping
        -fcoverage-mcdc
        -fno-inline-functions
        -fkeep-static-consts
        -g
        -O0
    )
    target_link_options(
        ${target_name}
        PRIVATE
        -fprofile-instr-generate
        -fcoverage-mapping
        -fcoverage-mcdc
    )
endfunction()

function(_synthclone_verify_target target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR, "${target_name}: invalid target name")
    endif()
endfunction()

function(synthclone_configure_application_target target_name)
    _synthclone_verify_target(${target_name})
    if(SYNTHCLONE_COVERAGE_ENABLED)
        _synthclone_add_target_coverage(${target_name})
    endif()
endfunction()

function(synthclone_configure_external_library_target target_name)
    _synthclone_verify_target(${target_name})
endfunction()

function(synthclone_configure_internal_library_target target_name)
    _synthclone_verify_target(${target_name})
    if(SYNTHCLONE_COVERAGE_ENABLED)
        _synthclone_add_target_coverage(${target_name})
    endif()
endfunction()

function(synthclone_configure_test_target target_name)
    _synthclone_verify_target(${target_name})
    if(SYNTHCLONE_COVERAGE_ENABLED)
        _synthclone_add_target_coverage(${target_name})
        set_tests_properties(
            "${target_name}"
            PROPERTIES
            ENVIRONMENT
            "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/${target_name}.profraw"
        )
    endif()
endfunction()

function(synthclone_initialize_config)
    if(SYNTHCLONE_COVERAGE_ENABLED)
        find_program(LLVM_COV llvm-cov-20 REQUIRED)
        find_program(LLVM_PROFDATA llvm-profdata-20 REQUIRED)

        add_custom_target(
            coverage

            ${LLVM_PROFDATA}
            merge
            -sparse "${CMAKE_BINARY_DIR}/*.profraw"
            -o ${CMAKE_BINARY_DIR}/synthclone.profdata

            COMMAND
            ${LLVM_COV}
            report
            -use-color=1
            -show-region-summary
            -show-branch-summary
            -show-mcdc-summary
            -instr-profile=${CMAKE_BINARY_DIR}/synthclone.profdata
            ${CMAKE_BINARY_DIR}/build/bin/*

            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endif()
endfunction()
