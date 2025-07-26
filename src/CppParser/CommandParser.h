#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class CommandParser {
   private:
    std::string_view __command;
    std::vector<std::string_view> __tokens;

   public:
    static std::unique_ptr<CommandParser> parse(std::string_view command);

   protected:
    static std::string_view ltrim(std::string_view s);
    static std::vector<std::string_view> __tokenize(std::string_view s);

   public:
    CommandParser(std::string_view command,
                  std::vector<std::string_view> tokens)
        : __command(command), __tokens(tokens) {}
    virtual ~CommandParser() = default;
    inline std::vector<std::string_view> tokens() const { return __tokens; }
    std::string_view command() const { return __command; }
    inline void set_command(std::string_view command) {
        __command = command;
        __tokens = __tokenize(__command);
    }
    virtual std::vector<std::pair<std::string, std::string>> macros() const = 0;
    virtual std::vector<std::pair<std::string, std::string>> options()
        const = 0;
    virtual std::vector<std::string> include_paths() const = 0;
    virtual std::vector<std::string> input_files() const = 0;
    virtual std::vector<std::string> output_files() const = 0;
};
