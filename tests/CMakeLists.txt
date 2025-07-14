
add_custom_command(
    OUTPUT re.re.cc
    COMMAND ${RE2C} -o re.re.cc ${CMAKE_CURRENT_SOURCE_DIR}/re.re --utf-8
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/re.re
)

add_executable(test_re re.re.cc)
