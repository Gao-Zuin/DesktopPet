#ifndef COLLECTION_INFO_H
#define COLLECTION_INFO_H

#include <QString>
#include <QDateTime>

enum class CollectionCategory {
    Material = 0,     // 材料
    Item = 1,         // 物品
    Skin = 2,         // 皮肤
    Achievement = 3   // 成就
};

enum class CollectionRarity {
    Common = 0,       // 普通 - 灰色
    Rare = 1,         // 稀有 - 蓝色
    Epic = 2,         // 史诗 - 紫色
    Legendary = 3     // 传说 - 金色
};

enum class CollectionStatus {
    Unknown = 0,      // 未发现 - 显示问号
    Discovered = 1,   // 已发现 - 半透明显示
    Collected = 2     // 已收集 - 完整显示
};

struct CollectionItemInfo {
    int id = 0;
    QString name;
    QString description;
    QString iconPath;
    QString detailImagePath;
    CollectionCategory category = CollectionCategory::Material;
    CollectionRarity rarity = CollectionRarity::Common;
    CollectionStatus status = CollectionStatus::Unknown;
    QDateTime firstObtainedTime;
    int totalObtained = 0;
    bool isHidden = false;
    
    // 构造函数
    CollectionItemInfo() = default;
    CollectionItemInfo(int itemId, const QString &itemName, const QString &desc, 
                      CollectionCategory cat, CollectionRarity rare, 
                      const QString &icon, const QString &detail = "", bool hidden = false)
        : id(itemId), name(itemName), description(desc), iconPath(icon), detailImagePath(detail),
          category(cat), rarity(rare), isHidden(hidden) {}
};

// 辅助函数
inline QString getRarityName(CollectionRarity rarity) {
    switch (rarity) {
        case CollectionRarity::Common: return "普通";
        case CollectionRarity::Rare: return "稀有";
        case CollectionRarity::Epic: return "史诗";
        case CollectionRarity::Legendary: return "传说";
        default: return "未知";
    }
}

inline QString getCategoryName(CollectionCategory category) {
    switch (category) {
        case CollectionCategory::Material: return "材料";
        case CollectionCategory::Item: return "物品";
        case CollectionCategory::Skin: return "皮肤";
        case CollectionCategory::Achievement: return "成就";
        default: return "未知";
    }
}

inline QString getStatusName(CollectionStatus status) {
    switch (status) {
        case CollectionStatus::Unknown: return "未发现";
        case CollectionStatus::Discovered: return "已发现";
        case CollectionStatus::Collected: return "已收集";
        default: return "未知";
    }
}

#endif // COLLECTION_INFO_H
