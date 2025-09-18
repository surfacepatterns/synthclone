include_guard()

option(SYNTHCLONE_COVERAGE_ENABLED, "enable code coverage")

function(_synthclone_configure_profiling_options target_name)
    target_compile_options(
        ${target_name}
        PRIVATE
        #-fprofile-instr-generate
        #-fcoverage-mapping
        #-fcoverage-mcdc
        --coverage
        -fno-inline-functions
        -fkeep-static-consts
        #--coverage
        #-fkeep-static-functions
        #-fkeep-inline-functions
        -g
        -O0
    )
    # target_link_options(
    #     ${target_name}
    #     PRIVATE
    #     -fprofile-instr-generate
    #     -fcoverage-mapping
    #     -fcoverage-mcdc
    # )
    target_link_options(${target_name} PRIVATE --coverage)
endfunction()

function(_synthclone_verify_target target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR, "${target_name}: invalid target name")
    endif()
endfunction()

function(synthclone_configure_library_target target_name)
    _synthclone_verify_target(${target_name})
    if(SYNTHCLONE_COVERAGE_ENABLED)
        _synthclone_configure_profiling_options(${target_name})
    endif()
endfunction()

function(synthclone_configure_test_target target_name)
    _synthclone_verify_target(${target_name})
    if(SYNTHCLONE_COVERAGE_ENABLED)
        _synthclone_configure_profiling_options(${target_name})
        # set_tests_properties(
        #     "${target_name}"
        #     PROPERTIES
        #     ENVIRONMENT
        #     "LLVM_PROFILE_FILE=${CMAKE_BINARY_DIR}/${target_name}.profraw"
        # )
    endif()
endfunction()

function(synthclone_initialize_config)
    if(SYNTHCLONE_COVERAGE_ENABLED)

        find_program(GENHTML genhtml REQUIRED)
        find_program(LCOV lcov REQUIRED)
        find_program(LLVM_COV llvm-cov-20 REQUIRED)

        add_custom_target(
            coverage

            ${LCOV}
            --all
            --branch-coverage
            --function-coverage
            --demangle-cpp
            --erase-functions __cxx_global_var_init
            --ignore-errors format,inconsistent
            --directory ${CMAKE_SOURCE_DIR}
            --base-directory ${CMAKE_SOURCE_DIR}
            --gcov-tool "${LLVM_COV}" --gcov-tool gcov
            --capture
            --no-external
            --output-file coverage.info

            COMMAND
            ${GENHTML}
            --branch-coverage
            --function-coverage
            --mcdc-coverage
            # XXX: exception? mcdc? orphan?
            --filter function,branch,brace
            --ignore-errors inconsistent,corrupt,category
            --show-proportion
            -o coverage-report
            coverage.info

            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )

    endif()
endfunction()
