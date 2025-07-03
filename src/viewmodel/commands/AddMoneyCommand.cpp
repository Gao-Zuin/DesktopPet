#include "AddMoneyCommand.h"
#include "../PetViewModel.h"
#include "../../common/CommandParameters.h"

AddMoneyCommand::AddMoneyCommand(PetViewModel* viewModel)
    : m_view_model(viewModel)
{
}

AddMoneyCommand::~AddMoneyCommand()
{
}

int AddMoneyCommand::exec(ICommandParameter* param)
{
    if (!m_view_model || !param) return -1;
    
    AddMoneyCommandParameter* money_param = static_cast<AddMoneyCommandParameter*>(param);
    m_view_model->add_money(money_param->money);
    
    // 自动保存
    m_view_model->save_pet_data();
    
    return 0;
}
