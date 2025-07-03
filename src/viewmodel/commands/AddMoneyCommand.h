#ifndef __ADD_MONEY_COMMAND_H__
#define __ADD_MONEY_COMMAND_H__

#include "../../common/CommandBase.h"

class PetViewModel;

class AddMoneyCommand : public ICommandBase
{
public:
    AddMoneyCommand(PetViewModel* viewModel);
    ~AddMoneyCommand();

    int exec(ICommandParameter* param) override;

private:
    PetViewModel* m_view_model;
};

#endif // __ADD_MONEY_COMMAND_H__
