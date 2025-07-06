#ifndef __FORGE_COMMAND_H__
#define __FORGE_COMMAND_H__

#include "../../common/CommandBase.h"
#include "../../common/CommandParameters.h"

class PetViewModel;

class ForgeCommand : public ICommandBase
{
public:
    ForgeCommand(PetViewModel* viewModel);
    ~ForgeCommand();

    int exec(ICommandParameter* param) override;

private:
    PetViewModel* m_view_model;
};

#endif // __FORGE_COMMAND_H__
