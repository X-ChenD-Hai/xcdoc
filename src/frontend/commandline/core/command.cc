#include "./command.h"
int Command::_exec() {
    for (auto& cmd : commands_)
        if (cmd->app()->parsed()) return cmd->_exec();
    int code = exec();
    if (code == -1) {
        std::cout << app()->help() << std::endl;
        return 0;
    }
    return code;
};
int CommandController::exec() {
    CLI11_PARSE(*app_, argc_, argv_);
    return root_command_->_exec();
}
CommandController::CommandController(int argc, char** argv)
    : argc_(argc), argv_(argv) {
    app_ = std::make_unique<CLI::App>();
    argv = app_->ensure_utf8(argv);
}
