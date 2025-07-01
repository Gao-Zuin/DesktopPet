#include "SwitchPetCommand.h"
#include "../PetViewModel.h"

int SwitchPetCommand::exec(ICommandParameter *p)
{
    SwitchPetCommandParameter *param = dynamic_cast<SwitchPetCommandParameter*>(p);
    if (!param) {
        return -1;
    }

    auto model = m_pvm->get_pet_model();
    if (model) {
        model->change_pet_type(param->petType);
        
        // 根据宠物类型切换动画
        QString animation;
        switch (param->petType) {
        case PetType::Spider:
            animation = ":/resources/gif/spider.gif";
            break;
        case PetType::Cassidy:
            animation = ":/resources/img/cassidy.png";
            break;
        default:
            animation = ":/resources/gif/spider.gif";
            break;
        }
        model->change_animation(animation);
    }

    return 0;
}
