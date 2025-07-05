#ifndef __COMMAND_MANAGER_H__
#define __COMMAND_MANAGER_H__

#include "CommandBase.h"
#include <unordered_map>
#include <string>

enum class CommandType {
    MOVE_PET,
    SWITCH_PET,
    SHOW_STATS_PANEL,
    SHOW_BACKPACK_PANEL,
    SHOW_COLLECTION_PANEL,
    ADD_EXPERIENCE,
    ADD_MONEY,
    USE_ITEM
};

class CommandManager
{
public:
    CommandManager();
    ~CommandManager();

    // 注册命令
    void register_command(CommandType type, ICommandBase* command);
    
    // 获取命令
    ICommandBase* get_command(CommandType type);
    
    // 执行命令
    int execute_command(CommandType type, ICommandParameter* param);
    
    // 清理所有命令
    void clear();

private:
    std::unordered_map<CommandType, ICommandBase*> m_commands;
};

#endif // __COMMAND_MANAGER_H__
