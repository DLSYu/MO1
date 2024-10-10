#include "ICommand.h"

ICommand::ICommand(int pid, ICommand::CommandType commandType)
    : pid(pid), commandType(commandType) {}

ICommand::CommandType ICommand::getCommandType() {
    return commandType;
}

void ICommand::execute() {

}