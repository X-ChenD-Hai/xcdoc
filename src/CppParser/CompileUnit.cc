#include "./CompileUnit.h"

#include <utils/public.h>

#include <filesystem>
#include <nlohmann/json.hpp>

#include "./CommandParser.h"
#include "PreCompiledLexer.h"

using json = nlohmann::json;
CompileUnit::CompileUnit(const std::string& command) : __command(command) {
    auto parser = CommandParser::parse(__command);
    OUT SV(command, __command) ENDL;
    namespace fs = std::filesystem;
    if (parser) {
        __source_file = parser->input_files().front();
        __includes_path.push_back(
            fs::path(__source_file).parent_path().string());
        __includes_path.append_range(parser->include_paths());
        __ext_macros = parser->macros();
        __output_files = parser->output_files();
        __source = utils::read_file(__source_file);
        __pre_lexer = std::make_unique<PreCompiledLexer>(&__source, this);
        for (auto i : __includes_path) {
            OUT SV(include_path, i) ENDL;
        }
        OUT NV(__source_file) ENDL;
        __pre_lexer->source();
    }
}
PreCompiledLexer* CompileUnit::attach_lexer(const std::string& file_name,
                                            bool is_absolute_path) {
    namespace fs = std::filesystem;
    auto it = __attached_lexers.find(file_name);

    OUT SV(search_file, file_name) ENDL;
    if (it == __attached_lexers.end()) {
        if (is_absolute_path) {
            OUT VV("\tabsolute path") ENDL;
            if (auto l = _attach_lexer(fs::path(file_name), file_name); l) {
                OUT VV("\tfound") ENDL;
                l->source();
                return l;
            }
            return nullptr;
        }
        for (auto& ps : __includes_path) {
            OUT SV(\tsearch_on, fs::path(ps) / file_name) ENDL;
            if (auto l = _attach_lexer(fs::path(ps) / file_name, file_name);
                l) {
                OUT VV("\tfound") ENDL;
                l->source();
                return l;
            }
        }
        OUT VV("\tnot found") ENDL;
    } else {
        OUT VV("\talready attached") ENDL;
        return it->second.get();
    }
    return nullptr;
}
PreCompiledLexer* CompileUnit::_attach_lexer(const std::filesystem::path& ps,
                                             const std::string& key) {
    namespace fs = std::filesystem;
    if (fs::is_regular_file(ps)) {
        auto content = new std::string(utils::read_file((ps.string())));
        __include_files.emplace_back(content);
        auto lexer = std::make_unique<PreCompiledLexer>(content, this);
        return __attached_lexers.emplace(key, std::move(lexer))
            .first->second.get();
    }
    return nullptr;
};