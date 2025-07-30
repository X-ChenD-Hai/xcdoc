#include "./command/root.h"
#include "core/command.h"

int app(int argc, char **argv) {
    CommandController controller(argc, argv);
    controller.setRootCommand<RootCommand>();

    return controller.exec();
}