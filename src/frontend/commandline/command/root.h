#include "../core/command.h"

class RootCommand : public Command {
   private:
    struct {
        bool version{false};
    } flags;

   public:
    RootCommand(CLI::App* _app) : Command(_app) {
        app()->add_flag("-v,--version", flags.version,
                        "Show version information");
    }
    int exec() override {
        // Root command execution logic can be added here if needed
        if (flags.version) {
            std::cout << "XCDOC version " XCDOC_VERSION << std::endl;
            return 0;
        }
        return -1;
    }
};
DECLARE_COMMAND(RootCommand, "envman", "Manage environment variables");
