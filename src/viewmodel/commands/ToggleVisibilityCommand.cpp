#include "ToggleVisibilityCommand.h"
#include "../PetViewModel.h"

int ToggleVisibilityCommand::exec(ICommandParameter *p)
{
    auto model = m_pvm->get_pet_model();
    if (model) {
        bool currentVisibility = model->get_info()->isVisible;
        model->change_visibility(!currentVisibility);
    }

    return 0;
}
