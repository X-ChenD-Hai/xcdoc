# yacc_generate(out_var out_h_var
#               [FILES] <file1.y> [<file2.y> ...]
#               [DEPS  <dep1> <dep2> ...])
#
# 输出：out_var 为所有生成的 .y.cc 文件列表
execute_process(
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/yacc"
)
include_directories("${CMAKE_BINARY_DIR}/yacc")
find_program(YACC yacc bison REQUIRED)
function(yacc_generate out_cc_var out_h_var)
    cmake_parse_arguments(ARG "" "" "FILES;DEPS" ${ARGN})
    set(gen_cc)
    set(gen_h)
    foreach(in_f IN LISTS ARG_FILES)
        get_filename_component(name "${in_f}" NAME_WE)
        set(out_cc "${CMAKE_BINARY_DIR}/yacc/${name}.tab.cc")
        set(out_h "${CMAKE_BINARY_DIR}/yacc/${name}.tab.h")

        add_custom_command(
            OUTPUT "${out_cc}" "${out_h}"
            COMMAND ${YACC} -d -H "${out_h}" -o "${out_cc}" "${in_f}"
            DEPENDS "${in_f}"
            ${ARG_DEPS}
            COMMENT "Running yacc on ${in_f}"
        )
        list(APPEND gen_cc "${out_cc}")
        list(APPEND gen_h "${out_h}")
    endforeach()

    set(${out_cc_var} ${gen_cc} PARENT_SCOPE)
    set(${out_h_var} ${gen_h} PARENT_SCOPE)
endfunction() # yacc_generate(<out_var>

