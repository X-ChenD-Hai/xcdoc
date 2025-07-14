add_executable(xcdoc main.cc)

add_custom_command(
    OUTPUT PreCompiledLexer.re.cc
    COMMAND ${RE2C} -o PreCompiledLexer.re.cc ${CMAKE_CURRENT_SOURCE_DIR}/PreCompiledLexer.re
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/PreCompiledLexer.re
)

target_sources(xcdoc PRIVATE PreCompiledLexer.re.cc)
target_include_directories(xcdoc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
