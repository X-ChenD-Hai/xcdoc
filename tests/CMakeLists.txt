find_package(GTest CONFIG REQUIRED)
set(TEST_LIBS GTest::gtest GTest::gtest_main)
set(TESTS_RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)
add_compile_definitions(XCDOC_TESTS_RESOURCES_DIR="${TESTS_RESOURCE_DIR}/")
link_libraries(xcdoc_lib ${TEST_LIBS})

add_executable(test_utils
    utils/string_slice_view.cc
)
add_executable(test_CppParser
    CppParser/CppParser.cc
    CppParser/CommandParser.cc
    CppParser/PreCompileLexer.cc
)
add_test(NAME utils.all COMMAND test_utils)
add_test(NAME utils.string_slice_view COMMAND test_utils --gtest_filter=string_slice_view.*)
add_test(NAME CppParser.all COMMAND test_CppParser)
add_test(NAME CppParser.CppParser COMMAND test_CppParser --gtest_filter=CppParser.*)
add_test(NAME CppParser.CommandParser COMMAND test_CppParser --gtest_filter=CommandParser.*)
add_test(NAME CppParser.PreCompileLexer COMMAND test_CppParser --gtest_filter=PreCompileLexer.*)
