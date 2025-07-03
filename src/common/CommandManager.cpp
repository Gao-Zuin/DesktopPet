#include "CommandManager.h"

CommandManager::CommandManager()
{
}

CommandManager::~CommandManager()
{
    clear();
}

void CommandManager::register_command(CommandType type, ICommandBase* command)
{
    if (command) {
        m_commands[type] = command;
    }
}

ICommandBase* CommandManager::get_command(CommandType type)
{
    auto it = m_commands.find(type);
    if (it != m_commands.end()) {
        return it->second;
    }
    return nullptr;
}

int CommandManager::execute_command(CommandType type, ICommandParameter* param)
{
    ICommandBase* command = get_command(type);
    if (command) {
        return command->exec(param);
    }
    return -1; // 命令不存在
}

void CommandManager::clear()
{
    m_commands.clear();
}
