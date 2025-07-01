#include "ChangeStateCommand.h"
#include "../PetViewModel.h"

int ChangeStateCommand::exec(ICommandParameter *p)
{
    StateCommandParameter *param = dynamic_cast<StateCommandParameter*>(p);
    if (!param) {
        return -1;
    }

    auto model = m_pvm->get_pet_model();
    if (model) {
        model->change_state(param->state);
    }

    return 0;
}
