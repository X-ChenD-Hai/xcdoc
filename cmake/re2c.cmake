# re2c_generate(<out_var>
#               [FILES] <file1.re> [<file2.re> ...]
#               [DEPS  <dep1> <dep2> ...])
#
# 输出：out_var 为所有生成的 .re.cc 文件列表
find_program(RE2C re2c REQUIRED)
function(re2c_generate out_var)
    cmake_parse_arguments(ARG "" "" "FILES;DEPS" ${ARGN})

    set(generated)
    foreach(in_f IN LISTS ARG_FILES)
        get_filename_component(name "${in_f}" NAME_WE)
        set(out_cc "${CMAKE_BINARY_DIR}/re2c/${name}.re.cc")

        add_custom_command(
            OUTPUT "${out_cc}"
            COMMAND ${RE2C} -o "${out_cc}" "${in_f}"
            DEPENDS "${in_f}"
            ${ARG_DEPS}
            COMMENT "Running re2c on ${in_f}"
        )
        list(APPEND generated "${out_cc}")
    endforeach()

    set(${out_var} ${generated} PARENT_SCOPE)
endfunction()# re2c_generate(<out_var>
#               [FILES] <file1.re> [<file2.re> ...]
#               [DEPS  <dep1> <dep2> ...])
#
# 输出：out_var 为所有生成的 .re.cc 文件列表
function(re2c_generate out_var)
    cmake_parse_arguments(ARG "" "" "FILES;DEPS" ${ARGN})

    set(generated)
    foreach(in_f IN LISTS ARG_FILES)
        get_filename_component(name "${in_f}" NAME_WE)
        set(out_cc "${CMAKE_BINARY_DIR}/re2c/${name}.re.cc")

        add_custom_command(
            OUTPUT "${out_cc}"
            COMMAND ${RE2C} -o "${out_cc}" "${in_f}"
            DEPENDS "${in_f}"
            ${ARG_DEPS}
            COMMENT "Running re2c on ${in_f}"
        )
        list(APPEND generated "${out_cc}")
    endforeach()

    set(${out_var} ${generated} PARENT_SCOPE)
endfunction()