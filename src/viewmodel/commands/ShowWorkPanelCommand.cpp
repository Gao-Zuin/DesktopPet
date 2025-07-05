#include "ShowWorkPanelCommand.h"
#include "../../common/PropertyIds.h"
#include <QDebug>

ShowWorkPanelCommand::ShowWorkPanelCommand(PropertyTrigger& trigger) noexcept
    : m_trigger(trigger)
{
}

ShowWorkPanelCommand::~ShowWorkPanelCommand() noexcept
{
}

int ShowWorkPanelCommand::exec(ICommandParameter* param)
{
    Q_UNUSED(param)
    
    // 触发显示打工面板的通知
    m_trigger.fire(PROP_ID_SHOW_WORK_PANEL);
    
    return 0;
}