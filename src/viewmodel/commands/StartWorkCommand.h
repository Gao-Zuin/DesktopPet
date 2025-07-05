#ifndef START_WORK_COMMAND_H
#define START_WORK_COMMAND_H

#include "../../common/CommandBase.h"
#include "../../common/CommandParameters.h"

class PetViewModel;

class StartWorkCommand : public ICommandBase
{
public:
    StartWorkCommand(PetViewModel* pvm) noexcept;
    ~StartWorkCommand() noexcept;

    int exec(ICommandParameter* param) override;

private:
    PetViewModel* m_pvm;
};

#endif // START_WORK_COMMAND_H