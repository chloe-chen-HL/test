#include "factory.h"

CommandPtr CommandFactory::lastCommand(Command::Type type)
{
    return lastCommand_;
}

void CommandFactory::setLastCommand(CommandPtr command)
{
    lastCommand_ = command;
}