add_custom_command(
    OUTPUT PreCompiledLexer.re.cc
    COMMAND ${RE2C} -o PreCompiledLexer.re.cc ${CMAKE_CURRENT_SOURCE_DIR}/PreCompiledLexer.re
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/PreCompiledLexer.re
    ${CMAKE_CURRENT_SOURCE_DIR}/def.re
)

add_library(xcdoc_lib STATIC)
target_include_directories(xcdoc_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(xcdoc_lib PRIVATE
    PreCompiledLexer.re.cc
)
