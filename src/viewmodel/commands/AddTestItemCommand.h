#ifndef ADD_TEST_ITEM_COMMAND_H
#define ADD_TEST_ITEM_COMMAND_H

#include "../../common/CommandBase.h"

class PetViewModel;

class AddTestItemCommand : public ICommandBase
{
public:
    AddTestItemCommand(PetViewModel* pvm) : m_pvm(pvm) {}
    virtual ~AddTestItemCommand() = default;

    virtual int exec(ICommandParameter* p) override;

private:
    PetViewModel* m_pvm;
};

#endif // ADD_TEST_ITEM_COMMAND_H
