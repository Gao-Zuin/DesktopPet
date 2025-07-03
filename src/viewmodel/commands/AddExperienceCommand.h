#ifndef __ADD_EXPERIENCE_COMMAND_H__
#define __ADD_EXPERIENCE_COMMAND_H__

#include "../../common/CommandBase.h"

class PetViewModel;

class AddExperienceCommand : public ICommandBase
{
public:
    AddExperienceCommand(PetViewModel* viewModel);
    ~AddExperienceCommand();

    int exec(ICommandParameter* param) override;

private:
    PetViewModel* m_view_model;
};

#endif // __ADD_EXPERIENCE_COMMAND_H__
