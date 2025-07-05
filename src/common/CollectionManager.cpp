#include "CollectionManager.h"
#include <QDebug>

CollectionManager& CollectionManager::getInstance()
{
    static CollectionManager instance;
    return instance;
}

void CollectionManager::setCollectionModel(std::shared_ptr<CollectionModel> model)
{
    m_collection_model = model;
    qDebug() << "CollectionManager: 设置图鉴模型";
}

std::shared_ptr<CollectionModel> CollectionManager::getCollectionModel() const
{
    return m_collection_model;
}

bool CollectionManager::unlockItem(int itemId)
{
    if (m_collection_model) {
        return m_collection_model->unlockItem(itemId);
    }
    return false;
}

bool CollectionManager::collectItem(int itemId, int count)
{
    if (m_collection_model) {
        return m_collection_model->collectItem(itemId, count);
    }
    return false;
}

bool CollectionManager::syncFromBackpack(const QVector<BackpackItemInfo>& backpackItems)
{
    if (!m_collection_model) {
        return false;
    }
    
    qDebug() << "同步背包数据到图鉴...";
    
    // 遍历背包物品，更新图鉴数据
    for (const auto& item : backpackItems) {
        unlockItem(item.itemId);
        
        // 先获取当前图鉴中的数量
        auto itemInfo = m_collection_model->getItemInfo(item.itemId);
        int currentTotal = itemInfo.totalObtained;
        
        // 如果背包数量大于图鉴数量，补充差额
        if (item.count > currentTotal) {
            int diff = item.count - currentTotal;
            collectItem(item.itemId, diff);
            qDebug() << "同步物品:" << item.itemId << "背包数量:" << item.count << "图鉴数量:" << currentTotal << "补充:" << diff;
        }
        // 如果背包数量小于图鉴数量，说明图鉴记录了历史总数，保持不变
        else if (item.count < currentTotal) {
            qDebug() << "物品" << item.itemId << "图鉴历史总数(" << currentTotal << ")大于背包当前数量(" << item.count << ")，保持图鉴记录";
        }
        else {
            qDebug() << "物品" << item.itemId << "数量已同步:" << item.count;
        }
    }
    
    return true;
}
