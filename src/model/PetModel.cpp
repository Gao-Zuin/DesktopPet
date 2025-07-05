#include "PetModel.h"
#include "../common/PropertyIds.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <cmath>
#include "../common/EventMgr.h"

PetModel::PetModel() noexcept
{
    // 在构造函数中注册事件
    EventMgr::GetInstance().RegisterEvent<TestEvent>(this);
    EventMgr::GetInstance().RegisterEvent<AddExperienceEvent>(this);
    EventMgr::GetInstance().RegisterEvent<AddItemEvent>(this);

    // 初始化每个宠物类型的默认数据
    for (int i = static_cast<int>(PetType::Spider); i <= static_cast<int>(PetType::Cassidy); ++i)
    {
        PetType type = static_cast<PetType>(i);
        PetInfo info;
        info.petType = type;

        // 设置每个宠物类型的默认动画和名称
        switch (type)
        {
        case PetType::Spider:
            info.currentAnimation = ":/resources/gif/spider.gif";
            info.name = "小蜘蛛";
            break;
        case PetType::Cassidy:
            info.currentAnimation = ":/resources/img/cassidy.png";
            info.name = "卡西迪";
            break;
        }

        m_pet_data[type] = info;
    }

    // 设置默认当前宠物为Spider
    m_current_info = m_pet_data[PetType::Spider];
}

void PetModel::OnEvent(TestEvent event)
{
    // 测试使用
    std::cout << "[TestEvent]:TestInt为" << event.TestInt << "，TestString为" << event.TestString << std::endl;
    m_current_info.money = event.TestInt;
    m_current_info.name.append(event.TestString);
}

void PetModel::OnEvent(AddExperienceEvent event)
{
    // 处理打工获得经验值事件
    qDebug() << "[AddExperienceEvent]: 获得经验值" << event.experience;
    add_experience(event.experience);
}

void PetModel::OnEvent(AddItemEvent event)
{
    // 处理添加物品到背包事件
    qDebug() << "[AddItemEvent]: 添加物品到背包 ID:" << event.itemId << "数量:" << event.count;

    // 通过事件系统通知PetViewModel添加物品到背包
    // 由于PetModel不应该直接操作背包，我们需要通过ViewModel来处理
    // 这里我们可以发送一个命令或者触发一个属性更新
    // 暂时先输出日志，实际的添加逻辑将在ViewModel层处理
}

PetModel::~PetModel() noexcept
{
    // 在析构函数中注销事件
    EventMgr::GetInstance().UnregisterEvent<TestEvent>(this);
    EventMgr::GetInstance().UnregisterEvent<AddExperienceEvent>(this);
    EventMgr::GetInstance().UnregisterEvent<AddItemEvent>(this);
}

void PetModel::change_position(const QPoint &position) noexcept
{
    // 只有当位置变化超过阈值时才更新，减少频繁通知
    QPoint diff = position - m_current_info.position;
    if (abs(diff.x()) > 2 || abs(diff.y()) > 2)
    {
        m_current_info.position = position;

        // 同步更新当前宠物类型的数据
        m_pet_data[m_current_info.petType].position = position;

        m_trigger.fire(PROP_ID_PET_POSITION);
    }
}

void PetModel::change_state(PetState state) noexcept
{
    if (m_current_info.state != state)
    {
        m_current_info.state = state;
        m_pet_data[m_current_info.petType].state = state;
        m_trigger.fire(PROP_ID_PET_STATE);
    }
}

void PetModel::change_animation(const QString &animation) noexcept
{
    if (m_current_info.currentAnimation != animation)
    {
        m_current_info.currentAnimation = animation;
        m_pet_data[m_current_info.petType].currentAnimation = animation;
        m_trigger.fire(PROP_ID_PET_ANIMATION);
    }
}

void PetModel::change_visibility(bool visible) noexcept
{
    if (m_current_info.isVisible != visible)
    {
        m_current_info.isVisible = visible;
        m_pet_data[m_current_info.petType].isVisible = visible;
        m_trigger.fire(PROP_ID_PET_VISIBLE);
    }
}

void PetModel::change_size(const QSize &size) noexcept
{
    if (m_current_info.size != size)
    {
        m_current_info.size = size;
        m_pet_data[m_current_info.petType].size = size;
        m_trigger.fire(PROP_ID_PET_SIZE);
    }
}

void PetModel::change_pet_type(PetType type) noexcept
{
    qDebug() << "切换宠物类型前:" << static_cast<int>(m_current_info.petType)
             << "名称:" << m_current_info.name
             << "等级:" << m_current_info.level
             << "经验:" << m_current_info.experience
             << "金钱:" << m_current_info.money;

    if (m_current_info.petType != type)
    {
        // 保存当前宠物的数据
        save_current_pet_data();

        // 切换到新的宠物类型
        load_pet_data_for_type(type);

        qDebug() << "切换宠物类型后:" << static_cast<int>(m_current_info.petType)
                 << "名称:" << m_current_info.name
                 << "等级:" << m_current_info.level
                 << "经验:" << m_current_info.experience
                 << "金钱:" << m_current_info.money;

        // 触发所有相关属性的更新通知
        m_trigger.fire(PROP_ID_PET_TYPE);
        m_trigger.fire(PROP_ID_PET_LEVEL);
        m_trigger.fire(PROP_ID_PET_EXPERIENCE);
        m_trigger.fire(PROP_ID_PET_MONEY);
        m_trigger.fire(PROP_ID_PET_ANIMATION);

        // 自动保存数据
        save_to_file("pet_data.json");
    }
    else
    {
        qDebug() << "宠物类型未变化，无需切换";
    }
}

// 经验值和等级系统实现
void PetModel::add_experience(int exp) noexcept
{
    if (exp <= 0)
        return;

    m_current_info.experience += exp;
    m_pet_data[m_current_info.petType].experience = m_current_info.experience;
    m_trigger.fire(PROP_ID_PET_EXPERIENCE);

    // 检查是否升级
    check_level_up();
}

void PetModel::set_level(int level) noexcept
{
    if (level < 1)
        level = 1;
    if (m_current_info.level != level)
    {
        m_current_info.level = level;
        m_current_info.experienceToNextLevel = calculate_experience_needed(level);

        // 同步到对应宠物类型数据
        m_pet_data[m_current_info.petType].level = level;
        m_pet_data[m_current_info.petType].experienceToNextLevel = m_current_info.experienceToNextLevel;

        m_trigger.fire(PROP_ID_PET_LEVEL);
    }
}

void PetModel::add_money(int amount) noexcept
{
    if (amount <= 0)
        return;

    m_current_info.money += amount;
    m_pet_data[m_current_info.petType].money = m_current_info.money;
    m_trigger.fire(PROP_ID_PET_MONEY);
}

void PetModel::spend_money(int amount) noexcept
{
    if (amount <= 0)
        return;

    if (m_current_info.money >= amount)
    {
        m_current_info.money -= amount;
        m_pet_data[m_current_info.petType].money = m_current_info.money;
        m_trigger.fire(PROP_ID_PET_MONEY);
    }
}

// 等级计算辅助方法
int PetModel::calculate_experience_needed(int level) const noexcept
{
    // 使用指数增长公式：基础经验 * 等级^1.5
    return static_cast<int>(100 * std::pow(level, 1.5));
}

void PetModel::check_level_up() noexcept
{
    while (m_current_info.experience >= m_current_info.experienceToNextLevel)
    {
        m_current_info.experience -= m_current_info.experienceToNextLevel;
        m_current_info.level++;
        m_current_info.experienceToNextLevel = calculate_experience_needed(m_current_info.level);

        // 同步到对应宠物类型数据
        m_pet_data[m_current_info.petType].experience = m_current_info.experience;
        m_pet_data[m_current_info.petType].level = m_current_info.level;
        m_pet_data[m_current_info.petType].experienceToNextLevel = m_current_info.experienceToNextLevel;

        m_trigger.fire(PROP_ID_PET_LEVEL);
    }
}

// 持久化方法实现
void PetModel::save_to_file(const QString &filename) const
{
    QJsonObject rootJson;

    // 保存当前宠物类型
    rootJson["currentPetType"] = static_cast<int>(m_current_info.petType);

    // 保存所有宠物数据
    QJsonObject petsJson;
    for (auto it = m_pet_data.constBegin(); it != m_pet_data.constEnd(); ++it)
    {
        PetType type = it.key();
        const PetInfo &info = it.value();

        QJsonObject petJson;
        petJson["name"] = info.name;
        petJson["level"] = info.level;
        petJson["experience"] = info.experience;
        petJson["experienceToNextLevel"] = info.experienceToNextLevel;
        petJson["money"] = info.money;
        petJson["currentAnimation"] = info.currentAnimation;
        petJson["state"] = static_cast<int>(info.state);

        petsJson[QString::number(static_cast<int>(type))] = petJson;
    }
    rootJson["pets"] = petsJson;

    // 保存全局设置（位置、大小等所有宠物共享的属性）
    QJsonObject globalJson;
    globalJson["positionX"] = m_current_info.position.x();
    globalJson["positionY"] = m_current_info.position.y();
    globalJson["sizeWidth"] = m_current_info.size.width();
    globalJson["sizeHeight"] = m_current_info.size.height();
    globalJson["speed"] = m_current_info.speed;
    globalJson["isVisible"] = m_current_info.isVisible;
    rootJson["globalSettings"] = globalJson;

    QJsonDocument doc(rootJson);

    // 获取应用数据存储路径
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists())
    {
        dir.mkpath(appDataPath);
    }

    QString fullPath = appDataPath + "/" + filename;
    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        file.close();
        qDebug() << "宠物数据已保存到:" << fullPath;
    }
}

void PetModel::load_from_file(const QString &filename)
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString fullPath = appDataPath + "/" + filename;

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "宠物数据文件不存在，使用默认值:" << fullPath;
        return; // 文件不存在，使用默认值
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
    {
        qDebug() << "JSON格式错误，使用默认值";
        return; // JSON格式错误，使用默认值
    }

    QJsonObject rootJson = doc.object();

    // 检查是否为新格式（包含pets字段）
    if (rootJson.contains("pets"))
    {
        qDebug() << "加载新格式的宠物数据";

        // 加载所有宠物数据
        QJsonObject petsJson = rootJson["pets"].toObject();
        for (auto it = petsJson.constBegin(); it != petsJson.constEnd(); ++it)
        {
            int petTypeInt = it.key().toInt();
            PetType petType = static_cast<PetType>(petTypeInt);
            QJsonObject petJson = it.value().toObject();

            // 如果该宠物类型已存在，使用现有数据作为基础
            PetInfo info;
            if (m_pet_data.contains(petType))
            {
                info = m_pet_data[petType];
            }
            else
            {
                info.petType = petType;
                // 设置默认动画和名称
                switch (petType)
                {
                case PetType::Spider:
                    info.currentAnimation = ":/resources/gif/spider.gif";
                    info.name = "小蜘蛛";
                    break;
                case PetType::Cassidy:
                    info.currentAnimation = ":/resources/img/cassidy.png";
                    info.name = "卡西迪";
                    break;
                }
            }

            // 覆盖从文件加载的数据
            if (petJson.contains("name"))
                info.name = petJson["name"].toString();
            if (petJson.contains("level"))
                info.level = petJson["level"].toInt();
            if (petJson.contains("experience"))
                info.experience = petJson["experience"].toInt();
            if (petJson.contains("experienceToNextLevel"))
                info.experienceToNextLevel = petJson["experienceToNextLevel"].toInt();
            if (petJson.contains("money"))
                info.money = petJson["money"].toInt();
            if (petJson.contains("currentAnimation"))
                info.currentAnimation = petJson["currentAnimation"].toString();
            if (petJson.contains("state"))
                info.state = static_cast<PetState>(petJson["state"].toInt());

            m_pet_data[petType] = info;
            qDebug() << "加载宠物数据 类型:" << static_cast<int>(petType)
                     << "名称:" << info.name
                     << "等级:" << info.level
                     << "经验:" << info.experience
                     << "金钱:" << info.money;
        }

        // 加载全局设置
        if (rootJson.contains("globalSettings"))
        {
            QJsonObject globalJson = rootJson["globalSettings"].toObject();
            QPoint position(100, 100);
            QSize size(200, 200);
            int speed = 50;
            bool isVisible = true;

            if (globalJson.contains("positionX") && globalJson.contains("positionY"))
            {
                position = QPoint(globalJson["positionX"].toInt(), globalJson["positionY"].toInt());
            }
            if (globalJson.contains("sizeWidth") && globalJson.contains("sizeHeight"))
            {
                size = QSize(globalJson["sizeWidth"].toInt(), globalJson["sizeHeight"].toInt());
            }
            if (globalJson.contains("speed"))
            {
                speed = globalJson["speed"].toInt();
            }
            if (globalJson.contains("isVisible"))
            {
                isVisible = globalJson["isVisible"].toBool();
            }

            // 应用全局设置到所有宠物
            for (auto &info : m_pet_data)
            {
                info.position = position;
                info.size = size;
                info.speed = speed;
                info.isVisible = isVisible;
            }
        }

        // 设置当前宠物类型
        PetType currentType = PetType::Spider;
        if (rootJson.contains("currentPetType"))
        {
            currentType = static_cast<PetType>(rootJson["currentPetType"].toInt());
        }

        if (m_pet_data.contains(currentType))
        {
            m_current_info = m_pet_data[currentType];
            qDebug() << "设置当前宠物:" << static_cast<int>(currentType) << "名称:" << m_current_info.name;
        }
    }
    else
    {
        // 兼容旧格式
        qDebug() << "加载旧格式的宠物数据，仅应用到当前宠物";

        // 加载数据到当前宠物
        if (rootJson.contains("name"))
            m_current_info.name = rootJson["name"].toString();
        if (rootJson.contains("petType"))
            m_current_info.petType = static_cast<PetType>(rootJson["petType"].toInt());
        if (rootJson.contains("level"))
            m_current_info.level = rootJson["level"].toInt();
        if (rootJson.contains("experience"))
            m_current_info.experience = rootJson["experience"].toInt();
        if (rootJson.contains("experienceToNextLevel"))
            m_current_info.experienceToNextLevel = rootJson["experienceToNextLevel"].toInt();
        if (rootJson.contains("money"))
            m_current_info.money = rootJson["money"].toInt();
        if (rootJson.contains("positionX") && rootJson.contains("positionY"))
        {
            m_current_info.position = QPoint(rootJson["positionX"].toInt(), rootJson["positionY"].toInt());
        }
        if (rootJson.contains("sizeWidth") && rootJson.contains("sizeHeight"))
        {
            m_current_info.size = QSize(rootJson["sizeWidth"].toInt(), rootJson["sizeHeight"].toInt());
        }
        if (rootJson.contains("speed"))
            m_current_info.speed = rootJson["speed"].toInt();
        if (rootJson.contains("isVisible"))
            m_current_info.isVisible = rootJson["isVisible"].toBool();
        if (rootJson.contains("currentAnimation"))
            m_current_info.currentAnimation = rootJson["currentAnimation"].toString();
        if (rootJson.contains("state"))
            m_current_info.state = static_cast<PetState>(rootJson["state"].toInt());

        // 更新对应宠物类型的数据
        m_pet_data[m_current_info.petType] = m_current_info;
    }
}

void PetModel::save_current_pet_data() noexcept
{
    // 将当前宠物信息保存到对应的类型数据中
    qDebug() << "保存宠物数据:" << static_cast<int>(m_current_info.petType)
             << "名称:" << m_current_info.name
             << "等级:" << m_current_info.level
             << "经验:" << m_current_info.experience
             << "金钱:" << m_current_info.money;
    m_pet_data[m_current_info.petType] = m_current_info;
}

void PetModel::load_pet_data_for_type(PetType type) noexcept
{
    // 从对应类型的数据中加载到当前宠物信息
    if (m_pet_data.contains(type))
    {
        qDebug() << "加载已存在的宠物数据:" << static_cast<int>(type);
        m_current_info = m_pet_data[type];
        qDebug() << "加载的数据 - 名称:" << m_current_info.name
                 << "等级:" << m_current_info.level
                 << "经验:" << m_current_info.experience
                 << "金钱:" << m_current_info.money;
    }
    else
    {
        qDebug() << "创建新的宠物数据:" << static_cast<int>(type);
        // 如果没有该类型的数据，创建默认数据
        PetInfo defaultInfo;
        defaultInfo.petType = type;

        // 设置默认动画和名称
        switch (type)
        {
        case PetType::Spider:
            defaultInfo.currentAnimation = ":/resources/gif/spider.gif";
            defaultInfo.name = "小蜘蛛";
            break;
        case PetType::Cassidy:
            defaultInfo.currentAnimation = ":/resources/img/cassidy.png";
            defaultInfo.name = "卡西迪";
            break;
        }

        m_pet_data[type] = defaultInfo;
        m_current_info = defaultInfo;
        qDebug() << "创建的默认数据 - 名称:" << m_current_info.name
                 << "等级:" << m_current_info.level
                 << "经验:" << m_current_info.experience
                 << "金钱:" << m_current_info.money;
    }
}
