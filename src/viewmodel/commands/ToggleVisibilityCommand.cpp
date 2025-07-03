#include "ToggleVisibilityCommand.h"
#include "../PetViewModel.h"

int ToggleVisibilityCommand::exec(ICommandParameter *p)
{
    (void)p; // 消除未使用参数警告
    auto model = m_pvm->get_pet_model();
    if (model) {
        bool currentVisibility = model->get_info()->isVisible;
        model->change_visibility(!currentVisibility);
    }

    return 0;
}
