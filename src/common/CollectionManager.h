#ifndef COLLECTION_MANAGER_H
#define COLLECTION_MANAGER_H

#include "../model/CollectionModel.h"
#include <memory>

class CollectionManager
{
public:
    static CollectionManager& getInstance();
    
    void setCollectionModel(std::shared_ptr<CollectionModel> model);
    std::shared_ptr<CollectionModel> getCollectionModel() const;
    
    // 便捷方法
    bool unlockItem(int itemId);
    bool collectItem(int itemId, int count = 1);

private:
    CollectionManager() = default;
    ~CollectionManager() = default;
    CollectionManager(const CollectionManager&) = delete;
    CollectionManager& operator=(const CollectionManager&) = delete;
    
    std::shared_ptr<CollectionModel> m_collection_model;
};

#endif // COLLECTION_MANAGER_H
