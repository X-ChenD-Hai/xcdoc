#pragma once
#include <string>
#include <vector>

class CompileUnit {
    std::vector<std::string> __includes_path;
    std::vector<std::pair<std::string, std::string>> __ext_macros;
    std::string __source_file;
    std::vector<std::string> __output_files;
    std::string __command;

   public:
    CompileUnit(const std::string& command);
};