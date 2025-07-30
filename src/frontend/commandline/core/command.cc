#include "./command.h"
int Command::_exec() {
    for (auto& cmd : __commands)
        if (cmd->app()->parsed()) return cmd->_exec();
    int code = exec();
    if (code == -1) {
        std::cout << app()->help() << std::endl;
        return 0;
    }
    return code;
};
int CommandController::exec() {
    CLI11_PARSE(*__app, __argc_, __argv_);
    return __root_command->_exec();
}
CommandController::CommandController(int argc, char** argv)
    : __argc_(argc), __argv_(argv) {
    __app = std::make_unique<CLI::App>();
    argv = __app->ensure_utf8(argv);
}
