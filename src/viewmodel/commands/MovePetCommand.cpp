#include "MovePetCommand.h"
#include "../PetViewModel.h"

int MovePetCommand::exec(ICommandParameter *p)
{
    MoveCommandParameter *param = dynamic_cast<MoveCommandParameter*>(p);
    if (!param) {
        return -1;
    }

    auto model = m_pvm->get_pet_model();
    if (model) {
        model->change_position(param->position);
    }

    return 0;
}
