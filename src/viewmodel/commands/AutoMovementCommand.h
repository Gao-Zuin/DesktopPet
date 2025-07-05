#ifndef __AUTO_MOVEMENT_COMMAND_H__
#define __AUTO_MOVEMENT_COMMAND_H__

#include "../../common/CommandBase.h"
#include "../../common/CommandParameters.h"

class PetViewModel;

/**
 * 自动移动命令类
 */
class AutoMovementCommand : public ICommandBase
{
public:
    AutoMovementCommand(PetViewModel *pvm) noexcept : m_pvm(pvm) {}
    AutoMovementCommand(const AutoMovementCommand&) = delete;
    ~AutoMovementCommand() noexcept {}

    AutoMovementCommand& operator=(const AutoMovementCommand&) = delete;

    // 重写执行方法
    virtual int exec(ICommandParameter *p) override;

private:
    PetViewModel *m_pvm;
};

#endif // __AUTO_MOVEMENT_COMMAND_H__
