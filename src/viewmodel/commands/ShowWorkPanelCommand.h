#ifndef SHOW_WORK_PANEL_COMMAND_H
#define SHOW_WORK_PANEL_COMMAND_H

#include "../../common/CommandBase.h"
#include "../../common/PropertyTrigger.h"

class ShowWorkPanelCommand : public ICommandBase
{
public:
    ShowWorkPanelCommand(PropertyTrigger& trigger) noexcept;
    ~ShowWorkPanelCommand() noexcept;

    int exec(ICommandParameter* param) override;

private:
    PropertyTrigger& m_trigger;
};

#endif // SHOW_WORK_PANEL_COMMAND_H