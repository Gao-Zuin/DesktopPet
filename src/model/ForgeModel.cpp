#include "ForgeModel.h"
#include "BackpackModel.h"
#include "CollectionModel.h"
#include "WorkModel.h"
#include "../common/PropertyIds.h"
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <QDebug>
#include <QDateTime>

ForgeModel::ForgeModel(QObject *parent)
    : QObject(parent)
    , m_totalForgeCount(0)
    , m_successfulForgeCount(0)
{
    // 初始化工作系统等级
    m_workSystemLevels[WorkType::Photosynthesis] = WorkSystemLevel::Basic;
    
    // 初始化默认配方和升级路径
    initialize();
}

ForgeModel::~ForgeModel()
{
}

void ForgeModel::initialize()
{
    initializeDefaultRecipes();
    initializeWorkUpgrades();
}

void ForgeModel::initializeDefaultRecipes()
{
    m_recipes.clear();
    
    // ========== 阳光精炼配方 ==========
    
    // 配方1: 3个微光阳光 -> 1个温暖阳光 (无需催化剂)
    ForgeRecipe sunlightRefine1;
    sunlightRefine1.recipeId = 1;
    sunlightRefine1.name = "阳光精炼 - 温暖";
    sunlightRefine1.description = "将3个微光阳光精炼成1个温暖阳光";
    sunlightRefine1.type = ForgeRecipeType::ItemUpgrade;
    sunlightRefine1.materials = {
        ForgeMaterial(SUNSHINE_WEAK_ID, 3, false)    // 3个微光阳光
    };
    sunlightRefine1.outputs = {
        ForgeOutput(SUNSHINE_WARM_ID, 1, 1.0f)       // 1个温暖阳光，100%成功率
    };
    sunlightRefine1.unlockLevel = 1;
    sunlightRefine1.requiresCatalyst = false;
    m_recipes.append(sunlightRefine1);
    
    // 配方2: 3个温暖阳光 -> 1个炽热阳光
    ForgeRecipe sunlightRefine2;
    sunlightRefine2.recipeId = 2;
    sunlightRefine2.name = "阳光精炼 - 炽热";
    sunlightRefine2.description = "将3个温暖阳光精炼成1个炽热阳光";
    sunlightRefine2.type = ForgeRecipeType::ItemUpgrade;
    sunlightRefine2.materials = {
        ForgeMaterial(SUNSHINE_WARM_ID, 3, false)    // 3个温暖阳光
    };
    sunlightRefine2.outputs = {
        ForgeOutput(SUNSHINE_HOT_ID, 1, 1.0f)        // 1个炽热阳光
    };
    sunlightRefine2.unlockLevel = 3;
    sunlightRefine2.requiresCatalyst = false;
    m_recipes.append(sunlightRefine2);
    
    // 配方3: 3个炽热阳光 -> 1个灿烂阳光
    ForgeRecipe sunlightRefine3;
    sunlightRefine3.recipeId = 3;
    sunlightRefine3.name = "阳光精炼 - 灿烂";
    sunlightRefine3.description = "将3个炽热阳光精炼成1个灿烂阳光";
    sunlightRefine3.type = ForgeRecipeType::ItemUpgrade;
    sunlightRefine3.materials = {
        ForgeMaterial(SUNSHINE_HOT_ID, 3, false)     // 3个炽热阳光
    };
    sunlightRefine3.outputs = {
        ForgeOutput(SUNSHINE_BRILLIANT_ID, 1, 1.0f)  // 1个灿烂阳光
    };
    sunlightRefine3.unlockLevel = 5;
    sunlightRefine3.requiresCatalyst = false;
    m_recipes.append(sunlightRefine3);
    
    // 配方4: 3个灿烂阳光 -> 1个神圣阳光
    ForgeRecipe sunlightRefine4;
    sunlightRefine4.recipeId = 4;
    sunlightRefine4.name = "阳光精炼 - 神圣";
    sunlightRefine4.description = "将3个灿烂阳光精炼成1个神圣阳光";
    sunlightRefine4.type = ForgeRecipeType::ItemUpgrade;
    sunlightRefine4.materials = {
        ForgeMaterial(SUNSHINE_BRILLIANT_ID, 3, false) // 3个灿烂阳光
    };
    sunlightRefine4.outputs = {
        ForgeOutput(SUNSHINE_SACRED_ID, 1, 1.0f)     // 1个神圣阳光
    };
    sunlightRefine4.unlockLevel = 8;
    sunlightRefine4.requiresCatalyst = false;
    m_recipes.append(sunlightRefine4);
    
    // ========== 物品通用强化配方 ==========
    
    // 配方5: 3个普通品质物品 -> 1个稀有品质物品 (需要温暖阳光催化剂)
    ForgeRecipe itemUpgrade1;
    itemUpgrade1.recipeId = 5;
    itemUpgrade1.name = "物品强化 - 稀有";
    itemUpgrade1.description = "将3个普通品质物品强化成1个稀有品质物品";
    itemUpgrade1.type = ForgeRecipeType::ItemUpgrade;
    itemUpgrade1.materials = {
        ForgeMaterial(0, 3, false),                   // 3个任意普通品质物品 (ID=0表示通用)
        ForgeMaterial(SUNSHINE_WARM_ID, 1, true)      // 1个温暖阳光作为催化剂
    };
    itemUpgrade1.outputs = {
        ForgeOutput(0, 1, 0.8f)                       // 1个稀有品质物品，80%成功率
    };
    itemUpgrade1.unlockLevel = 3;
    itemUpgrade1.requiresCatalyst = true;
    m_recipes.append(itemUpgrade1);
    
    // 配方6: 3个稀有品质物品 -> 1个史诗品质物品 (需要炽热阳光催化剂)
    ForgeRecipe itemUpgrade2;
    itemUpgrade2.recipeId = 6;
    itemUpgrade2.name = "物品强化 - 史诗";
    itemUpgrade2.description = "将3个稀有品质物品强化成1个史诗品质物品";
    itemUpgrade2.type = ForgeRecipeType::ItemUpgrade;
    itemUpgrade2.materials = {
        ForgeMaterial(0, 3, false),                   // 3个任意稀有品质物品
        ForgeMaterial(SUNSHINE_HOT_ID, 1, true)       // 1个炽热阳光作为催化剂
    };
    itemUpgrade2.outputs = {
        ForgeOutput(0, 1, 0.7f)                       // 1个史诗品质物品，70%成功率
    };
    itemUpgrade2.unlockLevel = 5;
    itemUpgrade2.requiresCatalyst = true;
    m_recipes.append(itemUpgrade2);
    
    // 配方7: 3个史诗品质物品 -> 1个传说品质物品 (需要神圣阳光催化剂)
    ForgeRecipe itemUpgrade3;
    itemUpgrade3.recipeId = 7;
    itemUpgrade3.name = "物品强化 - 传说";
    itemUpgrade3.description = "将3个史诗品质物品强化成1个传说品质物品";
    itemUpgrade3.type = ForgeRecipeType::ItemUpgrade;
    itemUpgrade3.materials = {
        ForgeMaterial(0, 3, false),                   // 3个任意史诗品质物品
        ForgeMaterial(SUNSHINE_SACRED_ID, 1, true)    // 1个神圣阳光作为催化剂
    };
    itemUpgrade3.outputs = {
        ForgeOutput(0, 1, 0.6f)                       // 1个传说品质物品，60%成功率
    };
    itemUpgrade3.unlockLevel = 8;
    itemUpgrade3.requiresCatalyst = true;
    m_recipes.append(itemUpgrade3);
    
    qDebug() << "ForgeModel: Initialized" << m_recipes.size() << "default recipes";
}

void ForgeModel::initializeWorkUpgrades()
{
    m_workUpgrades.clear();
    
    // 光合作用工作系统升级：基础 → 进阶
    WorkSystemUpgrade photoUpgrade1;
    photoUpgrade1.workType = WorkType::Photosynthesis;
    photoUpgrade1.currentLevel = WorkSystemLevel::Basic;
    photoUpgrade1.targetLevel = WorkSystemLevel::Advanced;
    photoUpgrade1.upgradeMaterials = {
        ForgeMaterial(SUNSHINE_WARM_ID, 10, false),   // 10个温暖阳光
        ForgeMaterial(SUNSHINE_HOT_ID, 5, false)      // 5个炽热阳光
    };
    photoUpgrade1.dropRateMultiplier = 1.5f;  // 掉落率提升50%
    photoUpgrade1.qualityBonus = 0.2f;        // 20%几率获得更高品质
    photoUpgrade1.unlockedItems = {201, 202}; // 解锁新物品
    m_workUpgrades.append(photoUpgrade1);
    
    // 专家级升级：进阶 → 专家
    WorkSystemUpgrade photoUpgrade2;
    photoUpgrade2.workType = WorkType::Photosynthesis;
    photoUpgrade2.currentLevel = WorkSystemLevel::Advanced;
    photoUpgrade2.targetLevel = WorkSystemLevel::Expert;
    photoUpgrade2.upgradeMaterials = {
        ForgeMaterial(SUNSHINE_HOT_ID, 15, false),        // 15个炽热阳光
        ForgeMaterial(SUNSHINE_BRILLIANT_ID, 8, false),   // 8个灿烂阳光
        ForgeMaterial(1, 10, false)                       // 10个木材作为辅助材料
    };
    photoUpgrade2.dropRateMultiplier = 2.0f;
    photoUpgrade2.qualityBonus = 0.4f;
    photoUpgrade2.unlockedItems = {203, 204, 205};
    m_workUpgrades.append(photoUpgrade2);
    
    // 大师级升级：专家 → 大师
    WorkSystemUpgrade photoUpgrade3;
    photoUpgrade3.workType = WorkType::Photosynthesis;
    photoUpgrade3.currentLevel = WorkSystemLevel::Expert;
    photoUpgrade3.targetLevel = WorkSystemLevel::Master;
    photoUpgrade3.upgradeMaterials = {
        ForgeMaterial(SUNSHINE_BRILLIANT_ID, 20, false),  // 20个灿烂阳光
        ForgeMaterial(SUNSHINE_SACRED_ID, 10, false),     // 10个神圣阳光
        ForgeMaterial(5, 5, false)                        // 5个魔法水晶
    };
    photoUpgrade3.dropRateMultiplier = 3.0f;
    photoUpgrade3.qualityBonus = 0.6f;
    photoUpgrade3.unlockedItems = {206, 207, 208, 209};
    m_workUpgrades.append(photoUpgrade3);
    
    qDebug() << "ForgeModel: Initialized" << m_workUpgrades.size() << "work upgrades";
}

bool ForgeModel::canForge(int recipeId) const
{
    ForgeRecipe recipe = getRecipeById(recipeId);
    if (recipe.recipeId == 0) {
        return false;  // 配方不存在
    }
    
    if (!isRecipeUnlocked(recipeId)) {
        return false;  // 配方未解锁
    }
    
    return hasSufficientMaterials(recipe.materials);
}

bool ForgeModel::forgeItem(int recipeId)
{
    ForgeRecipe recipe = getRecipeById(recipeId);
    if (!canForge(recipeId)) {
        qDebug() << "ForgeModel: Cannot forge recipe" << recipeId;
        return false;
    }
    
    // 消耗材料
    if (!consumeMaterials(recipe.materials)) {
        qDebug() << "ForgeModel: Failed to consume materials for recipe" << recipeId;
        return false;
    }
    
    // 记录锻造历史
    ForgeHistory history;
    history.recipeId = recipeId;
    history.forgeTime = QDateTime::currentDateTime();
    
    // 记录使用的材料
    for (const auto& material : recipe.materials) {
        history.materialsCost.append(material);
    }
    
    m_totalForgeCount++;
    
    // 处理每个产出
    bool anySuccess = false;
    for (const auto& output : recipe.outputs) {
        bool success = rollForgeSuccess(output.successRate);
        if (success) {
            // 添加物品到背包
            if (m_backpackModel) {
                m_backpackModel->addItem(output.itemId, output.outputCount);
            }
            
            // 记录产出
            for (int i = 0; i < output.outputCount; ++i) {
                history.productsGained.append(output.itemId);
            }
            anySuccess = true;
        }
    }
    
    if (anySuccess) {
        m_successfulForgeCount++;
    }
    
    history.success = anySuccess;
    addForgeHistory(history);
    
    // 发射信号
    emit forgeCompleted(recipeId, anySuccess, history.productsGained);
    
    // 检查是否解锁新配方
    checkAndUnlockRecipes();
    
    qDebug() << "ForgeModel: Forge completed for recipe" << recipeId 
             << "Success:" << anySuccess 
             << "Produced items:" << history.productsGained.size();
    
    return anySuccess;
}

bool ForgeModel::hasSufficientMaterials(const QVector<ForgeMaterial>& materials) const
{
    if (!m_backpackModel) {
        return false;
    }
    
    for (const auto& material : materials) {
        int availableCount = m_backpackModel->getItemCount(material.itemId);
        if (availableCount < material.requiredCount) {
            return false;
        }
    }
    return true;
}

bool ForgeModel::consumeMaterials(const QVector<ForgeMaterial>& materials)
{
    if (!m_backpackModel) {
        return false;
    }
    
    // 先检查是否有足够材料
    if (!hasSufficientMaterials(materials)) {
        return false;
    }
    
    // 消耗材料
    for (const auto& material : materials) {
        m_backpackModel->removeItem(material.itemId, material.requiredCount);
        
        if (material.isCatalyst) {
            emit catalystUsed(material.itemId, material.requiredCount);
        }
    }
    
    return true;
}

bool ForgeModel::rollForgeSuccess(float successRate) const
{
    float randomValue = QRandomGenerator::global()->generateDouble();
    return randomValue <= successRate;
}

ForgeRecipe ForgeModel::getRecipeById(int recipeId) const
{
    for (const auto& recipe : m_recipes) {
        if (recipe.recipeId == recipeId) {
            return recipe;
        }
    }
    return ForgeRecipe(); // 返回空配方
}

QVector<ForgeRecipe> ForgeModel::getAvailableRecipes() const
{
    QVector<ForgeRecipe> available;
    for (const auto& recipe : m_recipes) {
        if (isRecipeUnlocked(recipe.recipeId)) {
            available.append(recipe);
        }
    }
    return available;
}

bool ForgeModel::isRecipeUnlocked(int recipeId) const
{
    ForgeRecipe recipe = getRecipeById(recipeId);
    if (recipe.recipeId == 0) {
        return false;
    }
    
    // 简化解锁逻辑：基于等级
    // TODO: 可以根据实际需求添加更复杂的解锁条件
    return true; // 暂时全部解锁
}

void ForgeModel::addForgeHistory(const ForgeHistory& history)
{
    m_forgeHistory.append(history);
    
    // 限制历史记录数量
    if (m_forgeHistory.size() > 1000) {
        m_forgeHistory.removeFirst();
    }
}

void ForgeModel::checkAndUnlockRecipes()
{
    // TODO: 实现配方解锁逻辑
    // 可以根据锻造次数、成功率、拥有的物品等条件解锁新配方
}

// 工作系统升级方法
bool ForgeModel::upgradeWorkSystem(WorkType workType, WorkSystemLevel targetLevel)
{
    // 检查是否可以升级
    if (!canUpgradeWorkSystem(workType)) {
        qDebug() << "ForgeModel: Cannot upgrade work system" << static_cast<int>(workType);
        return false;
    }
    
    // 检查目标等级是否有效
    WorkSystemLevel currentLevel = getWorkSystemLevel(workType);
    if (targetLevel <= currentLevel) {
        qDebug() << "ForgeModel: Target level must be higher than current level";
        return false;
    }
    
    // 查找升级配方
    for (const auto& upgrade : m_workUpgrades) {
        if (upgrade.workType == workType && upgrade.targetLevel == targetLevel) {
            // 检查材料是否足够
            if (!hasSufficientMaterials(upgrade.upgradeMaterials)) {
                qDebug() << "ForgeModel: Insufficient materials for work system upgrade";
                return false;
            }
            
            // 消耗材料
            if (!consumeMaterials(upgrade.upgradeMaterials)) {
                qDebug() << "ForgeModel: Failed to consume materials for work system upgrade";
                return false;
            }
            
            // 升级工作系统
            m_workSystemLevels[workType] = targetLevel;
            
            // 更新工作系统效果
            updateWorkSystemBenefits(workType, targetLevel);
            
            // 发送升级完成信号
            emit workSystemUpgraded(workType, targetLevel);
            
            qDebug() << "ForgeModel: Successfully upgraded work system" << static_cast<int>(workType) 
                     << "to level" << static_cast<int>(targetLevel);
            return true;
        }
    }
    
    qDebug() << "ForgeModel: No upgrade recipe found for work system";
    return false;
}

bool ForgeModel::canUpgradeWorkSystem(WorkType workType) const
{
    WorkSystemLevel currentLevel = getWorkSystemLevel(workType);
    
    // 检查是否已经达到最高等级
    if (currentLevel >= WorkSystemLevel::Master) {
        return false;
    }
    
    // 检查是否有可用的升级配方
    WorkSystemLevel nextLevel = static_cast<WorkSystemLevel>(static_cast<int>(currentLevel) + 1);
    
    for (const auto& upgrade : m_workUpgrades) {
        if (upgrade.workType == workType && upgrade.targetLevel == nextLevel) {
            // 检查材料是否足够
            return hasSufficientMaterials(upgrade.upgradeMaterials);
        }
    }
    
    return false;
}

QVector<WorkSystemUpgrade> ForgeModel::getAvailableWorkUpgrades() const
{
    QVector<WorkSystemUpgrade> availableUpgrades;
    
    for (const auto& upgrade : m_workUpgrades) {
        WorkSystemLevel currentLevel = getWorkSystemLevel(upgrade.workType);
        
        // 只返回下一级的升级选项
        if (upgrade.targetLevel == static_cast<WorkSystemLevel>(static_cast<int>(currentLevel) + 1)) {
            availableUpgrades.append(upgrade);
        }
    }
    
    return availableUpgrades;
}

WorkSystemLevel ForgeModel::getWorkSystemLevel(WorkType workType) const
{
    return m_workSystemLevels.value(workType, WorkSystemLevel::Basic);
}

// 设置依赖模型
void ForgeModel::setBackpackModel(std::shared_ptr<BackpackModel> backpackModel)
{
    m_backpackModel = backpackModel;
    // TODO: 等BackpackModel实现itemChanged信号后再连接
    /*
    if (m_backpackModel) {
        connect(m_backpackModel.get(), &BackpackModel::itemChanged, 
                this, &ForgeModel::onBackpackChanged);
    }
    */
}

void ForgeModel::setCollectionModel(std::shared_ptr<CollectionModel> collectionModel)
{
    m_collectionModel = collectionModel;
    if (m_collectionModel) {
        connect(m_collectionModel.get(), &CollectionModel::itemUnlocked,
                this, &ForgeModel::onCollectionChanged);
    }
}

void ForgeModel::setWorkModel(std::shared_ptr<WorkModel> workModel)
{
    m_workModel = workModel;
    // TODO: 等WorkModel实现workCompleted信号后再连接
    /*
    if (m_workModel) {
        connect(m_workModel.get(), &WorkModel::workCompleted,
                this, &ForgeModel::onWorkModelChanged);
    }
    */
}

// 槽函数
void ForgeModel::onBackpackChanged()
{
    // 背包变化时检查配方解锁状态
    checkAndUnlockRecipes();
}

void ForgeModel::onCollectionChanged()
{
    // 图鉴变化时检查配方解锁状态
    checkAndUnlockRecipes();
}

void ForgeModel::onWorkModelChanged()
{
    // 工作模型变化时更新相关状态
}

// 统计方法
int ForgeModel::getTotalForgeCount() const
{
    return m_totalForgeCount;
}

int ForgeModel::getSuccessfulForgeCount() const
{
    return m_successfulForgeCount;
}

float ForgeModel::getForgeSuccessRate() const
{
    if (m_totalForgeCount == 0) {
        return 0.0f;
    }
    return static_cast<float>(m_successfulForgeCount) / m_totalForgeCount;
}

QVector<ForgeHistory> ForgeModel::getForgeHistory() const
{
    return m_forgeHistory;
}

// 数据持久化
void ForgeModel::saveToFile(const QString& filename) const
{
    QJsonObject json = toJson();
    QJsonDocument doc(json);
    
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        qDebug() << "ForgeModel: Saved to" << filename;
    } else {
        qDebug() << "ForgeModel: Failed to save to" << filename;
    }
}

void ForgeModel::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        fromJson(doc.object());
        qDebug() << "ForgeModel: Loaded from" << filename;
    } else {
        qDebug() << "ForgeModel: Failed to load from" << filename;
    }
}

QJsonObject ForgeModel::toJson() const
{
    QJsonObject json;
    json["totalForgeCount"] = m_totalForgeCount;
    json["successfulForgeCount"] = m_successfulForgeCount;
    
    // 保存工作系统等级
    QJsonObject workLevels;
    for (auto it = m_workSystemLevels.begin(); it != m_workSystemLevels.end(); ++it) {
        workLevels[QString::number(static_cast<int>(it.key()))] = static_cast<int>(it.value());
    }
    json["workSystemLevels"] = workLevels;
    
    // 保存锻造历史记录
    QJsonArray historyArray;
    for (const auto& history : m_forgeHistory) {
        QJsonObject historyObj;
        historyObj["recipeId"] = history.recipeId;
        historyObj["forgeTime"] = history.forgeTime.toString(Qt::ISODate);
        historyObj["success"] = history.success;
        
        QJsonArray materialsArray;
        for (const auto& material : history.materialsCost) {
            QJsonObject materialObj;
            materialObj["itemId"] = material.itemId;
            materialObj["count"] = material.requiredCount;
            materialObj["isCatalyst"] = material.isCatalyst;
            materialsArray.append(materialObj);
        }
        historyObj["materialsCost"] = materialsArray;
        
        QJsonArray productsArray;
        for (int itemId : history.productsGained) {
            productsArray.append(itemId);
        }
        historyObj["productsGained"] = productsArray;
        
        historyArray.append(historyObj);
    }
    json["forgeHistory"] = historyArray;
    
    return json;
}

void ForgeModel::fromJson(const QJsonObject& json)
{
    m_totalForgeCount = json["totalForgeCount"].toInt();
    m_successfulForgeCount = json["successfulForgeCount"].toInt();
    
    // 加载工作系统等级
    QJsonObject workLevels = json["workSystemLevels"].toObject();
    for (auto it = workLevels.begin(); it != workLevels.end(); ++it) {
        WorkType workType = static_cast<WorkType>(it.key().toInt());
        WorkSystemLevel level = static_cast<WorkSystemLevel>(it.value().toInt());
        m_workSystemLevels[workType] = level;
    }
    
    // 加载锻造历史记录
    m_forgeHistory.clear();
    QJsonArray historyArray = json["forgeHistory"].toArray();
    for (const auto& historyValue : historyArray) {
        QJsonObject historyObj = historyValue.toObject();
        ForgeHistory history;
        history.recipeId = historyObj["recipeId"].toInt();
        history.forgeTime = QDateTime::fromString(historyObj["forgeTime"].toString(), Qt::ISODate);
        history.success = historyObj["success"].toBool();
        
        QJsonArray materialsArray = historyObj["materialsCost"].toArray();
        for (const auto& materialValue : materialsArray) {
            QJsonObject materialObj = materialValue.toObject();
            ForgeMaterial material;
            material.itemId = materialObj["itemId"].toInt();
            material.requiredCount = materialObj["count"].toInt();
            material.isCatalyst = materialObj["isCatalyst"].toBool();
            history.materialsCost.append(material);
        }
        
        QJsonArray productsArray = historyObj["productsGained"].toArray();
        for (const auto& productValue : productsArray) {
            history.productsGained.append(productValue.toInt());
        }
        
        m_forgeHistory.append(history);
    }
}

void ForgeModel::updateWorkSystemBenefits(WorkType workType, WorkSystemLevel newLevel)
{
    // 更新工作系统的收益效果
    // 这里可以通知WorkModel更新相应的工作系统效果
    if (m_workModel) {
        // 如果WorkModel有相应的方法来更新工作系统效果，可以在这里调用
        // 例如: m_workModel->updateWorkSystemLevel(workType, newLevel);
    }
    
    qDebug() << "ForgeModel: Updated work system benefits for" 
             << static_cast<int>(workType) << "to level" << static_cast<int>(newLevel);
}

bool ForgeModel::forgeItemWithCustomMaterials(int recipeId, const QVector<ForgeMaterial>& customMaterials)
{
    if (!m_backpackModel) {
        qDebug() << "ForgeModel: BackpackModel not available";
        return false;
    }
    
    // 获取配方
    ForgeRecipe recipe = getRecipeById(recipeId);
    if (recipe.recipeId == 0) {
        qDebug() << "ForgeModel: Recipe not found:" << recipeId;
        return false;
    }
    
    // 检查是否有足够的自定义材料
    if (!hasSufficientMaterials(customMaterials)) {
        qDebug() << "ForgeModel: Insufficient custom materials";
        return false;
    }
    
    // 消耗自定义材料
    // for (const auto& material : customMaterials) {
    //     if (!m_backpackModel->removeItem(material.itemId, material.requiredCount)) {
    //         qDebug() << "ForgeModel: Failed to remove custom material:" << material.itemId;
    //         return false;
    //     }
    // }
    
    // 记录锻造历史
    ForgeHistory history;
    history.recipeId = recipeId;
    history.forgeTime = QDateTime::currentDateTime();
    
    // 记录使用的自定义材料
    for (const auto& material : customMaterials) {
        history.materialsCost.append(material);
    }
    
    m_totalForgeCount++;
    
    // 处理每个产出
    bool anySuccess = false;
    for (const auto& output : recipe.outputs) {
        bool success = rollForgeSuccess(output.successRate);
        if (success) {
            // 添加物品到背包
            if (m_backpackModel) {
                m_backpackModel->addItem(output.itemId, output.outputCount);
            }
            
            // 记录产出
            for (int i = 0; i < output.outputCount; ++i) {
                history.productsGained.append(output.itemId);
            }
            anySuccess = true;
        }
    }
    
    if (anySuccess) {
        m_successfulForgeCount++;
    }
    
    history.success = anySuccess;
    addForgeHistory(history);
    
    // 检查是否解锁新配方
    checkAndUnlockRecipes();
    
    // 发射信号
    emit forgeCompleted(recipeId, anySuccess, history.productsGained);
    
    qDebug() << "ForgeModel: Custom forge completed for recipe" << recipeId 
             << "Success:" << anySuccess 
             << "Produced items:" << history.productsGained.size();
    
    return anySuccess;
}
