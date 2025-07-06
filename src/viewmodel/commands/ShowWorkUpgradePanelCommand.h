#ifndef __SHOW_WORK_UPGRADE_PANEL_COMMAND_H__
#define __SHOW_WORK_UPGRADE_PANEL_COMMAND_H__

#include "../../common/CommandBase.h"

class ShowWorkUpgradePanelCommand : public ICommandBase
{
public:
    ShowWorkUpgradePanelCommand() = default;
    ~ShowWorkUpgradePanelCommand() = default;

    int exec(ICommandParameter *param) override;
};

#endif // __SHOW_WORK_UPGRADE_PANEL_COMMAND_H__