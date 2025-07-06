#include "ShowWorkUpgradePanelCommand.h"
#include "../../app/PetApp.h"

int ShowWorkUpgradePanelCommand::exec(ICommandParameter *param)
{
    Q_UNUSED(param)
    
    PetApp* app = PetApp::getInstance();
    if (app) {
        app->showWorkUpgradePanel();
        return 0;
    }
    return -1;
}