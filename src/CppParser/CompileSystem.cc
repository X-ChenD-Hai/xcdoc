#include "./CompileSystem.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;
CompileSystem::CompileSystem(std::string_view compile_command_json_string) {
    json data = json::parse(compile_command_json_string);
    if (data.is_array()) {
        for (auto o : data) {
            if (o["command"].is_string())
                units_.emplace_back(new CompileUnit(o["command"]));
        }

    } else {
    }
}
