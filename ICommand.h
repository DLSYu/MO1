#pragma once
class ICommand
{
public:
    enum CommandType {
        PRINT
    };

    ICommand(int pid, CommandType commandType);
    CommandType getCommandType();
    virtual void execute();

protected:
    int pid;
    CommandType commandType; 
};
