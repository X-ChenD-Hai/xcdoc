#pragma once
#include <memory>
#include <vector>

#include "CompileUnit.h"

class CompileSystem {
    using unit_list = std::vector<std::unique_ptr<CompileUnit>>;
    unit_list __units;

   public:
    CompileSystem(std::string_view compile_command_json_string);

    const unit_list& units() const { return __units; }
};