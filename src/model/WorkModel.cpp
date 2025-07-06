#include "WorkModel.h"
#include "../common/EventMgr.h"
#include "../common/EventDefine.h"
#include <QDebug>
#include <QRandomGenerator>

WorkModel::WorkModel() noexcept
    : QObject(nullptr), m_currentStatus(WorkStatus::Idle), m_currentWorkType(WorkType::Photosynthesis), m_remainingTime(0), m_continuousMode(false), m_workTimer(nullptr)
{
    initializeWorkTypes();

    // 初始化工作定时器
    m_workTimer = new QTimer(this);
    m_workTimer->setInterval(1000); // 每秒更新一次
    connect(m_workTimer, &QTimer::timeout, this, &WorkModel::onWorkTimer);
}

WorkModel::~WorkModel() noexcept
{
    if (m_workTimer)
    {
        m_workTimer->stop();
    }
}

void WorkModel::initializeWorkTypes()
{
    // 初始化光合作用打工类型 - 使用英文文件名的向日葵gif资源
    WorkInfo photosynthesis(
        WorkType::Photosynthesis,
        "光合作用",
        "通过光合作用获得经验值，桌宠会变成向日葵耄耋形态。",
        "向日葵耄耋",
        ":/resources/gif/sunflower.gif", // 使用英文文件名
        50,                              // 经验值奖励
        15                               // 工作持续时间15秒
    );

    // 初始化挖矿打工类型
    WorkInfo mining(
        WorkType::Mining,
        "挖矿",
        "在地下深处挖掘珍贵矿物，桌宠会变成挖矿形态。",
        "挖矿形态",
        ":/resources/gif/mining.gif", // 挖矿动画
        75,                           // 经验值奖励
        20                            // 工作持续时间20秒
    );

    // 初始化冒险打工类型
    WorkInfo adventure(
        WorkType::Adventure,
        "冒险",
        "在森林中冒险探索，收集各种珍贵木材，桌宠会变成冒险形态。",
        "冒险形态",
        ":/resources/gif/adventure.gif", // 冒险动画
        60,                              // 经验值奖励
        18                               // 工作持续时间18秒
    );

    m_workTypes.append(photosynthesis);
    m_workTypes.append(mining);
    m_workTypes.append(adventure);
}

void WorkModel::startWork(WorkType type) noexcept
{
    const WorkInfo *workInfo = getWorkInfo(type);
    if (!workInfo)
    {
        qDebug() << "未找到指定的工作类型";
        return;
    }

    // 如果已经在工作且是同一类型，则切换为连续模式
    if (m_currentStatus == WorkStatus::Working && m_currentWorkType == type)
    {
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
    if (m_currentStatus == WorkStatus::Idle)
    {
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

const WorkInfo *WorkModel::getWorkInfo(WorkType type) const noexcept
{
    for (const WorkInfo &info : m_workTypes)
    {
        if (info.type == type)
        {
            return &info;
        }
    }
    return nullptr;
}

void WorkModel::onWorkTimer()
{
    if (m_currentStatus != WorkStatus::Working)
    {
        m_workTimer->stop();
        return;
    }

    m_remainingTime--;

    if (m_remainingTime <= 0)
    {
        // 工作完成
        const WorkInfo *workInfo = getWorkInfo(m_currentWorkType);
        if (workInfo)
        {
            qDebug() << "工作完成！获得" << workInfo->experienceReward << "经验值";

            // 触发经验值增加事件
            AddExperienceEvent expEvent;
            expEvent.experience = workInfo->experienceReward;
            EventMgr::GetInstance().SendEvent(expEvent);

            // 如果是光合作用，随机生成阳光
            if (m_currentWorkType == WorkType::Photosynthesis)
            {
                generateSunshine();
            }
            // 如果是挖矿，随机生成矿石
            else if (m_currentWorkType == WorkType::Mining)
            {
                generateMinerals();
            }
            // 如果是冒险，随机生成木头
            else if (m_currentWorkType == WorkType::Adventure)
            {
                generateWoods();
            }
        }

        if (m_continuousMode)
        {
            // 如果是连续模式，自动开始下一轮工作
            qDebug() << "自动开始下一轮工作：" << workInfo->name;
            m_remainingTime = workInfo->workDuration; // 重置剩余时间
            fireWorkStatusUpdate();
        }
        else
        {
            // 停止工作
            m_currentStatus = WorkStatus::Idle;
            m_workTimer->stop();
            fireWorkStatusUpdate();
        }
    }
    else
    {
        // 更新剩余时间
        fireWorkStatusUpdate();
    }
}

void WorkModel::generateSunshine()
{
    // 阳光物品ID范围：6-10 (微光阳光到神圣阳光)
    // 稀有度概率：普通60%，稀有25%，史诗12%，传说3%

    QRandomGenerator *rng = QRandomGenerator::global();
    int random = rng->bounded(100); // 0-99

    int sunshineId;
    int count = 1;

    if (random < 60)
    {
        // 60% 概率获得微光阳光 (ID=6, 普通)
        sunshineId = 6;
        count = rng->bounded(1, 4); // 1-3个
    }
    else if (random < 85)
    {
        // 25% 概率获得温暖阳光 (ID=7, 稀有)
        sunshineId = 7;
        count = rng->bounded(1, 3); // 1-2个
    }
    else if (random < 97)
    {
        // 12% 概率获得炽热阳光或灿烂阳光 (ID=8或9, 史诗)
        sunshineId = rng->bounded(8, 10); // 8或9
        count = 1;
    }
    else
    {
        // 3% 概率获得神圣阳光 (ID=10, 传说)
        sunshineId = 10;
        count = 1;
    }

    // 触发添加物品事件
    AddItemEvent itemEvent;
    itemEvent.itemId = sunshineId;
    itemEvent.count = count;
    EventMgr::GetInstance().SendEvent(itemEvent);

    qDebug() << "光合作用产生阳光! 物品ID:" << sunshineId << "数量:" << count;
}

void WorkModel::generateMinerals()
{
    // 矿石物品ID范围：11-15 (粗糙矿石到传说矿石)
    // 稀有度概率：普通50%，稀有30%，史诗15%，传说5%

    QRandomGenerator *rng = QRandomGenerator::global();
    int random = rng->bounded(100); // 0-99

    int mineralId;
    int count = 1;

    if (random < 50)
    {
        // 50% 概率获得粗糙矿石 (ID=11, 普通)
        mineralId = 11;
        count = rng->bounded(2, 6); // 2-5个
    }
    else if (random < 80)
    {
        // 30% 概率获得普通矿石 (ID=12, 稀有)
        mineralId = 12;
        count = rng->bounded(1, 4); // 1-3个
    }
    else if (random < 95)
    {
        // 15% 概率获得优质矿石或稀有矿石 (ID=13或14, 史诗)
        mineralId = rng->bounded(13, 15); // 13或14
        count = 1;
    }
    else
    {
        // 5% 概率获得传说矿石 (ID=15, 传说)
        mineralId = 15;
        count = 1;
    }

    // 触发添加物品事件
    AddItemEvent itemEvent;
    itemEvent.itemId = mineralId;
    itemEvent.count = count;
    EventMgr::GetInstance().SendEvent(itemEvent);

    qDebug() << "挖矿产生矿石! 物品ID:" << mineralId << "数量:" << count;
}

void WorkModel::generateWoods()
{
    // 木头物品ID范围：16-20 (枯木到神木)
    // 稀有度概率：普通55%，稀有30%，史诗10%，传说5%

    QRandomGenerator *rng = QRandomGenerator::global();
    int random = rng->bounded(100); // 0-99

    int woodId;
    int count = 1;

    if (random < 55)
    {
        // 55% 概率获得枯木 (ID=16, 普通)
        woodId = 16;
        count = rng->bounded(2, 5); // 2-4个
    }
    else if (random < 85)
    {
        // 30% 概率获得普通木材 (ID=17, 稀有)
        woodId = 17;
        count = rng->bounded(1, 3); // 1-2个
    }
    else if (random < 95)
    {
        // 10% 概率获得优质木材或稀有木材 (ID=18或19, 史诗)
        woodId = rng->bounded(18, 20); // 18或19
        count = 1;
    }
    else
    {
        // 5% 概率获得神木 (ID=20, 传说)
        woodId = 20;
        count = 1;
    }

    // 触发添加物品事件
    AddItemEvent itemEvent;
    itemEvent.itemId = woodId;
    itemEvent.count = count;
    EventMgr::GetInstance().SendEvent(itemEvent);

    qDebug() << "冒险产生木头! 物品ID:" << woodId << "数量:" << count;
}

void WorkModel::fireWorkStatusUpdate()
{
    m_trigger.fire(PROP_ID_WORK_STATUS_UPDATE);
}