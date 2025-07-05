#include "WorkModel.h"
#include "../common/EventMgr.h"
#include "../common/EventDefine.h"
#include <QDebug>

WorkModel::WorkModel() noexcept
    : QObject(nullptr)
    , m_currentStatus(WorkStatus::Idle)
    , m_currentWorkType(WorkType::Photosynthesis)
    , m_remainingTime(0)
    , m_continuousMode(false)
    , m_workTimer(nullptr)
{
    initializeWorkTypes();
    
    // 初始化工作定时器
    m_workTimer = new QTimer(this);
    m_workTimer->setInterval(1000); // 每秒更新一次
    connect(m_workTimer, &QTimer::timeout, this, &WorkModel::onWorkTimer);
}

WorkModel::~WorkModel() noexcept
{
    if (m_workTimer) {
        m_workTimer->stop();
    }
}

void WorkModel::initializeWorkTypes()
{
    // 初始化光合作用打工类型 - 使用英文文件名的向日葵gif资源
    WorkInfo photosynthesis(
        WorkType::Photosynthesis,
        "光合作用",
        "通过光合作用获得经验值，桌宠会变成向日葵耄耋形态",
        "向日葵耄耋",
        ":/resources/gif/sunflower.gif",  // 使用英文文件名
        50,  // 经验值奖励
        30   // 工作持续时间30秒
    );
    
    m_workTypes.append(photosynthesis);
}

void WorkModel::startWork(WorkType type) noexcept
{
    const WorkInfo* workInfo = getWorkInfo(type);
    if (!workInfo) {
        qDebug() << "未找到指定的工作类型";
        return;
    }
    
    // 如果已经在工作且是同一类型，则切换为连续模式
    if (m_currentStatus == WorkStatus::Working && m_currentWorkType == type) {
        m_continuousMode = true;
        qDebug() << "切换到连续打工模式：" << workInfo->name;
        fireWorkStatusUpdate();
        return;
    }
    
    // 开始新的工作
    m_currentStatus = WorkStatus::Working;
    m_currentWorkType = type;
    m_remainingTime = workInfo->workDuration;
    m_continuousMode = true; // 默认开启连续模式
    
    qDebug() << "开始连续打工：" << workInfo->name << "，持续时间：" << workInfo->workDuration << "秒";
    
    m_workTimer->start();
    fireWorkStatusUpdate();
}

void WorkModel::stopWork() noexcept
{
    if (m_currentStatus == WorkStatus::Idle) {
        qDebug() << "当前没有在工作";
        return;
    }
    
    m_currentStatus = WorkStatus::Idle;
    m_remainingTime = 0;
    m_continuousMode = false; // 停止连续模式
    m_workTimer->stop();
    
    qDebug() << "停止打工和连续模式";
    fireWorkStatusUpdate();
}

const WorkInfo* WorkModel::getWorkInfo(WorkType type) const noexcept
{
    for (const WorkInfo& info : m_workTypes) {
        if (info.type == type) {
            return &info;
        }
    }
    return nullptr;
}

void WorkModel::onWorkTimer()
{
    if (m_currentStatus != WorkStatus::Working) {
        m_workTimer->stop();
        return;
    }
    
    m_remainingTime--;
    
    if (m_remainingTime <= 0) {
        // 工作完成
        const WorkInfo* workInfo = getWorkInfo(m_currentWorkType);
        if (workInfo) {
            qDebug() << "工作完成！获得" << workInfo->experienceReward << "经验值";
            
            // 触发经验值增加事件
            AddExperienceEvent event;
            event.experience = workInfo->experienceReward;
            EventMgr::GetInstance().SendEvent(event);
        }
        
        if (m_continuousMode) {
            // 如果是连续模式，自动开始下一轮工作
            qDebug() << "自动开始下一轮工作：" << workInfo->name;
            m_remainingTime = workInfo->workDuration; // 重置剩余时间
            fireWorkStatusUpdate();
        } else {
            // 停止工作
            m_currentStatus = WorkStatus::Idle;
            m_workTimer->stop();
            fireWorkStatusUpdate();
        }
    } else {
        // 更新剩余时间
        fireWorkStatusUpdate();
    }
}

void WorkModel::fireWorkStatusUpdate()
{
    m_trigger.fire(PROP_ID_WORK_STATUS_UPDATE);
}