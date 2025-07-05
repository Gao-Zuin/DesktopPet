#ifndef __SHOW_BACKPACK_PANEL_COMMAND_H__
#define __SHOW_BACKPACK_PANEL_COMMAND_H__

#include "../../common/CommandBase.h"
#include "../../common/PropertyTrigger.h"

class ShowBackpackPanelCommand : public ICommandBase
{
public:
    ShowBackpackPanelCommand(PropertyTrigger& trigger) noexcept;
    ~ShowBackpackPanelCommand() noexcept;

    int exec(ICommandParameter* param) override;

private:
    PropertyTrigger& m_trigger;
};

#endif // __SHOW_STATS_PANEL_COMMAND_H__
