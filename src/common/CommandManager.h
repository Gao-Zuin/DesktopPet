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
    USE_ITEM,
    SHOW_WORK_PANEL,
    START_WORK,
    STOP_WORK,
    AUTO_MOVEMENT,       // 自动移动命令
    FORGE,               // 锻造命令
    SHOW_FORGE_PANEL,    // 显示锻造面板命令
    SHOW_WORK_UPGRADE_PANEL  // 显示工作升级面板命令
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
