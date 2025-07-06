#include "ShowForgePanelCommand.h"
#include "../../common/PropertyIds.h"
#include "../../common/CommandParameters.h"
#include <QDebug>

ShowForgePanelCommand::ShowForgePanelCommand(PropertyTrigger& trigger) noexcept
    : m_trigger(trigger)
{
}

ShowForgePanelCommand::~ShowForgePanelCommand() noexcept
{
}

int ShowForgePanelCommand::exec(ICommandParameter* param)
{
    qDebug() << "ShowForgePanelCommand::exec called";
    
    // 触发显示锻造面板的属性通知
    m_trigger.fire(PROP_ID_SHOW_FORGE_PANEL);
    
    qDebug() << "ShowForgePanelCommand: Property trigger fired with ID" << PROP_ID_SHOW_FORGE_PANEL;
    
    return 0;
}
