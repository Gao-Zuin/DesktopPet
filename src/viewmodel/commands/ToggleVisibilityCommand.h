#ifndef __TOGGLE_VISIBILITY_COMMAND_H__
#define __TOGGLE_VISIBILITY_COMMAND_H__

#include "../../common/CommandBase.h"

class PetViewModel;

class ToggleVisibilityCommand : public ICommandBase
{
public:
    ToggleVisibilityCommand(PetViewModel *p) noexcept : m_pvm(p)
    {
    }
    ToggleVisibilityCommand(const ToggleVisibilityCommand&) = delete;
    ~ToggleVisibilityCommand() noexcept
    {
    }

    ToggleVisibilityCommand& operator=(const ToggleVisibilityCommand&) = delete;

    // Overriders
    virtual int exec(ICommandParameter *p) override;

private:
    PetViewModel *m_pvm;
};

#endif
