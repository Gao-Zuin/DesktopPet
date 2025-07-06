#ifndef WORK_INFO_H
#define WORK_INFO_H

#include <QString>
#include "../../common/Types.h"

// 打工信息结构
struct WorkInfo {
    WorkType type;
    QString name;
    QString description;
    QString petForm;        // 对应的桌宠形态
    QString petFormImage;   // 桌宠形态图片路径
    int experienceReward;   // 经验值奖励
    int workDuration;       // 工作持续时间(秒)
    
    WorkInfo(WorkType t = WorkType::Photosynthesis, 
             const QString& n = "", 
             const QString& desc = "",
             const QString& form = "",
             const QString& formImg = "",
             int exp = 0, 
             int duration = 10)
        : type(t), name(n), description(desc), petForm(form), 
          petFormImage(formImg), experienceReward(exp), workDuration(duration) {}
};

#endif // WORK_INFO_H