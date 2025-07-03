#include "ShowStatsPanelCommand.h"
#include "../../common/PropertyIds.h"
#include "../../common/CommandParameters.h"

ShowStatsPanelCommand::ShowStatsPanelCommand(PropertyTrigger& trigger) noexcept
    : m_trigger(trigger)
{
}

ShowStatsPanelCommand::~ShowStatsPanelCommand() noexcept
{
}

int ShowStatsPanelCommand::exec(ICommandParameter* param)
{
    // 触发显示统计面板的属性通知
    m_trigger.fire(PROP_ID_SHOW_STATS_PANEL);
    return 0;
}
