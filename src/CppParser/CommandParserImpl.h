#pragma once
#include "CppParser/CommandParser.h"
class ClangClCommandParser : public CommandParser {
   public:
    using CommandParser::CommandParser;
    std::vector<std::pair<std::string, std::string>> macros() const override;
    std::vector<std::pair<std::string, std::string>> options() const override;
    std::vector<std::string> include_paths() const override;
    std::vector<std::string> input_files() const override;
    std::vector<std::string> output_files() const override;

   private:
    std::vector<std::pair<std::string, std::string>> extract_prefixed(
        std::string_view flag) const;
};

class GccCommandParser : public CommandParser {
   public:
    using CommandParser::CommandParser;
    std::vector<std::pair<std::string, std::string>> macros() const override;
    std::vector<std::pair<std::string, std::string>> options() const override;
    std::vector<std::string> include_paths() const override;
    std::vector<std::string> input_files() const override;
    std::vector<std::string> output_files() const override;

   private:
    std::vector<std::pair<std::string, std::string>> extract_prefixed(
        std::string_view flag) const;
};

using ClangCommandParser = GccCommandParser;
class MsvcCommandParser : public CommandParser {
   public:
    using CommandParser::CommandParser;
    std::vector<std::pair<std::string, std::string>> macros() const override;
    std::vector<std::pair<std::string, std::string>> options() const override;
    std::vector<std::string> include_paths() const override;
    std::vector<std::string> input_files() const override;
    std::vector<std::string> output_files() const override;

   private:
    std::vector<std::pair<std::string, std::string>> extract_prefixed(
        std::string_view flag) const;
};