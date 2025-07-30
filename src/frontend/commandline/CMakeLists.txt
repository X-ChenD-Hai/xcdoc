find_package(CLI11 CONFIG REQUIRED)

add_library(xcdoc_frontend_commandline INTERFACE)
target_link_libraries(xcdoc_frontend_commandline INTERFACE CLI11::CLI11)

target_sources(xcdoc_frontend_commandline INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/app.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/core/command.cc
)
