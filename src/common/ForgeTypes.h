#ifndef __FORGE_TYPES_H__
#define __FORGE_TYPES_H__

#include "Types.h"
#include <QVector>
#include <QString>
#include <QMap>
#include <QDateTime>

// 锻造材料信息
struct ForgeMaterial {
    int itemId;         // 物品ID
    int requiredCount;  // 需要数量
    bool isCatalyst;    // 是否为催化剂
    
    ForgeMaterial() : itemId(0), requiredCount(0), isCatalyst(false) {}
    ForgeMaterial(int id, int count, bool catalyst = false) 
        : itemId(id), requiredCount(count), isCatalyst(catalyst) {}
};

// 锻造产出信息
struct ForgeOutput {
    int itemId;         // 产出物品ID
    int outputCount;    // 产出数量
    float successRate;  // 成功率 (0.0-1.0)
    
    ForgeOutput() : itemId(0), outputCount(0), successRate(1.0f) {}
    ForgeOutput(int id, int count, float rate = 1.0f) 
        : itemId(id), outputCount(count), successRate(rate) {}
};

// 锻造配方
struct ForgeRecipe {
    int recipeId;                           // 配方ID
    QString name;                           // 配方名称
    QString description;                    // 配方描述
    ForgeRecipeType type;                   // 配方类型
    QVector<ForgeMaterial> materials;       // 所需材料
    QVector<ForgeOutput> outputs;           // 产出物品
    int unlockLevel;                        // 解锁等级要求
    bool requiresCatalyst;                  // 是否需要催化剂
    
    ForgeRecipe() : recipeId(0), type(ForgeRecipeType::ItemUpgrade), 
                   unlockLevel(1), requiresCatalyst(false) {}
};

// 工作系统升级信息
struct WorkSystemUpgrade {
    WorkType workType;                      // 工作类型
    WorkSystemLevel currentLevel;           // 当前等级
    WorkSystemLevel targetLevel;            // 目标等级
    QVector<ForgeMaterial> upgradeMaterials; // 升级所需材料
    
    // 升级后的收益改变
    float dropRateMultiplier;               // 掉落率倍数
    float qualityBonus;                     // 品质加成
    QVector<int> unlockedItems;             // 解锁的新物品
    
    WorkSystemUpgrade() : workType(WorkType::Photosynthesis), 
                         currentLevel(WorkSystemLevel::Basic),
                         targetLevel(WorkSystemLevel::Advanced),
                         dropRateMultiplier(1.0f), qualityBonus(0.0f) {}
};

// 锻造历史记录
struct ForgeHistory {
    int recipeId;
    QDateTime forgeTime;
    bool success;
    QVector<ForgeMaterial> materialsCost;   // 消耗的材料
    QVector<int> productsGained;            // 获得的产品
    
    ForgeHistory() : recipeId(0), success(false) {}
};

#endif // __FORGE_TYPES_H__
