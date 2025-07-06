#ifndef __SHOW_FORGE_PANEL_COMMAND_H__
#define __SHOW_FORGE_PANEL_COMMAND_H__

#include "../../common/CommandBase.h"
#include "../../common/PropertyTrigger.h"

class ShowForgePanelCommand : public ICommandBase
{
public:
    ShowForgePanelCommand(PropertyTrigger& trigger) noexcept;
    ~ShowForgePanelCommand() noexcept;

    ShowForgePanelCommand(const ShowForgePanelCommand&) = delete;
    ShowForgePanelCommand& operator=(const ShowForgePanelCommand&) = delete;

    // overriders
    virtual int exec(ICommandParameter* param) override;

private:
    PropertyTrigger& m_trigger;
};

#endif // __SHOW_FORGE_PANEL_COMMAND_H__
