#include "BackpackModel.h"
#include "../common/PropertyIds.h"
#include "../common/CollectionManager.h"
#include <QDebug>

BackpackModel::BackpackModel() noexcept
{
    qDebug() << "BackpackModel 初始化...";
    // 不再添加测试物品，而是等待图鉴系统初始化完成后再初始化
    qDebug() << "BackpackModel 初始化完成，等待图鉴系统数据";
}

BackpackModel::~BackpackModel() noexcept
{
}

int BackpackModel::findItemIndex(int itemId) const noexcept
{
    for (int i = 0; i < m_items.size(); ++i) {
        if (m_items[i].itemId == itemId) {
            return i;
        }
    }
    return -1;
}

void BackpackModel::addItem(int itemId, int count) noexcept
{
    if (count <= 0) return;
    
    // 检查物品是否在图鉴系统中存在
    CollectionItemInfo itemInfo;
    if (!getItemInfo(itemId, itemInfo)) {
        qWarning() << "尝试添加不存在的物品:" << itemId;
        return;
    }
    
    int index = findItemIndex(itemId);
    if (index != -1) {
        // 物品已存在，增加数量
        m_items[index].count += count;
    } else {
        // 新物品，添加到背包
        m_items.append(BackpackItemInfo(itemId, count));
    }
    
    // 自动解锁和收集图鉴物品
    CollectionManager& collectionMgr = CollectionManager::getInstance();
    collectionMgr.unlockItem(itemId);
    collectionMgr.collectItem(itemId, count);
    
    qDebug() << "添加物品到背包:" << itemInfo.name << "(" << itemId << ") 数量:" << count << "并自动解锁图鉴";
    
    fireBackpackUpdate();
}

void BackpackModel::removeItem(int itemId, int count) noexcept
{
    if (count <= 0) return;
    
    int index = findItemIndex(itemId);
    if (index == -1) return; // 物品不存在
    
    if (m_items[index].count <= count) {
        // 移除全部数量
        m_items.remove(index);
    } else {
        // 减少数量
        m_items[index].count -= count;
    }
    
    fireBackpackUpdate();
}

void BackpackModel::setItemCount(int itemId, int newCount) noexcept
{
    if (newCount <= 0) {
        removeItem(itemId, INT_MAX); // 移除物品
        return;
    }
    
    int index = findItemIndex(itemId);
    if (index != -1) {
        // 更新现有物品数量
        m_items[index].count = newCount;
    } else {
        // 添加新物品
        m_items.append(BackpackItemInfo(itemId, newCount));
    }
    
    fireBackpackUpdate();
}

int BackpackModel::getItemCount(int itemId) const noexcept
{
    int index = findItemIndex(itemId);
    return (index != -1) ? m_items[index].count : 0;
}

bool BackpackModel::hasItem(int itemId) const noexcept
{
    return findItemIndex(itemId) != -1;
}

void BackpackModel::clear() noexcept
{
    if (!m_items.isEmpty()) {
        m_items.clear();
        fireBackpackUpdate();
    }
}

void BackpackModel::saveToFile(const QString &filename) const
{
    QJsonArray jsonArray;
    for (const auto &item : m_items) {
        QJsonObject itemObj;
        itemObj["itemId"] = item.itemId;
        itemObj["count"] = item.count;
        jsonArray.append(itemObj);
    }
    
    QJsonDocument doc(jsonArray);
    
    // 获取应用数据存储路径
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(appDataPath);
    }
    
    QString fullPath = appDataPath + "/" + filename;
    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void BackpackModel::loadFromFile(const QString &filename)
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString fullPath = appDataPath + "/" + filename;
    
    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return; // 文件不存在，使用默认值
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        return; // JSON格式错误，使用默认值
    }
    
    QJsonArray jsonArray = doc.array();
    QVector<BackpackItemInfo> newItems;
    for (const QJsonValue &value : jsonArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            int itemId = obj["itemId"].toInt();
            int count = obj["count"].toInt();
            if (itemId > 0 && count > 0) {
                newItems.append(BackpackItemInfo(itemId, count));
            }
        }
    }
    
    // 更新数据
    if (!newItems.isEmpty() || !m_items.isEmpty()) {
        m_items = newItems;
        fireBackpackUpdate();
    }
}

void BackpackModel::initializeFromCollection() noexcept
{
    qDebug() << "从图鉴系统初始化背包...";
    
    CollectionManager& collectionMgr = CollectionManager::getInstance();
    auto collectionModel = collectionMgr.getCollectionModel();
    
    if (!collectionModel) {
        qWarning() << "图鉴系统未初始化，无法初始化背包";
        return;
    }
    
    // 清空现有物品
    m_items.clear();
    
    // 直接添加物品到背包，不通过addItem方法（避免循环调用）
    // 选择几样不同类型的物品，使用新的物品ID
    m_items.append(BackpackItemInfo(6, 5));   // 微光阳光 - 基础阳光材料
    m_items.append(BackpackItemInfo(7, 3));   // 温暖阳光 - 稀有阳光材料
    m_items.append(BackpackItemInfo(11, 4));  // 粗糙矿石 - 基础矿石材料
    m_items.append(BackpackItemInfo(16, 3));  // 枯木 - 基础木材
    m_items.append(BackpackItemInfo(101, 1)); // 木质锤子 - 工具
    m_items.append(BackpackItemInfo(151, 1)); // 草帽 - 装备
    
    // 手动解锁图鉴物品
    collectionMgr.unlockItem(6);
    collectionMgr.collectItem(6, 5);
    collectionMgr.unlockItem(7);
    collectionMgr.collectItem(7, 3);
    collectionMgr.unlockItem(11);
    collectionMgr.collectItem(11, 4);
    collectionMgr.unlockItem(16);
    collectionMgr.collectItem(16, 3);
    collectionMgr.unlockItem(101);
    collectionMgr.collectItem(101, 1);
    collectionMgr.unlockItem(151);
    collectionMgr.collectItem(151, 1);
    
    qDebug() << "背包初始化完成，物品数量:" << m_items.size();
    
    fireBackpackUpdate();
}

bool BackpackModel::getItemInfo(int itemId, CollectionItemInfo& outInfo) const noexcept
{
    CollectionManager& collectionMgr = CollectionManager::getInstance();
    auto collectionModel = collectionMgr.getCollectionModel();
    
    if (!collectionModel) {
        return false;
    }
    
    outInfo = collectionModel->getItemInfo(itemId);
    
    // 检查是否找到了有效的物品信息
    return (outInfo.id == itemId);
}

QString BackpackModel::getItemName(int itemId) const noexcept
{
    CollectionItemInfo itemInfo;
    if (getItemInfo(itemId, itemInfo)) {
        return itemInfo.name;
    }
    return QString("未知物品 %1").arg(itemId);
}

QString BackpackModel::getItemDescription(int itemId) const noexcept
{
    CollectionItemInfo itemInfo;
    if (getItemInfo(itemId, itemInfo)) {
        return itemInfo.description;
    }
    return QString("未知物品描述");
}

QString BackpackModel::getItemIcon(int itemId) const noexcept
{
    CollectionItemInfo itemInfo;
    if (getItemInfo(itemId, itemInfo)) {
        return itemInfo.iconPath;
    }
    return QString(":/resources/img/default_item.png");
}

CollectionCategory BackpackModel::getItemCategory(int itemId) const noexcept
{
    CollectionItemInfo itemInfo;
    if (getItemInfo(itemId, itemInfo)) {
        return itemInfo.category;
    }
    return CollectionCategory::Item;
}

CollectionRarity BackpackModel::getItemRarity(int itemId) const noexcept
{
    CollectionItemInfo itemInfo;
    if (getItemInfo(itemId, itemInfo)) {
        return itemInfo.rarity;
    }
    return CollectionRarity::Common;
}