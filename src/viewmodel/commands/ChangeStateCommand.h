#ifndef __CHANGE_STATE_COMMAND_H__
#define __CHANGE_STATE_COMMAND_H__

#include "../../common/CommandBase.h"
#include "../../model/base/PetInfo.h"

class PetViewModel;

class StateCommandParameter : public ICommandParameter
{
public:
    StateCommandParameter(PetState s) : state(s) {}
    PetState state;
};

class ChangeStateCommand : public ICommandBase
{
public:
    ChangeStateCommand(PetViewModel *p) noexcept : m_pvm(p)
    {
    }
    ChangeStateCommand(const ChangeStateCommand&) = delete;
    ~ChangeStateCommand() noexcept
    {
    }

    ChangeStateCommand& operator=(const ChangeStateCommand&) = delete;

    // Overriders
    virtual int exec(ICommandParameter *p) override;

private:
    PetViewModel *m_pvm;
};

#endif
