include_guard()

option(SYNTHCLONE_COVERAGE_ENABLED, "enable code coverage")

function(synthclone_configure_target_coverage target_name)

    if(NOT TARGET ${target_name})
        message(FATAL_ERROR, "${target_name}: invalid target name")
    endif()

    if(SYNTHCLONE_COVERAGE_ENABLED)
        target_compile_options(
            ${target_name}
            PRIVATE
            --coverage
            -fno-inline-functions
            -fkeep-static-consts
            #--coverage
            #-fkeep-static-functions
            #-fkeep-inline-functions
            -g
            -O0
        )
        target_link_options(${target_name} PRIVATE --coverage)
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
            --branch-coverage
            --function-coverage
            --demangle-cpp
            --erase-functions __cxx_global_var_init
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
            #--demangle-cpp
            --filter function,branch
            --ignore-errors inconsistent,corrupt,category
            --show-proportion
            -o coverage-report
            coverage.info

            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )

    endif()
endfunction()
