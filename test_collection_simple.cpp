#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include "src/model/CollectionModel.h"
#include "src/common/CollectionManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    QCoreApplication::setOrganizationName("DesktopPet");
    QCoreApplication::setApplicationName("DesktopPet");
    
    // 创建图鉴模型
    qDebug() << "=== 创建图鉴模型 ===";
    auto collectionModel = std::make_shared<CollectionModel>();
    
    // 设置到CollectionManager
    CollectionManager& manager = CollectionManager::getInstance();
    manager.setCollectionModel(collectionModel);
    qDebug() << "CollectionManager 设置成功";
    
    // 测试加载图鉴数据
    qDebug() << "\n=== 测试图鉴数据加载 ===";
    collectionModel->loadItemsFromCSV(":/resources/csv/collection_items.csv");
    auto items = collectionModel->getAllItems();
    qDebug() << "图鉴条目总数:" << items.size();
    
    if (items.size() > 0) {
        // 显示前5个物品信息
        qDebug() << "\n=== 前5个物品信息 ===";
        for (int i = 0; i < qMin(5, items.size()); ++i) {
            const auto& item = items[i];
            qDebug() << "ID:" << item.id 
                     << "名称:" << item.name 
                     << "类别:" << (int)item.category
                     << "稀有度:" << (int)item.rarity
                     << "图标:" << item.iconPath;
        }
        
        // 测试收集物品
        qDebug() << "\n=== 测试物品收集 ===";
        manager.collectItem(1, 1);  // 收集木材
        manager.collectItem(101, 1); // 收集木质锤子
        manager.collectItem(201, 1); // 收集小蜘蛛皮肤
        manager.collectItem(301, 1); // 收集初心者成就
        
        // 检查收集状态
        qDebug() << "木材收集状态:" << collectionModel->isItemCollected(1);
        qDebug() << "木质锤子收集状态:" << collectionModel->isItemCollected(101);
        qDebug() << "小蜘蛛皮肤收集状态:" << collectionModel->isItemCollected(201);
        qDebug() << "初心者成就收集状态:" << collectionModel->isItemCollected(301);
        
        // 获取收集统计
        qDebug() << "\n=== 收集统计 ===";
        qDebug() << "总数:" << collectionModel->getTotalItems()
                 << "已收集:" << collectionModel->getCollectedItems()
                 << "完成率:" << QString::number(collectionModel->getCompletionRate() * 100, 'f', 2) << "%";
        
        // 测试搜索功能
        qDebug() << "\n=== 测试搜索功能 ===";
        auto searchResults = collectionModel->searchItems("锤子");
        qDebug() << "搜索'锤子'的结果数量:" << searchResults.size();
        for (const auto& item : searchResults) {
            qDebug() << "- " << item.name << "(" << item.description << ")";
        }
    }
    
    qDebug() << "\n=== 测试完成 ===";
    
    return 0;  // 直接退出，不需要事件循环
}
