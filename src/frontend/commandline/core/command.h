#ifndef COMMAND_H
#define COMMAND_H
#include <CLI/CLI.hpp>
template <typename T>
constexpr auto __COMMAND_NAME = "";
template <typename T>
constexpr auto __COMMAND_DESCRIPTION = "";
template <typename T>
constexpr const char* __COMMAND_ALIAS[] = {};
class Command {
    friend class CommandController;

   private:
    CLI::App* __app;
    std::vector<std::unique_ptr<Command>> __commands;

   public:
    inline CLI::App* app() const { return __app; }
    Command(CLI::App* app) : __app(app) {}
    template <typename T, class... Args>
        requires std::derived_from<T, Command>
    T* add_subcommand(Args&&... args) {
        auto _a =
            app()->add_subcommand(__COMMAND_NAME<T>, __COMMAND_DESCRIPTION<T>);
        for (auto alias : __COMMAND_ALIAS<T>) {
            _a->alias(alias);
        }
        auto cmd = new T(_a, std::forward<Args>(args)...);
        __commands.emplace_back(cmd);
        return cmd;
    }
    virtual int exec() = 0;

    virtual ~Command() {}

   private:
    int _exec();
};

class CommandController {
   private:
    int __argc_;
    char** __argv_;
    std::unique_ptr<Command> __root_command;
    std::unique_ptr<CLI::App> __app;

   public:
    int exec();
    CommandController(int argc, char** argv);
    template <typename T, class... Args>
        requires std::derived_from<T, Command>
    void setRootCommand(Args&&... args) {
        __root_command =
            std::make_unique<T>(__app.get(), std::forward<Args>(args)...);
        __app->description(__COMMAND_DESCRIPTION<T>);
    }
    ~CommandController() {}
};

#define DECLARE_COMMAND(Command, NAME, DESCRIPTION)       \
    template <>                                           \
    constexpr inline auto __COMMAND_NAME<Command> = NAME; \
    template <>                                           \
    constexpr inline auto __COMMAND_DESCRIPTION<Command> = DESCRIPTION;

#define DECLARE_ALIAS(Command, ALIAS...) \
    template <>                          \
    constexpr inline const char* __COMMAND_ALIAS<Command>[] = {ALIAS};
#endif  // COMMAND_H