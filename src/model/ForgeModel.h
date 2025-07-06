#ifndef __FORGE_MODEL_H__
#define __FORGE_MODEL_H__

#include "../common/PropertyTrigger.h"
#include "../common/ForgeTypes.h"
#include "../common/Types.h"
#include <QObject>
#include <QVector>
#include <QMap>
#include <QJsonObject>
#include <QJsonArray>
#include <memory>

class BackpackModel;
class CollectionModel;
class WorkModel;

class ForgeModel : public QObject
{
    Q_OBJECT

public:
    explicit ForgeModel(QObject *parent = nullptr);
    ~ForgeModel();

    // 初始化和配置
    void initialize();
    void loadRecipesFromFile(const QString& filePath);
    void loadRecipesFromCSV(const QString& csvPath);
    
    // 设置依赖模型
    void setBackpackModel(std::shared_ptr<BackpackModel> backpackModel);
    void setCollectionModel(std::shared_ptr<CollectionModel> collectionModel);
    void setWorkModel(std::shared_ptr<WorkModel> workModel);

    // 配方管理
    QVector<ForgeRecipe> getAvailableRecipes() const;
    QVector<ForgeRecipe> getRecipesByType(ForgeRecipeType type) const;
    ForgeRecipe getRecipeById(int recipeId) const;
    bool isRecipeUnlocked(int recipeId) const;

    // 锻造操作
    bool canForge(int recipeId) const;
    bool forgeItem(int recipeId);
    bool forgeItemWithCustomMaterials(int recipeId, const QVector<ForgeMaterial>& customMaterials);
    bool upgradeWorkSystem(WorkType workType, WorkSystemLevel targetLevel);
    
    // 材料检查
    bool hasSufficientMaterials(const QVector<ForgeMaterial>& materials) const;
    QVector<ForgeMaterial> getMissingMaterials(const QVector<ForgeMaterial>& required) const;
    
    // 催化剂系统
    bool hasCatalyst(int catalystId, int requiredCount) const;
    QVector<int> getAvailableCatalysts() const;
    
    // 工作系统升级
    WorkSystemLevel getWorkSystemLevel(WorkType workType) const;
    QVector<WorkSystemUpgrade> getAvailableWorkUpgrades() const;
    bool canUpgradeWorkSystem(WorkType workType) const;
    
    // 统计和历史
    QVector<ForgeHistory> getForgeHistory() const;
    int getTotalForgeCount() const;
    int getSuccessfulForgeCount() const;
    float getForgeSuccessRate() const;

    // 数据持久化
    void saveToFile(const QString& filename) const;
    void loadFromFile(const QString& filename);
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);

    // 属性触发器
    PropertyTrigger& get_trigger() { return m_trigger; }

signals:
    void forgeCompleted(int recipeId, bool success, const QVector<int>& producedItems);
    void workSystemUpgraded(WorkType workType, WorkSystemLevel newLevel);
    void recipeUnlocked(int recipeId);
    void catalystUsed(int catalystId, int count);

private slots:
    void onBackpackChanged();
    void onCollectionChanged();
    void onWorkModelChanged();

private:
    // 内部方法
    void initializeDefaultRecipes();
    void initializeWorkUpgrades();
    bool consumeMaterials(const QVector<ForgeMaterial>& materials);
    void produceItems(const QVector<ForgeOutput>& outputs);
    void updateWorkSystemBenefits(WorkType workType, WorkSystemLevel newLevel);
    bool rollForgeSuccess(float successRate) const;
    void addForgeHistory(const ForgeHistory& history);
    void checkAndUnlockRecipes();

private:
    // 核心数据
    QVector<ForgeRecipe> m_recipes;
    QMap<WorkType, WorkSystemLevel> m_workSystemLevels;
    QVector<WorkSystemUpgrade> m_workUpgrades;
    QVector<ForgeHistory> m_forgeHistory;
    
    // 依赖模型
    std::shared_ptr<BackpackModel> m_backpackModel;
    std::shared_ptr<CollectionModel> m_collectionModel;
    std::shared_ptr<WorkModel> m_workModel;
    
    // 统计数据
    int m_totalForgeCount;
    int m_successfulForgeCount;
    
    // 属性触发器
    PropertyTrigger m_trigger;
    
    // 常量定义
    static constexpr int SUNSHINE_WEAK_ID = 6;        // 微光阳光ID
    static constexpr int SUNSHINE_WARM_ID = 7;        // 温暖阳光ID
    static constexpr int SUNSHINE_HOT_ID = 8;         // 炽热阳光ID
    static constexpr int SUNSHINE_BRILLIANT_ID = 9;   // 灿烂阳光ID
    static constexpr int SUNSHINE_SACRED_ID = 10;     // 神圣阳光ID
};

#endif // __FORGE_MODEL_H__
