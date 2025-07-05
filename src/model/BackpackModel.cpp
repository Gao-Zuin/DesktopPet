#include "BackpackModel.h"
#include "../common/PropertyIds.h"
#include <QDebug>

BackpackModel::BackpackModel() noexcept
{
    qDebug() << "BackpackModel 初始化...";
    // 初始化一些测试物品
    addItem(1, 3); 
    addItem(2, 1); 
    addItem(3, 5); 
    qDebug() << "BackpackModel 初始化完成，物品数量:" << m_items.size();
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
    
    int index = findItemIndex(itemId);
    if (index != -1) {
        // 物品已存在，增加数量
        m_items[index].count += count;
    } else {
        // 新物品，添加到背包
        m_items.append(BackpackItemInfo(itemId, count));
    }
    
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