#pragma once
#include <memory>
#include <vector>

#include "CompileUnit.h"

class CompileSystem {
    std::vector<std::unique_ptr<CompileUnit>> units;

   public:
    CompileSystem(std::string_view compile_command_json_string);
};