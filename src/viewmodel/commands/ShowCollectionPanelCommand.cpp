#include "ShowCollectionPanelCommand.h"
#include "../PetViewModel.h"
#include "../../common/PropertyIds.h"

int ShowCollectionPanelCommand::exec(ICommandParameter* p)
{
    Q_UNUSED(p)
    
    // 触发显示图鉴面板的通知
    m_pvm->get_trigger().fire(PROP_ID_SHOW_COLLECTION_PANEL);
    
    return 0;
}
