#ifndef __SWITCH_PET_COMMAND_H__
#define __SWITCH_PET_COMMAND_H__

#include "../../common/CommandBase.h"
#include "../../common/CommandParameters.h"

class PetViewModel;

class SwitchPetCommand : public ICommandBase
{
public:
    SwitchPetCommand(PetViewModel *p) noexcept : m_pvm(p)
    {
    }
    SwitchPetCommand(const SwitchPetCommand&) = delete;
    ~SwitchPetCommand() noexcept
    {
    }

    SwitchPetCommand& operator=(const SwitchPetCommand&) = delete;

    // overriders
    virtual int exec(ICommandParameter *p) override;

private:
    PetViewModel *m_pvm;
};

#endif
