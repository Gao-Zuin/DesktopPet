#ifndef BACKPACKMODEL_H
#define BACKPACKMODEL_H

#include "../common/PropertyTrigger.h"
#include "../common/PropertyIds.h"
#include "base/BackpackItemInfo.h"
#include "base/CollectionInfo.h"
#include <QMap>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include "../common/EventMgr.h"
#include "../common/EventDefine.h"

class BackpackModel
{
public:
    BackpackModel() noexcept;
    BackpackModel(const BackpackModel &) = delete;
    ~BackpackModel() noexcept;

    BackpackModel &operator=(const BackpackModel &) = delete;

    // 获取背包物品列表
    const QVector<BackpackItemInfo>& getItems() const noexcept {
        return m_items;
    }

    PropertyTrigger &get_trigger() noexcept {
        return m_trigger;
    }

    // 物品操作方法
    void addItem(int itemId, int count = 1) noexcept;
    void removeItem(int itemId, int count = 1) noexcept;
    void setItemCount(int itemId, int newCount) noexcept;
    int getItemCount(int itemId) const noexcept;
    bool hasItem(int itemId) const noexcept;
    void clear() noexcept;

    // 基于图鉴系统的物品信息获取
    bool getItemInfo(int itemId, CollectionItemInfo& outInfo) const noexcept;
    QString getItemName(int itemId) const noexcept;
    QString getItemDescription(int itemId) const noexcept;
    QString getItemIcon(int itemId) const noexcept;
    CollectionCategory getItemCategory(int itemId) const noexcept;
    CollectionRarity getItemRarity(int itemId) const noexcept;

    // 初始化方法
    void initializeFromCollection() noexcept;

    // 持久化方法
    void saveToFile(const QString &filename) const;
    void loadFromFile(const QString &filename);

private:
private:
    // 查找物品索引
    int findItemIndex(int itemId) const noexcept;
    
    // 触发背包更新通知
    void fireBackpackUpdate() {
        m_trigger.fire(PROP_ID_BACKPACK_UPDATE);
    }

private:
    QVector<BackpackItemInfo> m_items;  // 背包物品列表
    PropertyTrigger m_trigger;           // 属性触发器
};

#endif // BACKPACKMODEL_H