add_library(xcdoc_lib STATIC)
target_include_directories(xcdoc_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xcdoc_lib PUBLIC
    nlohmann_json::nlohmann_json
)
set(UTILS_SOURCE
    # utils
    utils/public.cc
    utils/string_slice_view.cc
)
re2c_generate(
    CPP_PARSER_RE2C_SOURCES
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/CppParser/PreCompiledLexer.re
    ${CMAKE_CURRENT_SOURCE_DIR}/CppParser/MacroExpandMacroHelper.re
    ${CMAKE_CURRENT_SOURCE_DIR}/CppParser/SourceLexer.re
    DEPS
    ${CMAKE_CURRENT_SOURCE_DIR}/CppParser/def.re
)
set(CPP_PARSER_SOURCE
    # CppParser
    # CppParser/PreCompiledLexer.re.cc
    # CppParser/MacroExpandMacroHelper.re.cc
    ${CPP_PARSER_RE2C_SOURCES}
    CppParser/CompileSystem.cc
    CppParser/CompileUnit.cc
    CppParser/CommandParserImpl.cc
)

target_sources(xcdoc_lib PRIVATE
    ${UTILS_SOURCE}
    ${CPP_PARSER_SOURCE}
)

add_executable(xcdoc)

target_sources(xcdoc PRIVATE
    main.cc
)
target_link_libraries(xcdoc PRIVATE
    xcdoc_lib
)
