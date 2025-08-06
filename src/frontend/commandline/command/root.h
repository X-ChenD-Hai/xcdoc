#include "../core/command.h"
#include "./class.h"
class RootCommand : public Command {
   private:
    struct {
        bool version{false};
    } flags_;

   public:
    RootCommand(CLI::App* _app) : Command(_app) {
        app()->add_flag("-v,--version", flags_.version,
                        "Show version information");
        add_subcommand<ClassCommand>();
    }
    int exec() override {
        // Root command execution logic can be added here if needed
        if (flags_.version) {
            std::cout << "XCDOC version " XCDOC_VERSION << std::endl;
            return 0;
        }
        return -1;
    }
};
XCDOC_DECLARE_COMMAND(RootCommand, "xcdox", "XCDoc command line Interface");
