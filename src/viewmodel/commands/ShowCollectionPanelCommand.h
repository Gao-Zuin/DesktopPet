#ifndef SHOW_COLLECTION_PANEL_COMMAND_H
#define SHOW_COLLECTION_PANEL_COMMAND_H

#include "../../common/CommandBase.h"

class PetViewModel;

class ShowCollectionPanelCommand : public ICommandBase
{
public:
    ShowCollectionPanelCommand(PetViewModel* pvm) : m_pvm(pvm) {}
    virtual ~ShowCollectionPanelCommand() = default;

    virtual int exec(ICommandParameter* p) override;

private:
    PetViewModel* m_pvm;
};

#endif // SHOW_COLLECTION_PANEL_COMMAND_H
