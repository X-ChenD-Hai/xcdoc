add_custom_command(
    OUTPUT CppParser/PreCompiledLexer.re.cc
    COMMAND ${RE2C} -o CppParser/PreCompiledLexer.re.cc ${CMAKE_CURRENT_SOURCE_DIR}/CppParser/PreCompiledLexer.re
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/CppParser/PreCompiledLexer.re
    ${CMAKE_CURRENT_SOURCE_DIR}/CppParser/def.re
)
add_custom_command(
    OUTPUT CppParser/MacroExpandMacroHelper.re.cc
    COMMAND ${RE2C} -o CppParser/MacroExpandMacroHelper.re.cc ${CMAKE_CURRENT_SOURCE_DIR}/CppParser/MacroExpandMacroHelper.re
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/CppParser/MacroExpandMacroHelper.re
    ${CMAKE_CURRENT_SOURCE_DIR}/CppParser/def.re
)

add_library(xcdoc_lib STATIC)
target_include_directories(xcdoc_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(xcdoc_lib PRIVATE
    CppParser/PreCompiledLexer.re.cc
    CppParser/MacroExpandMacroHelper.re.cc
    utils/string_slice_view.cc
)
