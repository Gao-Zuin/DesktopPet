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
    EventMgr::GetInstance().RegisterEvent(this);
    // 初始化每个宠物类型的默认数据
    for (int i = static_cast<int>(PetType::Spider); i <= static_cast<int>(PetType::Cassidy); ++i)
    {
        PetType type = static_cast<PetType>(i);
        PetInfo info;
        info.petType = type;

        // 设置每个宠物类型的默认动画
        switch (type)
        {
        case PetType::Spider:
            info.currentAnimation = ":/resources/gif/spider.gif";
            break;
        case PetType::Cassidy:
            info.currentAnimation = ":/resources/img/cassidy.png";
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

PetModel::~PetModel() noexcept
{
    // 在析构函数中注销事件
    EventMgr::GetInstance().UnregisterEvent(this);
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
    if (m_current_info.petType != type)
    {
        // 保存当前宠物的数据
        save_current_pet_data();

        // 切换到新的宠物类型
        load_pet_data_for_type(type);

        // 触发所有相关属性的更新通知
        m_trigger.fire(PROP_ID_PET_TYPE);
        m_trigger.fire(PROP_ID_PET_LEVEL);
        m_trigger.fire(PROP_ID_PET_EXPERIENCE);
        m_trigger.fire(PROP_ID_PET_MONEY);
        m_trigger.fire(PROP_ID_PET_ANIMATION);
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
    QJsonObject json;
    json["name"] = m_current_info.name;
    json["petType"] = static_cast<int>(m_current_info.petType);
    json["level"] = m_current_info.level;
    json["experience"] = m_current_info.experience;
    json["experienceToNextLevel"] = m_current_info.experienceToNextLevel;
    json["money"] = m_current_info.money;
    json["positionX"] = m_current_info.position.x();
    json["positionY"] = m_current_info.position.y();
    json["sizeWidth"] = m_current_info.size.width();
    json["sizeHeight"] = m_current_info.size.height();
    json["speed"] = m_current_info.speed;
    json["isVisible"] = m_current_info.isVisible;
    json["currentAnimation"] = m_current_info.currentAnimation;
    json["state"] = static_cast<int>(m_current_info.state);

    QJsonDocument doc(json);

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
    }
}

void PetModel::load_from_file(const QString &filename)
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString fullPath = appDataPath + "/" + filename;

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        return; // 文件不存在，使用默认值
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
    {
        return; // JSON格式错误，使用默认值
    }

    QJsonObject json = doc.object();

    // 加载数据
    if (json.contains("name"))
        m_current_info.name = json["name"].toString();
    if (json.contains("petType"))
        m_current_info.petType = static_cast<PetType>(json["petType"].toInt());
    if (json.contains("level"))
        m_current_info.level = json["level"].toInt();
    if (json.contains("experience"))
        m_current_info.experience = json["experience"].toInt();
    if (json.contains("experienceToNextLevel"))
        m_current_info.experienceToNextLevel = json["experienceToNextLevel"].toInt();
    if (json.contains("money"))
        m_current_info.money = json["money"].toInt();
    if (json.contains("positionX") && json.contains("positionY"))
    {
        m_current_info.position = QPoint(json["positionX"].toInt(), json["positionY"].toInt());
    }
    if (json.contains("sizeWidth") && json.contains("sizeHeight"))
    {
        m_current_info.size = QSize(json["sizeWidth"].toInt(), json["sizeHeight"].toInt());
    }
    if (json.contains("speed"))
        m_current_info.speed = json["speed"].toInt();
    if (json.contains("isVisible"))
        m_current_info.isVisible = json["isVisible"].toBool();
    if (json.contains("currentAnimation"))
        m_current_info.currentAnimation = json["currentAnimation"].toString();
    if (json.contains("state"))
        m_current_info.state = static_cast<PetState>(json["state"].toInt());
}

void PetModel::save_current_pet_data() noexcept
{
    // 将当前宠物信息保存到对应的类型数据中
    m_pet_data[m_current_info.petType] = m_current_info;
}

void PetModel::load_pet_data_for_type(PetType type) noexcept
{
    // 从对应类型的数据中加载到当前宠物信息
    if (m_pet_data.contains(type))
    {
        m_current_info = m_pet_data[type];
    }
    else
    {
        // 如果没有该类型的数据，创建默认数据
        PetInfo defaultInfo;
        defaultInfo.petType = type;

        // 设置默认动画
        switch (type)
        {
        case PetType::Spider:
            defaultInfo.currentAnimation = ":/resources/gif/spider.gif";
            break;
        case PetType::Cassidy:
            defaultInfo.currentAnimation = ":/resources/img/cassidy.png";
            break;
        }

        m_pet_data[type] = defaultInfo;
        m_current_info = defaultInfo;
    }
}
