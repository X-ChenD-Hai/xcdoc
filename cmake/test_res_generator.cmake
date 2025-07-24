find_program(NINJA_EXE NAMES ninja)
if(NOT NINJA_EXE OR NINJA_EXE STREQUAL "NINJA_EXE-NOTFOUND")
    message(WARNING "Ninja not found, using make instead")
    if(UNIX)
        find_program(MAKE_EXE NAMES make)
        set(TEST_GENERTOR_TYPE "Unix Makefiles")
    elseif(WIN32)
        find_program(MAKE_EXE NAMES mingw32-make)
        set(TEST_GENERTOR_TYPE "MinGW Makefiles")
    endif()
else()
    set(TEST_GENERTOR_TYPE "Ninja")
endif()
function(__add_test_res def_key root_dir CC CXX generator_dirs)
    if(CC AND CXX AND NOT CC STREQUAL "CC-NOTFOUND" AND NOT CXX STREQUAL "CXX-NOTFOUND")
        set(g ${${generator_dirs}})
        execute_process(
            COMMAND ${CMAKE_COMMAND}
            -S ${source_dir}
            -B ${CMAKE_BINARY_DIR}/tests/res/build/${root_dir}
            -G ${TEST_GENERTOR_TYPE}
            -D "CMAKE_C_COMPILER=${CC}"
            -D "CMAKE_CXX_COMPILER=${CXX}"
        )
        list(APPEND g ${CMAKE_BINARY_DIR}/tests/res/build/${root_dir})
        add_compile_definitions(${def_key}=\"${CMAKE_BINARY_DIR}/tests/res/build/${root_dir}/compile_commands.json\")
        set(${generator_dirs} ${g} PARENT_SCOPE)
    endif()
endfunction(__add_test_res)

function(generate_test_res source_dir generator_dirs)
    set(g "")
    message(STATUS "Generating test resources...")
    find_program(CLANG NAMES clang)
    find_program(CLANGXX NAMES clang++)
    set(root_dir clang)
    __add_test_res("CLANG_COMPILE_DATABASE" ${root_dir} ${CLANG} ${CLANGXX} g)
    find_program(CLANG_CL NAMES clang-cl)
    set(root_dir clang-cl)
    __add_test_res("CLANG_CL_COMPILE_DATABASE" ${root_dir} ${CLANG_CL} ${CLANG_CL} g)
    find_program(CC NAMES gcc)
    find_program(CXX NAMES g++)
    set(root_dir gcc)
    __add_test_res("GCC_COMPILE_DATABASE" ${root_dir} ${CC} ${CXX} g)
    set(${generator_dirs} ${g} PARENT_SCOPE)
endfunction(generate_test_res)

