#include "./CompileUnit.h"

#include <utils/public.h>

#include <nlohmann/json.hpp>

#include "./CommandParser.h"

using json = nlohmann::json;
CompileUnit::CompileUnit(const std::string& command) : __command(command) {
    auto parser = CommandParser::parse(__command);
    if (parser) {
        __includes_path = parser->include_paths();
        __ext_macros = parser->macros();
        __source_file = parser->input_files().front();
        __output_files = parser->output_files();
    }
}
