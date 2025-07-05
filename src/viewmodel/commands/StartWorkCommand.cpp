#include "StartWorkCommand.h"
#include "../PetViewModel.h"
#include "../../common/Types.h"
#include <QDebug>

StartWorkCommand::StartWorkCommand(PetViewModel* pvm) noexcept
    : m_pvm(pvm)
{
}

StartWorkCommand::~StartWorkCommand() noexcept
{
}

int StartWorkCommand::exec(ICommandParameter* param)
{
    StartWorkCommandParameter* workParam = dynamic_cast<StartWorkCommandParameter*>(param);
    if (!workParam) {
        qDebug() << "StartWorkCommand: 无效的参数";
        return -1;
    }

    // 获取工作模型并开始工作
    auto workModel = m_pvm->get_work_model();
    if (workModel) {
        WorkType workType = static_cast<WorkType>(workParam->workType);
        workModel->startWork(workType);

        // 获取对应的桌宠形态并切换
        const WorkInfo* workInfo = workModel->getWorkInfo(workType);
        if (workInfo) {
            auto petModel = m_pvm->get_pet_model();
            if (petModel) {
                // 切换桌宠到工作形态
                petModel->change_animation(workInfo->petFormImage);
                qDebug() << "StartWorkCommand: 切换桌宠形态到" << workInfo->petFormImage;
            }
        }

        qDebug() << "StartWorkCommand: 开始工作类型" << workParam->workType;
    } else {
        qDebug() << "StartWorkCommand: 工作模型未找到";
        return -1;
    }

    return 0;
}