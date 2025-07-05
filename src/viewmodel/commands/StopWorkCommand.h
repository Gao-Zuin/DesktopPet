#ifndef STOP_WORK_COMMAND_H
#define STOP_WORK_COMMAND_H

#include "../../common/CommandBase.h"
#include "../../common/CommandParameters.h"

class PetViewModel;

class StopWorkCommand : public ICommandBase
{
public:
    StopWorkCommand(PetViewModel* pvm) noexcept;
    ~StopWorkCommand() noexcept;

    int exec(ICommandParameter* param) override;

private:
    PetViewModel* m_pvm;
};

#endif // STOP_WORK_COMMAND_H