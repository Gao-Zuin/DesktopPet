#include "WorkModel.h"
#include "../common/EventMgr.h"
#include "../common/EventDefine.h"
#include <QDebug>
#include <QRandomGenerator>

WorkModel::WorkModel() noexcept
    : m_trigger(), m_currentStatus(WorkStatus::Idle), m_currentWorkType(WorkType::Photosynthesis), m_remainingTime(0), m_continuousMode(false), m_workTimer(nullptr)
{
    initializeWorkTypes();

    // 初始化工作系统等级（默认为基础级）
    m_workSystemLevels[WorkType::Photosynthesis] = WorkSystemLevel::Basic;
    m_workSystemLevels[WorkType::Mining] = WorkSystemLevel::Basic;
    m_workSystemLevels[WorkType::Adventure] = WorkSystemLevel::Basic;

    // 初始化工作系统加成效果
    initializeWorkSystemBonuses();

    // 创建工作定时器
    m_workTimer = new QTimer(this);
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
    // 获取当前光合作用系统的加成效果
    float dropRateMultiplier = getDropRateMultiplier(WorkType::Photosynthesis);
    float qualityBonus = getQualityBonus(WorkType::Photosynthesis);

    // 阳光物品ID范围：6-10 (微光阳光到神圣阳光)
    // 基础概率：普通60%，稀有25%，史诗12%，传说3%
    // 应用品质加成后调整概率

    QRandomGenerator *rng = QRandomGenerator::global();
    int random = rng->bounded(100); // 0-99

    // 应用品质加成，提高高品质物品的概率
    float adjustedRandom = random - (qualityBonus * 100);
    if (adjustedRandom < 0)
        adjustedRandom = 0;

    int sunshineId;
    int count = 1;

    if (adjustedRandom < 60)
    {
        // 基础概率60%获得微光阳光 (ID=6, 普通)
        sunshineId = 6;
        count = rng->bounded(1, 4); // 1-3个
    }
    else if (adjustedRandom < 85)
    {
        // 25% 概率获得温暖阳光 (ID=7, 稀有)
        sunshineId = 7;
        count = rng->bounded(1, 3); // 1-2个
    }
    else if (adjustedRandom < 97)
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

    // 应用掉落率倍数
    int finalCount = static_cast<int>(count * dropRateMultiplier);
    if (finalCount < 1)
        finalCount = 1;

    // 触发添加物品事件
    AddItemEvent itemEvent;
    itemEvent.itemId = sunshineId;
    itemEvent.count = finalCount;
    EventMgr::GetInstance().SendEvent(itemEvent);

    qDebug() << "光合作用产生阳光! 物品ID:" << sunshineId << "数量:" << finalCount
             << "(等级加成: x" << dropRateMultiplier << ", 品质加成: +" << (qualityBonus * 100) << "%)";
}

void WorkModel::generateMinerals()
{
    // 获取当前挖矿系统的加成效果
    float dropRateMultiplier = getDropRateMultiplier(WorkType::Mining);
    float qualityBonus = getQualityBonus(WorkType::Mining);

    // 矿石物品ID范围：11-15 (粗糙矿石到传说矿石)
    // 基础概率：普通50%，稀有30%，史诗15%，传说5%
    // 应用品质加成后调整概率

    QRandomGenerator *rng = QRandomGenerator::global();
    int random = rng->bounded(100); // 0-99

    // 应用品质加成，提高高品质物品的概率
    float adjustedRandom = random - (qualityBonus * 100);
    if (adjustedRandom < 0)
        adjustedRandom = 0;

    int mineralId;
    int count = 1;

    if (adjustedRandom < 50)
    {
        // 50% 概率获得粗糙矿石 (ID=11, 普通)
        mineralId = 11;
        count = rng->bounded(2, 6); // 2-5个
    }
    else if (adjustedRandom < 80)
    {
        // 30% 概率获得普通矿石 (ID=12, 稀有)
        mineralId = 12;
        count = rng->bounded(1, 4); // 1-3个
    }
    else if (adjustedRandom < 95)
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

    // 应用掉落率倍数
    int finalCount = static_cast<int>(count * dropRateMultiplier);
    if (finalCount < 1)
        finalCount = 1;

    // 触发添加物品事件
    AddItemEvent itemEvent;
    itemEvent.itemId = mineralId;
    itemEvent.count = finalCount;
    EventMgr::GetInstance().SendEvent(itemEvent);

    qDebug() << "挖矿产生矿石! 物品ID:" << mineralId << "数量:" << finalCount
             << "(等级加成: x" << dropRateMultiplier << ", 品质加成: +" << (qualityBonus * 100) << "%)";
}

void WorkModel::generateWoods()
{
    // 获取当前冒险系统的加成效果
    float dropRateMultiplier = getDropRateMultiplier(WorkType::Adventure);
    float qualityBonus = getQualityBonus(WorkType::Adventure);

    // 木头物品ID范围：16-20 (枯木到神木)
    // 基础概率：普通55%，稀有30%，史诗10%，传说5%
    // 应用品质加成后调整概率

    QRandomGenerator *rng = QRandomGenerator::global();
    int random = rng->bounded(100); // 0-99

    // 应用品质加成，提高高品质物品的概率
    float adjustedRandom = random - (qualityBonus * 100);
    if (adjustedRandom < 0)
        adjustedRandom = 0;

    int woodId;
    int count = 1;

    if (adjustedRandom < 55)
    {
        // 55% 概率获得枯木 (ID=16, 普通)
        woodId = 16;
        count = rng->bounded(2, 5); // 2-4个
    }
    else if (adjustedRandom < 85)
    {
        // 30% 概率获得普通木材 (ID=17, 稀有)
        woodId = 17;
        count = rng->bounded(1, 3); // 1-2个
    }
    else if (adjustedRandom < 95)
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

    // 应用掉落率倍数
    int finalCount = static_cast<int>(count * dropRateMultiplier);
    if (finalCount < 1)
        finalCount = 1;

    // 触发添加物品事件
    AddItemEvent itemEvent;
    itemEvent.itemId = woodId;
    itemEvent.count = finalCount;
    EventMgr::GetInstance().SendEvent(itemEvent);

    qDebug() << "冒险产生木头! 物品ID:" << woodId << "数量:" << finalCount
             << "(等级加成: x" << dropRateMultiplier << ", 品质加成: +" << (qualityBonus * 100) << "%)";
}

void WorkModel::fireWorkStatusUpdate()
{
    m_trigger.fire(PROP_ID_WORK_STATUS_UPDATE);
}

// 初始化工作系统加成效果
void WorkModel::initializeWorkSystemBonuses()
{
    // 光合作用系统加成
    m_workSystemBonuses[WorkType::Photosynthesis][WorkSystemLevel::Basic] = {1.0f, 0.0f, {}};
    m_workSystemBonuses[WorkType::Photosynthesis][WorkSystemLevel::Advanced] = {1.5f, 0.2f, {201, 202}};
    m_workSystemBonuses[WorkType::Photosynthesis][WorkSystemLevel::Expert] = {2.0f, 0.4f, {203, 204, 205}};
    m_workSystemBonuses[WorkType::Photosynthesis][WorkSystemLevel::Master] = {3.0f, 0.6f, {206, 207, 208, 209}};

    // 挖矿系统加成
    m_workSystemBonuses[WorkType::Mining][WorkSystemLevel::Basic] = {1.0f, 0.0f, {}};
    m_workSystemBonuses[WorkType::Mining][WorkSystemLevel::Advanced] = {1.5f, 0.2f, {211, 212}};
    m_workSystemBonuses[WorkType::Mining][WorkSystemLevel::Expert] = {2.0f, 0.4f, {213, 214, 215}};
    m_workSystemBonuses[WorkType::Mining][WorkSystemLevel::Master] = {3.0f, 0.6f, {216, 217, 218, 219}};

    // 冒险系统加成
    m_workSystemBonuses[WorkType::Adventure][WorkSystemLevel::Basic] = {1.0f, 0.0f, {}};
    m_workSystemBonuses[WorkType::Adventure][WorkSystemLevel::Advanced] = {1.5f, 0.2f, {221, 222}};
    m_workSystemBonuses[WorkType::Adventure][WorkSystemLevel::Expert] = {2.0f, 0.4f, {223, 224, 225}};
    m_workSystemBonuses[WorkType::Adventure][WorkSystemLevel::Master] = {3.0f, 0.6f, {226, 227, 228, 229}};
}

// 工作系统等级管理
void WorkModel::setWorkSystemLevel(WorkType workType, WorkSystemLevel level) noexcept
{
    if (m_workSystemLevels[workType] != level)
    {
        m_workSystemLevels[workType] = level;
        emit workSystemLevelChanged(workType, level);
        qDebug() << "工作系统等级已更新：" << static_cast<int>(workType) << "-> 等级" << static_cast<int>(level);
    }
}

WorkSystemLevel WorkModel::getWorkSystemLevel(WorkType workType) const noexcept
{
    return m_workSystemLevels.value(workType, WorkSystemLevel::Basic);
}

// 获取当前工作系统的加成效果
float WorkModel::getDropRateMultiplier(WorkType workType) const noexcept
{
    WorkSystemLevel level = getWorkSystemLevel(workType);
    return m_workSystemBonuses[workType][level].dropRateMultiplier;
}

float WorkModel::getQualityBonus(WorkType workType) const noexcept
{
    WorkSystemLevel level = getWorkSystemLevel(workType);
    return m_workSystemBonuses[workType][level].qualityBonus;
}

QVector<int> WorkModel::getUnlockedItems(WorkType workType) const noexcept
{
    WorkSystemLevel level = getWorkSystemLevel(workType);
    return m_workSystemBonuses[workType][level].unlockedItems;
}