#include "ShowBackpackPanelCommand.h"
#include "../../common/PropertyIds.h"
#include "../../common/CommandParameters.h"

ShowBackpackPanelCommand::ShowBackpackPanelCommand(PropertyTrigger& trigger) noexcept
    : m_trigger(trigger)
{
}

ShowBackpackPanelCommand::~ShowBackpackPanelCommand() noexcept
{
}

int ShowBackpackPanelCommand::exec(ICommandParameter* param)
{
    // 触发显示统计面板的属性通知
    m_trigger.fire(PROP_ID_SHOW_BACKPACK_PANEL);
    return 0;
}
