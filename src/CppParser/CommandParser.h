#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class CommandParser {
   private:
    std::string_view __command;

   public:
    static std::unique_ptr<CommandParser> parse(std::string_view command);

   protected:
    std::string_view ltrim(std::string_view s) const;
    std::vector<std::string_view> tokens() const;

   public:
    CommandParser(std::string_view command) : __command(command) {}
    virtual ~CommandParser() = default;
    std::string_view command() const { return __command; }
    void set_command(std::string_view command) { __command = command; }
    virtual std::vector<std::pair<std::string, std::string>> macros() const = 0;
    virtual std::vector<std::pair<std::string, std::string>> options()
        const = 0;
    virtual std::vector<std::string> include_paths() const = 0;
    virtual std::vector<std::string> input_files() const = 0;
    virtual std::vector<std::string> output_files() const = 0;
};
