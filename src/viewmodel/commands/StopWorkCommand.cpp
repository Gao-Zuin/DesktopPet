#include "StopWorkCommand.h"
#include "../PetViewModel.h"
#include <QDebug>

StopWorkCommand::StopWorkCommand(PetViewModel* pvm) noexcept
    : m_pvm(pvm)
{
}

StopWorkCommand::~StopWorkCommand() noexcept
{
}

int StopWorkCommand::exec(ICommandParameter* param)
{
    Q_UNUSED(param)
    
    // 获取工作模型并停止工作
    auto workModel = m_pvm->get_work_model();
    if (workModel) {
        workModel->stopWork();
        
        // 恢复桌宠到默认形态
        auto petModel = m_pvm->get_pet_model();
        if (petModel) {
            // 根据当前宠物类型恢复到对应的默认形态
            // 这里可以根据需要设置默认动画
            petModel->change_animation(":/resources/gif/spider.gif"); // 默认蜘蛛形态
            qDebug() << "StopWorkCommand: 恢复桌宠到默认形态";
        }
        
        qDebug() << "StopWorkCommand: 停止工作";
    } else {
        qDebug() << "StopWorkCommand: 工作模型未找到";
        return -1;
    }

    return 0;
}