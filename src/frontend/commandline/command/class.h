#include "../core/command.h"

class ClassCommand : public Command {
   private:
    struct {
    } flags_;

   public:
    ClassCommand(CLI::App* _app) : Command(_app) {}
    int exec() override { return 0; }
};
XCDOC_DECLARE_COMMAND(ClassCommand, "class", "project class commands");
