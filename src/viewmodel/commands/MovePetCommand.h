#ifndef __MOVE_PET_COMMAND_H__
#define __MOVE_PET_COMMAND_H__

#include "../../common/CommandBase.h"
#include "../../common/CommandParameters.h"

class PetViewModel;

class MovePetCommand : public ICommandBase
{
public:
    MovePetCommand(PetViewModel *p) noexcept : m_pvm(p)
    {
    }
    MovePetCommand(const MovePetCommand&) = delete;
    ~MovePetCommand() noexcept
    {
    }

    MovePetCommand& operator=(const MovePetCommand&) = delete;

    // Overriders
    virtual int exec(ICommandParameter *p) override;

private:
    PetViewModel *m_pvm;
};

#endif
