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
    : QObject(parent), m_totalForgeCount(0), m_successfulForgeCount(0)
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

    // ========== 阳光合成配方 ========= =
    // 阳光合成不需要其他物品，直接升级

    // 配方1: 5个微光阳光 -> 1个温暖阳光
    ForgeRecipe sunlightUpgrade1;
    sunlightUpgrade1.recipeId = 1;
    sunlightUpgrade1.name = "温暖阳光";
    sunlightUpgrade1.description = "5个微光阳光合成1个温暖阳光";
    sunlightUpgrade1.type = ForgeRecipeType::Special;
    sunlightUpgrade1.materials = {
        ForgeMaterial(SUNSHINE_WEAK_ID, 5, false) // 5个微光阳光
    };
    sunlightUpgrade1.outputs = {
        ForgeOutput(SUNSHINE_WARM_ID, 1, 1.0f) // 1个温暖阳光，100%成功率
    };
    sunlightUpgrade1.unlockLevel = 1;
    sunlightUpgrade1.requiresCatalyst = false;
    m_recipes.append(sunlightUpgrade1);

    // 配方2: 5个温暖阳光 -> 1个炽热阳光
    ForgeRecipe sunlightUpgrade2;
    sunlightUpgrade2.recipeId = 2;
    sunlightUpgrade2.name = "炽热阳光";
    sunlightUpgrade2.description = "5个温暖阳光合成1个炽热阳光";
    sunlightUpgrade2.type = ForgeRecipeType::Special;
    sunlightUpgrade2.materials = {
        ForgeMaterial(SUNSHINE_WARM_ID, 5, false) // 5个温暖阳光
    };
    sunlightUpgrade2.outputs = {
        ForgeOutput(SUNSHINE_HOT_ID, 1, 1.0f) // 1个炽热阳光
    };
    sunlightUpgrade2.unlockLevel = 1;
    sunlightUpgrade2.requiresCatalyst = false;
    m_recipes.append(sunlightUpgrade2);

    // 配方3: 5个炽热阳光 -> 1个灿烂阳光
    ForgeRecipe sunlightUpgrade3;
    sunlightUpgrade3.recipeId = 3;
    sunlightUpgrade3.name = "灿烂阳光";
    sunlightUpgrade3.description = "5个炽热阳光合成1个灿烂阳光";
    sunlightUpgrade3.type = ForgeRecipeType::Special;
    sunlightUpgrade3.materials = {
        ForgeMaterial(SUNSHINE_HOT_ID, 5, false) // 5个炽热阳光
    };
    sunlightUpgrade3.outputs = {
        ForgeOutput(SUNSHINE_BRILLIANT_ID, 1, 1.0f) // 1个灿烂阳光
    };
    sunlightUpgrade3.unlockLevel = 1;
    sunlightUpgrade3.requiresCatalyst = false;
    m_recipes.append(sunlightUpgrade3);

    // 配方4: 5个灿烂阳光 -> 1个神圣阳光
    ForgeRecipe sunlightUpgrade4;
    sunlightUpgrade4.recipeId = 4;
    sunlightUpgrade4.name = "神圣阳光";
    sunlightUpgrade4.description = "5个灿烂阳光合成1个神圣阳光";
    sunlightUpgrade4.type = ForgeRecipeType::Special;
    sunlightUpgrade4.materials = {
        ForgeMaterial(SUNSHINE_BRILLIANT_ID, 5, false) // 5个灿烂阳光
    };
    sunlightUpgrade4.outputs = {
        ForgeOutput(SUNSHINE_SACRED_ID, 1, 1.0f) // 1个神圣阳光
    };
    sunlightUpgrade4.unlockLevel = 1;
    sunlightUpgrade4.requiresCatalyst = false;
    m_recipes.append(sunlightUpgrade4);

    // ========== 矿石合成配方 ========= =
    // 5个低品质矿石 + 对应品质阳光 -> 1个高品质矿石

    // 配方5: 5个粗糙矿石 + 1个温暖阳光 -> 1个普通矿石
    ForgeRecipe mineralUpgrade1;
    mineralUpgrade1.recipeId = 5;
    mineralUpgrade1.name = "普通矿石";
    mineralUpgrade1.description = "5个粗糙矿石 + 1个温暖阳光合成1个普通矿石";
    mineralUpgrade1.type = ForgeRecipeType::Special;
    mineralUpgrade1.materials = {
        ForgeMaterial(11, 5, false),             // 5个粗糙矿石
        ForgeMaterial(SUNSHINE_WARM_ID, 1, true) // 1个温暖阳光作为催化剂
    };
    mineralUpgrade1.outputs = {
        ForgeOutput(12, 1, 1.0f) // 1个普通矿石
    };
    mineralUpgrade1.unlockLevel = 1;
    mineralUpgrade1.requiresCatalyst = true;
    m_recipes.append(mineralUpgrade1);

    // 配方6: 5个普通矿石 + 1个炽热阳光 -> 1个优质矿石
    ForgeRecipe mineralUpgrade2;
    mineralUpgrade2.recipeId = 6;
    mineralUpgrade2.name = "优质矿石";
    mineralUpgrade2.description = "5个普通矿石 + 1个炽热阳光合成1个优质矿石";
    mineralUpgrade2.type = ForgeRecipeType::Special;
    mineralUpgrade2.materials = {
        ForgeMaterial(12, 5, false),            // 5个普通矿石
        ForgeMaterial(SUNSHINE_HOT_ID, 1, true) // 1个炽热阳光作为催化剂
    };
    mineralUpgrade2.outputs = {
        ForgeOutput(13, 1, 1.0f) // 1个优质矿石
    };
    mineralUpgrade2.unlockLevel = 1;
    mineralUpgrade2.requiresCatalyst = true;
    m_recipes.append(mineralUpgrade2);

    // 配方7: 5个优质矿石 + 1个灿烂阳光 -> 1个稀有矿石
    ForgeRecipe mineralUpgrade3;
    mineralUpgrade3.recipeId = 7;
    mineralUpgrade3.name = "稀有矿石";
    mineralUpgrade3.description = "5个优质矿石 + 1个灿烂阳光合成1个稀有矿石";
    mineralUpgrade3.type = ForgeRecipeType::Special;
    mineralUpgrade3.materials = {
        ForgeMaterial(13, 5, false),                  // 5个优质矿石
        ForgeMaterial(SUNSHINE_BRILLIANT_ID, 1, true) // 1个灿烂阳光作为催化剂
    };
    mineralUpgrade3.outputs = {
        ForgeOutput(14, 1, 1.0f) // 1个稀有矿石
    };
    mineralUpgrade3.unlockLevel = 1;
    mineralUpgrade3.requiresCatalyst = true;
    m_recipes.append(mineralUpgrade3);

    // 配方8: 5个稀有矿石 + 1个神圣阳光 -> 1个传说矿石
    ForgeRecipe mineralUpgrade4;
    mineralUpgrade4.recipeId = 8;
    mineralUpgrade4.name = "传说矿石";
    mineralUpgrade4.description = "5个稀有矿石 + 1个神圣阳光合成1个传说矿石";
    mineralUpgrade4.type = ForgeRecipeType::Special;
    mineralUpgrade4.materials = {
        ForgeMaterial(14, 5, false),               // 5个稀有矿石
        ForgeMaterial(SUNSHINE_SACRED_ID, 1, true) // 1个神圣阳光作为催化剂
    };
    mineralUpgrade4.outputs = {
        ForgeOutput(15, 1, 1.0f) // 1个传说矿石
    };
    mineralUpgrade4.unlockLevel = 1;
    mineralUpgrade4.requiresCatalyst = true;
    m_recipes.append(mineralUpgrade4);

    // ========== 木材合成配方 ========= =
    // 5个低品质木材 + 对应品质阳光 -> 1个高品质木材

    // 配方9: 5个枯木 + 1个温暖阳光 -> 1个普通木材
    ForgeRecipe woodUpgrade1;
    woodUpgrade1.recipeId = 9;
    woodUpgrade1.name = "普通木材";
    woodUpgrade1.description = "5个枯木 + 1个温暖阳光合成1个普通木材";
    woodUpgrade1.type = ForgeRecipeType::Special;
    woodUpgrade1.materials = {
        ForgeMaterial(16, 5, false),             // 5个枯木
        ForgeMaterial(SUNSHINE_WARM_ID, 1, true) // 1个温暖阳光作为催化剂
    };
    woodUpgrade1.outputs = {
        ForgeOutput(17, 1, 1.0f) // 1个普通木材
    };
    woodUpgrade1.unlockLevel = 1;
    woodUpgrade1.requiresCatalyst = true;
    m_recipes.append(woodUpgrade1);

    // 配方10: 5个普通木材 + 1个炽热阳光 -> 1个优质木材
    ForgeRecipe woodUpgrade2;
    woodUpgrade2.recipeId = 10;
    woodUpgrade2.name = "优质木材";
    woodUpgrade2.description = "5个普通木材 + 1个炽热阳光合成1个优质木材";
    woodUpgrade2.type = ForgeRecipeType::Special;
    woodUpgrade2.materials = {
        ForgeMaterial(17, 5, false),            // 5个普通木材
        ForgeMaterial(SUNSHINE_HOT_ID, 1, true) // 1个炽热阳光作为催化剂
    };
    woodUpgrade2.outputs = {
        ForgeOutput(18, 1, 1.0f) // 1个优质木材
    };
    woodUpgrade2.unlockLevel = 1;
    woodUpgrade2.requiresCatalyst = true;
    m_recipes.append(woodUpgrade2);

    // 配方11: 5个优质木材 + 1个灿烂阳光 -> 1个稀有木材
    ForgeRecipe woodUpgrade3;
    woodUpgrade3.recipeId = 11;
    woodUpgrade3.name = "稀有木材";
    woodUpgrade3.description = "5个优质木材 + 1个灿烂阳光合成1个稀有木材";
    woodUpgrade3.type = ForgeRecipeType::Special;
    woodUpgrade3.materials = {
        ForgeMaterial(18, 5, false),                  // 5个优质木材
        ForgeMaterial(SUNSHINE_BRILLIANT_ID, 1, true) // 1个灿烂阳光作为催化剂
    };
    woodUpgrade3.outputs = {
        ForgeOutput(19, 1, 1.0f) // 1个稀有木材
    };
    woodUpgrade3.unlockLevel = 1;
    woodUpgrade3.requiresCatalyst = true;
    m_recipes.append(woodUpgrade3);

    // 配方12: 5个稀有木材 + 1个神圣阳光 -> 1个神木
    ForgeRecipe woodUpgrade4;
    woodUpgrade4.recipeId = 12;
    woodUpgrade4.name = "神木";
    woodUpgrade4.description = "5个稀有木材 + 1个神圣阳光合成1个神木";
    woodUpgrade4.type = ForgeRecipeType::Special;
    woodUpgrade4.materials = {
        ForgeMaterial(19, 5, false),               // 5个稀有木材
        ForgeMaterial(SUNSHINE_SACRED_ID, 1, true) // 1个神圣阳光作为催化剂
    };
    woodUpgrade4.outputs = {
        ForgeOutput(20, 1, 1.0f) // 1个神木
    };
    woodUpgrade4.unlockLevel = 1;
    woodUpgrade4.requiresCatalyst = true;
    m_recipes.append(woodUpgrade4);

    qDebug() << "ForgeModel: Initialized" << m_recipes.size() << "synthesis recipes";
}

void ForgeModel::initializeWorkUpgrades()
{
    m_workUpgrades.clear();

    // ========== 光合作用工作系统升级 ==========
    // 基础 → 进阶
    WorkSystemUpgrade photoUpgrade1;
    photoUpgrade1.workType = WorkType::Photosynthesis;
    photoUpgrade1.currentLevel = WorkSystemLevel::Basic;
    photoUpgrade1.targetLevel = WorkSystemLevel::Advanced;
    photoUpgrade1.upgradeMaterials = {
        ForgeMaterial(SUNSHINE_WARM_ID, 10, false), // 10个温暖阳光
        ForgeMaterial(SUNSHINE_HOT_ID, 5, false),   // 5个炽热阳光
        ForgeMaterial(16, 5, false)                 // 5个枯木作为辅助材料
    };
    photoUpgrade1.dropRateMultiplier = 1.5f;
    photoUpgrade1.qualityBonus = 0.2f;
    photoUpgrade1.unlockedItems = {201, 202};
    m_workUpgrades.append(photoUpgrade1);

    // 进阶 → 专家
    WorkSystemUpgrade photoUpgrade2;
    photoUpgrade2.workType = WorkType::Photosynthesis;
    photoUpgrade2.currentLevel = WorkSystemLevel::Advanced;
    photoUpgrade2.targetLevel = WorkSystemLevel::Expert;
    photoUpgrade2.upgradeMaterials = {
        ForgeMaterial(SUNSHINE_HOT_ID, 15, false),      // 15个炽热阳光
        ForgeMaterial(SUNSHINE_BRILLIANT_ID, 8, false), // 8个灿烂阳光
        ForgeMaterial(17, 8, false),                    // 8个普通木材
        ForgeMaterial(11, 10, false)                    // 10个粗糙矿石
    };
    photoUpgrade2.dropRateMultiplier = 2.0f;
    photoUpgrade2.qualityBonus = 0.4f;
    photoUpgrade2.unlockedItems = {203, 204, 205};
    m_workUpgrades.append(photoUpgrade2);

    // 专家 → 大师
    WorkSystemUpgrade photoUpgrade3;
    photoUpgrade3.workType = WorkType::Photosynthesis;
    photoUpgrade3.currentLevel = WorkSystemLevel::Expert;
    photoUpgrade3.targetLevel = WorkSystemLevel::Master;
    photoUpgrade3.upgradeMaterials = {
        ForgeMaterial(SUNSHINE_BRILLIANT_ID, 20, false), // 20个灿烂阳光
        ForgeMaterial(SUNSHINE_SACRED_ID, 10, false),    // 10个神圣阳光
        ForgeMaterial(18, 5, false),                     // 5个优质木材
        ForgeMaterial(13, 5, false)                      // 5个优质矿石
    };
    photoUpgrade3.dropRateMultiplier = 3.0f;
    photoUpgrade3.qualityBonus = 0.6f;
    photoUpgrade3.unlockedItems = {206, 207, 208, 209};
    m_workUpgrades.append(photoUpgrade3);

    // ========== 挖矿工作系统升级 ==========
    // 基础 → 进阶
    WorkSystemUpgrade miningUpgrade1;
    miningUpgrade1.workType = WorkType::Mining;
    miningUpgrade1.currentLevel = WorkSystemLevel::Basic;
    miningUpgrade1.targetLevel = WorkSystemLevel::Advanced;
    miningUpgrade1.upgradeMaterials = {
        ForgeMaterial(11, 15, false),             // 15个粗糙矿石
        ForgeMaterial(12, 8, false),              // 8个普通矿石
        ForgeMaterial(SUNSHINE_WARM_ID, 5, false) // 5个温暖阳光
    };
    miningUpgrade1.dropRateMultiplier = 1.5f;
    miningUpgrade1.qualityBonus = 0.2f;
    miningUpgrade1.unlockedItems = {211, 212};
    m_workUpgrades.append(miningUpgrade1);

    // 进阶 → 专家
    WorkSystemUpgrade miningUpgrade2;
    miningUpgrade2.workType = WorkType::Mining;
    miningUpgrade2.currentLevel = WorkSystemLevel::Advanced;
    miningUpgrade2.targetLevel = WorkSystemLevel::Expert;
    miningUpgrade2.upgradeMaterials = {
        ForgeMaterial(12, 20, false),             // 20个普通矿石
        ForgeMaterial(13, 12, false),             // 12个优质矿石
        ForgeMaterial(SUNSHINE_HOT_ID, 8, false), // 8个炽热阳光
        ForgeMaterial(17, 10, false)              // 10个普通木材
    };
    miningUpgrade2.dropRateMultiplier = 2.0f;
    miningUpgrade2.qualityBonus = 0.4f;
    miningUpgrade2.unlockedItems = {213, 214, 215};
    m_workUpgrades.append(miningUpgrade2);

    // 专家 → 大师
    WorkSystemUpgrade miningUpgrade3;
    miningUpgrade3.workType = WorkType::Mining;
    miningUpgrade3.currentLevel = WorkSystemLevel::Expert;
    miningUpgrade3.targetLevel = WorkSystemLevel::Master;
    miningUpgrade3.upgradeMaterials = {
        ForgeMaterial(13, 25, false),                   // 25个优质矿石
        ForgeMaterial(14, 15, false),                   // 15个稀有矿石
        ForgeMaterial(15, 3, false),                    // 3个传说矿石
        ForgeMaterial(SUNSHINE_BRILLIANT_ID, 10, false) // 10个灿烂阳光
    };
    miningUpgrade3.dropRateMultiplier = 3.0f;
    miningUpgrade3.qualityBonus = 0.6f;
    miningUpgrade3.unlockedItems = {216, 217, 218, 219};
    m_workUpgrades.append(miningUpgrade3);

    // ========== 冒险工作系统升级 ==========
    // 基础 → 进阶
    WorkSystemUpgrade adventureUpgrade1;
    adventureUpgrade1.workType = WorkType::Adventure;
    adventureUpgrade1.currentLevel = WorkSystemLevel::Basic;
    adventureUpgrade1.targetLevel = WorkSystemLevel::Advanced;
    adventureUpgrade1.upgradeMaterials = {
        ForgeMaterial(16, 15, false), // 15个枯木
        ForgeMaterial(17, 8, false),  // 8个普通木材
        ForgeMaterial(11, 5, false)   // 5个粗糙矿石
    };
    adventureUpgrade1.dropRateMultiplier = 1.5f;
    adventureUpgrade1.qualityBonus = 0.2f;
    adventureUpgrade1.unlockedItems = {221, 222};
    m_workUpgrades.append(adventureUpgrade1);

    // 进阶 → 专家
    WorkSystemUpgrade adventureUpgrade2;
    adventureUpgrade2.workType = WorkType::Adventure;
    adventureUpgrade2.currentLevel = WorkSystemLevel::Advanced;
    adventureUpgrade2.targetLevel = WorkSystemLevel::Expert;
    adventureUpgrade2.upgradeMaterials = {
        ForgeMaterial(17, 20, false),            // 20个普通木材
        ForgeMaterial(18, 12, false),            // 12个优质木材
        ForgeMaterial(12, 8, false),             // 8个普通矿石
        ForgeMaterial(SUNSHINE_HOT_ID, 5, false) // 5个炽热阳光
    };
    adventureUpgrade2.dropRateMultiplier = 2.0f;
    adventureUpgrade2.qualityBonus = 0.4f;
    adventureUpgrade2.unlockedItems = {223, 224, 225};
    m_workUpgrades.append(adventureUpgrade2);

    // 专家 → 大师
    WorkSystemUpgrade adventureUpgrade3;
    adventureUpgrade3.workType = WorkType::Adventure;
    adventureUpgrade3.currentLevel = WorkSystemLevel::Expert;
    adventureUpgrade3.targetLevel = WorkSystemLevel::Master;
    adventureUpgrade3.upgradeMaterials = {
        ForgeMaterial(18, 25, false),               // 25个优质木材
        ForgeMaterial(19, 15, false),               // 15个稀有木材
        ForgeMaterial(20, 3, false),                // 3个神木
        ForgeMaterial(SUNSHINE_SACRED_ID, 5, false) // 5个神圣阳光
    };
    adventureUpgrade3.dropRateMultiplier = 3.0f;
    adventureUpgrade3.qualityBonus = 0.6f;
    adventureUpgrade3.unlockedItems = {226, 227, 228, 229};
    m_workUpgrades.append(adventureUpgrade3);

    qDebug() << "ForgeModel: Initialized" << m_workUpgrades.size() << "work upgrades for all work types";
}

bool ForgeModel::canForge(int recipeId) const
{
    qDebug() << "ForgeModel::canForge: 检查配方" << recipeId << "是否可以锻造";
    
    ForgeRecipe recipe = getRecipeById(recipeId);
    if (recipe.recipeId == 0)
    {
        qDebug() << "ForgeModel::canForge: 配方不存在";
        return false; // 配方不存在
    }

    if (!isRecipeUnlocked(recipeId))
    {
        qDebug() << "ForgeModel::canForge: 配方未解锁";
        return false; // 配方未解锁
    }

    bool sufficient = hasSufficientMaterials(recipe.materials);
    qDebug() << "ForgeModel::canForge: 材料是否足够:" << sufficient;
    
    return sufficient;
}

bool ForgeModel::forgeItem(int recipeId)
{
    qDebug() << "ForgeModel::forgeItem: 开始锻造，配方ID:" << recipeId;
    
    ForgeRecipe recipe = getRecipeById(recipeId);
    if (!canForge(recipeId))
    {
        qDebug() << "ForgeModel: Cannot forge recipe" << recipeId;
        return false;
    }

    qDebug() << "ForgeModel::forgeItem: 配方名称:" << recipe.name;
    qDebug() << "ForgeModel::forgeItem: 需要材料数量:" << recipe.materials.size();
    for (const auto &material : recipe.materials)
    {
        qDebug() << "ForgeModel::forgeItem: 材料ID:" << material.itemId 
                 << "需要数量:" << material.requiredCount 
                 << "是否为催化剂:" << material.isCatalyst;
        int availableCount = m_backpackModel ? m_backpackModel->getItemCount(material.itemId) : 0;
        qDebug() << "ForgeModel::forgeItem: 背包中当前数量:" << availableCount;
    }

    // 消耗材料
    if (!consumeMaterials(recipe.materials))
    {
        qDebug() << "ForgeModel: Failed to consume materials for recipe" << recipeId;
        return false;
    }
    
    qDebug() << "ForgeModel::forgeItem: 材料消耗成功";

    // 记录锻造历史
    ForgeHistory history;
    history.recipeId = recipeId;
    history.forgeTime = QDateTime::currentDateTime();

    // 记录使用的材料
    for (const auto &material : recipe.materials)
    {
        history.materialsCost.append(material);
    }

    m_totalForgeCount++;

    // 处理每个产出
    bool anySuccess = false;
    qDebug() << "ForgeModel::forgeItem: 开始处理产出，产出数量:" << recipe.outputs.size();
    for (const auto &output : recipe.outputs)
    {
        qDebug() << "ForgeModel::forgeItem: 产出物品ID:" << output.itemId 
                 << "产出数量:" << output.outputCount 
                 << "成功率:" << output.successRate;
        
        bool success = rollForgeSuccess(output.successRate);
        qDebug() << "ForgeModel::forgeItem: 产出判定结果:" << success;
        
        if (success)
        {
            // 添加物品到背包
            if (m_backpackModel)
            {
                qDebug() << "ForgeModel::forgeItem: 添加物品到背包，ID:" << output.itemId << "数量:" << output.outputCount;
                m_backpackModel->addItem(output.itemId, output.outputCount);
            }
            else
            {
                qDebug() << "ForgeModel::forgeItem: 警告：背包模型为空，无法添加物品";
            }

            // 记录产出
            for (int i = 0; i < output.outputCount; ++i)
            {
                history.productsGained.append(output.itemId);
            }
            anySuccess = true;
        }
    }

    if (anySuccess)
    {
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

bool ForgeModel::hasSufficientMaterials(const QVector<ForgeMaterial> &materials) const
{
    qDebug() << "ForgeModel::hasSufficientMaterials: 检查材料是否足够";
    
    if (!m_backpackModel)
    {
        qDebug() << "ForgeModel::hasSufficientMaterials: 背包模型为空";
        return false;
    }

    for (const auto &material : materials)
    {
        int availableCount = m_backpackModel->getItemCount(material.itemId);
        qDebug() << "ForgeModel::hasSufficientMaterials: 物品ID:" << material.itemId 
                 << "需要:" << material.requiredCount 
                 << "拥有:" << availableCount;
        
        if (availableCount < material.requiredCount)
        {
            qDebug() << "ForgeModel::hasSufficientMaterials: 物品ID" << material.itemId << "数量不足";
            return false;
        }
    }
    
    qDebug() << "ForgeModel::hasSufficientMaterials: 材料足够";
    return true;
}

bool ForgeModel::consumeMaterials(const QVector<ForgeMaterial> &materials)
{
    qDebug() << "ForgeModel::consumeMaterials: 开始消耗材料";
    
    if (!m_backpackModel)
    {
        qDebug() << "ForgeModel::consumeMaterials: 背包模型为空";
        return false;
    }

    // 先检查是否有足够材料
    if (!hasSufficientMaterials(materials))
    {
        qDebug() << "ForgeModel::consumeMaterials: 材料不足";
        return false;
    }

    // 消耗材料
    for (const auto &material : materials)
    {
        qDebug() << "ForgeModel::consumeMaterials: 消耗材料ID:" << material.itemId 
                 << "数量:" << material.requiredCount;
        
        m_backpackModel->removeItem(material.itemId, material.requiredCount);

        if (material.isCatalyst)
        {
            emit catalystUsed(material.itemId, material.requiredCount);
        }
    }

    qDebug() << "ForgeModel::consumeMaterials: 材料消耗完成";
    return true;
}

bool ForgeModel::rollForgeSuccess(float successRate) const
{
    float randomValue = QRandomGenerator::global()->generateDouble();
    return randomValue <= successRate;
}

ForgeRecipe ForgeModel::getRecipeById(int recipeId) const
{
    for (const auto &recipe : m_recipes)
    {
        if (recipe.recipeId == recipeId)
        {
            return recipe;
        }
    }
    return ForgeRecipe(); // 返回空配方
}

QVector<ForgeRecipe> ForgeModel::getAvailableRecipes() const
{
    QVector<ForgeRecipe> available;
    for (const auto &recipe : m_recipes)
    {
        if (isRecipeUnlocked(recipe.recipeId))
        {
            available.append(recipe);
        }
    }
    return available;
}

bool ForgeModel::isRecipeUnlocked(int recipeId) const
{
    ForgeRecipe recipe = getRecipeById(recipeId);
    if (recipe.recipeId == 0)
    {
        return false;
    }

    // 简化解锁逻辑：基于等级
    // TODO: 可以根据实际需求添加更复杂的解锁条件
    return true; // 暂时全部解锁
}

void ForgeModel::addForgeHistory(const ForgeHistory &history)
{
    m_forgeHistory.append(history);

    // 限制历史记录数量
    if (m_forgeHistory.size() > 1000)
    {
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
    if (!canUpgradeWorkSystem(workType))
    {
        qDebug() << "ForgeModel: Cannot upgrade work system" << static_cast<int>(workType);
        return false;
    }

    // 检查目标等级是否有效
    WorkSystemLevel currentLevel = getWorkSystemLevel(workType);
    if (targetLevel <= currentLevel)
    {
        qDebug() << "ForgeModel: Target level must be higher than current level";
        return false;
    }

    // 查找升级配方
    for (const auto &upgrade : m_workUpgrades)
    {
        if (upgrade.workType == workType && upgrade.targetLevel == targetLevel)
        {
            // 检查材料是否足够
            if (!hasSufficientMaterials(upgrade.upgradeMaterials))
            {
                qDebug() << "ForgeModel: Insufficient materials for work system upgrade";
                return false;
            }

            // 消耗材料
            if (!consumeMaterials(upgrade.upgradeMaterials))
            {
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
    if (currentLevel >= WorkSystemLevel::Master)
    {
        return false;
    }

    // 检查是否有可用的升级配方
    WorkSystemLevel nextLevel = static_cast<WorkSystemLevel>(static_cast<int>(currentLevel) + 1);

    for (const auto &upgrade : m_workUpgrades)
    {
        if (upgrade.workType == workType && upgrade.targetLevel == nextLevel)
        {
            // 检查材料是否足够
            return hasSufficientMaterials(upgrade.upgradeMaterials);
        }
    }

    return false;
}

QVector<WorkSystemUpgrade> ForgeModel::getAvailableWorkUpgrades() const
{
    QVector<WorkSystemUpgrade> availableUpgrades;

    for (const auto &upgrade : m_workUpgrades)
    {
        WorkSystemLevel currentLevel = getWorkSystemLevel(upgrade.workType);

        // 只返回下一级的升级选项
        if (upgrade.targetLevel == static_cast<WorkSystemLevel>(static_cast<int>(currentLevel) + 1))
        {
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
    if (m_backpackModel) {
        connect(m_backpackModel.get(), &BackpackModel::itemChanged,
                this, &ForgeModel::onBackpackChanged);
        connect(m_backpackModel.get(), &BackpackModel::backpackUpdated,
                this, &ForgeModel::onBackpackChanged);
        qDebug() << "ForgeModel: 已连接BackpackModel信号";
    }
}

void ForgeModel::setCollectionModel(std::shared_ptr<CollectionModel> collectionModel)
{
    m_collectionModel = collectionModel;
    if (m_collectionModel)
    {
        connect(m_collectionModel.get(), &CollectionModel::itemUnlocked,
                this, &ForgeModel::onCollectionChanged);
        connect(m_collectionModel.get(), &CollectionModel::collectionUpdated,
                this, &ForgeModel::onCollectionChanged);
        qDebug() << "ForgeModel: 已连接CollectionModel信号";
    }
}

void ForgeModel::setWorkModel(std::shared_ptr<WorkModel> workModel)
{
    m_workModel = workModel;
    if (m_workModel) {
        connect(m_workModel.get(), &WorkModel::workCompleted,
                this, &ForgeModel::onWorkModelChanged);
        connect(m_workModel.get(), &WorkModel::workSystemLevelChanged,
                this, &ForgeModel::onWorkModelChanged);
        qDebug() << "ForgeModel: 已连接WorkModel信号";
    }
}

// 槽函数
void ForgeModel::onBackpackChanged()
{
    qDebug() << "ForgeModel: 背包变化，检查配方解锁状态";
    // 背包变化时检查配方解锁状态
    checkAndUnlockRecipes();
    
    // 发射通知，让UI更新锻造面板数据
    emit forgeDataChanged();
}

void ForgeModel::onCollectionChanged()
{
    qDebug() << "ForgeModel: 图鉴变化，检查配方解锁状态";
    // 图鉴变化时检查配方解锁状态
    checkAndUnlockRecipes();
    
    // 发射通知
    emit forgeDataChanged();
}

void ForgeModel::onWorkModelChanged()
{
    qDebug() << "ForgeModel: 工作模型变化，更新相关状态";
    // 工作模型变化时更新相关状态
    // 可以检查是否有新的工作系统等级解锁了新的配方
    checkAndUnlockRecipes();
    
    // 发射通知
    emit forgeDataChanged();
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
    if (m_totalForgeCount == 0)
    {
        return 0.0f;
    }
    return static_cast<float>(m_successfulForgeCount) / m_totalForgeCount;
}

QVector<ForgeHistory> ForgeModel::getForgeHistory() const
{
    return m_forgeHistory;
}

// 数据持久化
void ForgeModel::saveToFile(const QString &filename) const
{
    QJsonObject json = toJson();
    QJsonDocument doc(json);

    QFile file(filename);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(doc.toJson());
        qDebug() << "ForgeModel: Saved to" << filename;
    }
    else
    {
        qDebug() << "ForgeModel: Failed to save to" << filename;
    }
}

void ForgeModel::loadFromFile(const QString &filename)
{
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray data = file.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        fromJson(doc.object());
        qDebug() << "ForgeModel: Loaded from" << filename;
    }
    else
    {
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
    for (auto it = m_workSystemLevels.begin(); it != m_workSystemLevels.end(); ++it)
    {
        workLevels[QString::number(static_cast<int>(it.key()))] = static_cast<int>(it.value());
    }
    json["workSystemLevels"] = workLevels;

    // 保存锻造历史记录
    QJsonArray historyArray;
    for (const auto &history : m_forgeHistory)
    {
        QJsonObject historyObj;
        historyObj["recipeId"] = history.recipeId;
        historyObj["forgeTime"] = history.forgeTime.toString(Qt::ISODate);
        historyObj["success"] = history.success;

        QJsonArray materialsArray;
        for (const auto &material : history.materialsCost)
        {
            QJsonObject materialObj;
            materialObj["itemId"] = material.itemId;
            materialObj["count"] = material.requiredCount;
            materialObj["isCatalyst"] = material.isCatalyst;
            materialsArray.append(materialObj);
        }
        historyObj["materialsCost"] = materialsArray;

        QJsonArray productsArray;
        for (int itemId : history.productsGained)
        {
            productsArray.append(itemId);
        }
        historyObj["productsGained"] = productsArray;

        historyArray.append(historyObj);
    }
    json["forgeHistory"] = historyArray;

    return json;
}

void ForgeModel::fromJson(const QJsonObject &json)
{
    m_totalForgeCount = json["totalForgeCount"].toInt();
    m_successfulForgeCount = json["successfulForgeCount"].toInt();

    // 加载工作系统等级
    QJsonObject workLevels = json["workSystemLevels"].toObject();
    for (auto it = workLevels.begin(); it != workLevels.end(); ++it)
    {
        WorkType workType = static_cast<WorkType>(it.key().toInt());
        WorkSystemLevel level = static_cast<WorkSystemLevel>(it.value().toInt());
        m_workSystemLevels[workType] = level;
    }

    // 加载锻造历史记录
    m_forgeHistory.clear();
    QJsonArray historyArray = json["forgeHistory"].toArray();
    for (const auto &historyValue : historyArray)
    {
        QJsonObject historyObj = historyValue.toObject();
        ForgeHistory history;
        history.recipeId = historyObj["recipeId"].toInt();
        history.forgeTime = QDateTime::fromString(historyObj["forgeTime"].toString(), Qt::ISODate);
        history.success = historyObj["success"].toBool();

        QJsonArray materialsArray = historyObj["materialsCost"].toArray();
        for (const auto &materialValue : materialsArray)
        {
            QJsonObject materialObj = materialValue.toObject();
            ForgeMaterial material;
            material.itemId = materialObj["itemId"].toInt();
            material.requiredCount = materialObj["count"].toInt();
            material.isCatalyst = materialObj["isCatalyst"].toBool();
            history.materialsCost.append(material);
        }

        QJsonArray productsArray = historyObj["productsGained"].toArray();
        for (const auto &productValue : productsArray)
        {
            history.productsGained.append(productValue.toInt());
        }

        m_forgeHistory.append(history);
    }
}

void ForgeModel::updateWorkSystemBenefits(WorkType workType, WorkSystemLevel newLevel)
{
    // 更新工作系统的收益效果
    // 这里可以通知WorkModel更新相应的工作系统效果
    if (m_workModel)
    {
        // 通知WorkModel更新工作系统等级
        m_workModel->setWorkSystemLevel(workType, newLevel);
        qDebug() << "ForgeModel: 已通知WorkModel更新工作系统等级";
    }

    qDebug() << "ForgeModel: Updated work system benefits for"
             << static_cast<int>(workType) << "to level" << static_cast<int>(newLevel);
}

bool ForgeModel::forgeItemWithCustomMaterials(int recipeId, const QVector<ForgeMaterial> &customMaterials)
{
    if (!m_backpackModel)
    {
        qDebug() << "ForgeModel: BackpackModel not available";
        return false;
    }

    // 获取配方
    ForgeRecipe recipe = getRecipeById(recipeId);
    if (recipe.recipeId == 0)
    {
        qDebug() << "ForgeModel: Recipe not found:" << recipeId;
        return false;
    }

    // 检查是否有足够的自定义材料
    if (!hasSufficientMaterials(customMaterials))
    {
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
    for (const auto &material : customMaterials)
    {
        history.materialsCost.append(material);
    }

    m_totalForgeCount++;

    // 处理每个产出
    bool anySuccess = false;
    for (const auto &output : recipe.outputs)
    {
        bool success = rollForgeSuccess(output.successRate);
        if (success)
        {
            // 添加物品到背包
            if (m_backpackModel)
            {
                m_backpackModel->addItem(output.itemId, output.outputCount);
            }

            // 记录产出
            for (int i = 0; i < output.outputCount; ++i)
            {
                history.productsGained.append(output.itemId);
            }
            anySuccess = true;
        }
    }

    if (anySuccess)
    {
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
