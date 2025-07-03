#ifndef __SHOW_STATS_PANEL_COMMAND_H__
#define __SHOW_STATS_PANEL_COMMAND_H__

#include "../../common/CommandBase.h"
#include "../../common/PropertyTrigger.h"

class ShowStatsPanelCommand : public ICommandBase
{
public:
    ShowStatsPanelCommand(PropertyTrigger& trigger) noexcept;
    ~ShowStatsPanelCommand() noexcept;

    int exec(ICommandParameter* param) override;

private:
    PropertyTrigger& m_trigger;
};

#endif // __SHOW_STATS_PANEL_COMMAND_H__
