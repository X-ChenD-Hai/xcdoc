#ifndef COMMAND_H
#define COMMAND_H
#include <CLI/CLI.hpp>
template <typename T>
constexpr auto kCOMMAND_NAME = "";
template <typename T>
constexpr auto kCOMMAND_DESCRIPTION = "";
template <typename T>
constexpr const char* kCOMMAND_ALIAS[] = {};
class Command {
    friend class CommandController;

   private:
    CLI::App* app_;
    std::vector<std::unique_ptr<Command>> commands_;

   public:
    inline CLI::App* app() const { return app_; }
    Command(CLI::App* app) : app_(app) {}
    template <typename T, class... Args>
        requires std::derived_from<T, Command>
    T* add_subcommand(Args&&... args) {
        auto _a =
            app()->add_subcommand(kCOMMAND_NAME<T>, kCOMMAND_DESCRIPTION<T>);
        for (auto alias : kCOMMAND_ALIAS<T>) {
            _a->alias(alias);
        }
        auto cmd = new T(_a, std::forward<Args>(args)...);
        commands_.emplace_back(cmd);
        return cmd;
    }
    virtual int exec() = 0;

    virtual ~Command() {}

   private:
    int _exec();
};

class CommandController {
   private:
    int argc_;
    char** argv_;
    std::unique_ptr<Command> root_command_;
    std::unique_ptr<CLI::App> app_;

   public:
    int exec();
    CommandController(int argc, char** argv);
    template <typename T, class... Args>
        requires std::derived_from<T, Command>
    void setRootCommand(Args&&... args) {
        root_command_ =
            std::make_unique<T>(app_.get(), std::forward<Args>(args)...);
        app_->description(kCOMMAND_DESCRIPTION<T>);
    }
    ~CommandController() {}
};

#define XCDOC_DECLARE_COMMAND(Command, NAME, DESCRIPTION) \
    template <>                                           \
    constexpr inline auto kCOMMAND_NAME<Command> = NAME;  \
    template <>                                           \
    constexpr inline auto kCOMMAND_DESCRIPTION<Command> = DESCRIPTION;

#define XCDOC_DECLARE_ALIAS(Command, ALIAS...) \
    template <>                                \
    constexpr inline const char* kCOMMAND_ALIAS<Command>[] = {ALIAS};
#endif  // COMMAND_H