#ifndef COLLECTION_MODEL_H
#define COLLECTION_MODEL_H

#include <QObject>
#include <QMap>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include "../common/base/CollectionInfo.h"
#include "../common/PropertyTrigger.h"

class CollectionModel : public QObject
{
    Q_OBJECT

public:
    explicit CollectionModel(QObject *parent = nullptr);
    
    // 文件操作
    void loadFromFile(const QString &filename = "collection_data.json");
    void saveToFile(const QString &filename = "collection_data.json") const;
    void loadItemsFromCSV(const QString &csvPath);
    
    // 解锁和收集
    bool unlockItem(int itemId);
    bool collectItem(int itemId, int count = 1);
    bool isItemUnlocked(int itemId) const;
    bool isItemCollected(int itemId) const;
    
    // 查询
    CollectionItemInfo getItemInfo(int itemId) const;
    QVector<CollectionItemInfo> getItemsByCategory(CollectionCategory category) const;
    QVector<CollectionItemInfo> getItemsByRarity(CollectionRarity rarity) const;
    QVector<CollectionItemInfo> getItemsByStatus(CollectionStatus status) const;
    QVector<CollectionItemInfo> getAllItems() const;
    
    // 搜索
    QVector<CollectionItemInfo> searchItems(const QString &keyword) const;
    
    // 统计
    int getTotalItems() const;
    int getCollectedItems() const;
    int getDiscoveredItems() const;
    int getItemsByCategory(CollectionCategory category, CollectionStatus status) const;
    float getCompletionRate() const;
    
    // 调试
    void printStatistics() const;
    
    // 获取属性触发器
    PropertyTrigger& get_trigger() noexcept {
        return m_trigger;
    }

signals:
    void itemUnlocked(int itemId);
    void itemCollected(int itemId, int count);
    void collectionUpdated();

private:
    QMap<int, CollectionItemInfo> m_items;
    PropertyTrigger m_trigger;
    
    void fireCollectionUpdate();
    void ensureItemExists(int itemId);
};

#endif // COLLECTION_MODEL_H
