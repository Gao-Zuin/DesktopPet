#include "AddExperienceCommand.h"
#include "../PetViewModel.h"
#include "../../common/CommandParameters.h"

AddExperienceCommand::AddExperienceCommand(PetViewModel* viewModel)
    : m_view_model(viewModel)
{
}

AddExperienceCommand::~AddExperienceCommand()
{
}

int AddExperienceCommand::exec(ICommandParameter* param)
{
    if (!m_view_model || !param) return -1;
    
    AddExperienceCommandParameter* exp_param = static_cast<AddExperienceCommandParameter*>(param);
    m_view_model->add_experience(exp_param->experience);
    
    // 自动保存
    m_view_model->save_pet_data();
    
    return 0;
}
