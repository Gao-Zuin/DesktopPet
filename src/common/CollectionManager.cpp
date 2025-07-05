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
