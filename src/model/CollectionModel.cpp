#include "CollectionModel.h"

CollectionModel::CollectionModel(QObject *parent)
    : QObject(parent)
{
    qDebug() << "CollectionModel 构造完成";
}

void CollectionModel::loadFromFile(const QString &filename)
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString fullPath = appDataPath + "/" + filename;
    
    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "图鉴数据文件不存在，使用默认值:" << fullPath;
        return;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qDebug() << "图鉴数据JSON格式错误";
        return;
    }
    
    QJsonObject rootJson = doc.object();
    
    if (rootJson.contains("collections")) {
        QJsonObject collectionsJson = rootJson["collections"].toObject();
        
        for (auto it = collectionsJson.constBegin(); it != collectionsJson.constEnd(); ++it) {
            int itemId = it.key().toInt();
            QJsonObject itemJson = it.value().toObject();
            
            if (m_items.contains(itemId)) {
                CollectionItemInfo &info = m_items[itemId];
                
                if (itemJson.contains("status")) {
                    info.status = static_cast<CollectionStatus>(itemJson["status"].toInt());
                }
                if (itemJson.contains("totalObtained")) {
                    info.totalObtained = itemJson["totalObtained"].toInt();
                }
                if (itemJson.contains("firstObtainedTime")) {
                    QString timeStr = itemJson["firstObtainedTime"].toString();
                    if (!timeStr.isEmpty()) {
                        info.firstObtainedTime = QDateTime::fromString(timeStr, Qt::ISODate);
                    }
                }
                
                qDebug() << "加载图鉴物品:" << itemId << "状态:" << static_cast<int>(info.status) 
                         << "获得数量:" << info.totalObtained;
            }
        }
    }
    
    qDebug() << "图鉴数据加载完成，总物品数:" << m_items.size();
}

void CollectionModel::saveToFile(const QString &filename) const
{
    QJsonObject rootJson;
    rootJson["version"] = "1.0";
    
    QJsonObject collectionsJson;
    for (auto it = m_items.constBegin(); it != m_items.constEnd(); ++it) {
        const CollectionItemInfo &info = it.value();
        
        // 只保存已发现或已收集的物品
        if (info.status != CollectionStatus::Unknown) {
            QJsonObject itemJson;
            itemJson["status"] = static_cast<int>(info.status);
            itemJson["totalObtained"] = info.totalObtained;
            itemJson["firstObtainedTime"] = info.firstObtainedTime.toString(Qt::ISODate);
            
            collectionsJson[QString::number(info.id)] = itemJson;
        }
    }
    rootJson["collections"] = collectionsJson;
    
    // 保存统计信息
    QJsonObject statsJson;
    statsJson["totalItems"] = getTotalItems();
    statsJson["collectedItems"] = getCollectedItems();
    statsJson["discoveredItems"] = getDiscoveredItems();
    statsJson["completionRate"] = getCompletionRate();
    rootJson["statistics"] = statsJson;
    
    QJsonDocument doc(rootJson);
    
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
        qDebug() << "图鉴数据已保存到:" << fullPath;
    } else {
        qDebug() << "保存图鉴数据失败:" << fullPath;
    }
}

void CollectionModel::loadItemsFromCSV(const QString &csvPath)
{
    qDebug() << "开始加载图鉴物品配置:" << csvPath;
    
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开图鉴配置文件:" << csvPath;
        return;
    }
    
    QTextStream in(&file);
    bool firstLine = true;
    int loadedCount = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }
        
        // 跳过标题行
        if (firstLine) {
            firstLine = false;
            continue;
        }
        
        QStringList parts = line.split(',');
        if (parts.size() < 7) {
            qWarning() << "无效的图鉴配置行:" << line;
            continue;
        }
        
        bool ok;
        int id = parts[0].toInt(&ok);
        if (!ok) {
            qWarning() << "无效的物品ID:" << parts[0];
            continue;
        }
        
        QString name = parts[1].trimmed();
        QString description = parts[2].trimmed();
        CollectionCategory category = static_cast<CollectionCategory>(parts[3].toInt());
        CollectionRarity rarity = static_cast<CollectionRarity>(parts[4].toInt());
        QString iconPath = parts[5].trimmed();
        QString detailPath = parts[6].trimmed();
        bool isHidden = (parts.size() > 7) ? (parts[7].trimmed().toLower() == "true") : false;
        
        CollectionItemInfo info(id, name, description, category, rarity, iconPath, detailPath, isHidden);
        m_items[id] = info;
        loadedCount++;
    }
    
    file.close();
    qDebug() << "图鉴物品配置加载完成，共加载" << loadedCount << "个物品";
}

bool CollectionModel::unlockItem(int itemId)
{
    ensureItemExists(itemId);
    
    CollectionItemInfo &info = m_items[itemId];
    if (info.status == CollectionStatus::Unknown) {
        info.status = CollectionStatus::Discovered;
        qDebug() << "解锁图鉴物品:" << itemId << "(" << info.name << ")";
        
        emit itemUnlocked(itemId);
        fireCollectionUpdate();
        return true;
    }
    
    return false;
}

bool CollectionModel::collectItem(int itemId, int count)
{
    if (count <= 0) return false;
    
    ensureItemExists(itemId);
    
    CollectionItemInfo &info = m_items[itemId];
    
    // 如果是首次收集，记录时间
    if (info.status != CollectionStatus::Collected) {
        info.firstObtainedTime = QDateTime::currentDateTime();
        info.status = CollectionStatus::Collected;
        qDebug() << "首次收集图鉴物品:" << itemId << "(" << info.name << ")";
    }
    
    info.totalObtained += count;
    qDebug() << "收集图鉴物品:" << itemId << "数量:" << count << "总计:" << info.totalObtained;
    
    emit itemCollected(itemId, count);
    fireCollectionUpdate();
    return true;
}

bool CollectionModel::isItemUnlocked(int itemId) const
{
    if (!m_items.contains(itemId)) return false;
    return m_items[itemId].status != CollectionStatus::Unknown;
}

bool CollectionModel::isItemCollected(int itemId) const
{
    if (!m_items.contains(itemId)) return false;
    return m_items[itemId].status == CollectionStatus::Collected;
}

CollectionItemInfo CollectionModel::getItemInfo(int itemId) const
{
    if (m_items.contains(itemId)) {
        return m_items[itemId];
    }
    return CollectionItemInfo();  // 返回空信息
}

QVector<CollectionItemInfo> CollectionModel::getItemsByCategory(CollectionCategory category) const
{
    QVector<CollectionItemInfo> result;
    for (const auto &info : m_items) {
        if (info.category == category) {
            result.append(info);
        }
    }
    return result;
}

QVector<CollectionItemInfo> CollectionModel::getItemsByRarity(CollectionRarity rarity) const
{
    QVector<CollectionItemInfo> result;
    for (const auto &info : m_items) {
        if (info.rarity == rarity) {
            result.append(info);
        }
    }
    return result;
}

QVector<CollectionItemInfo> CollectionModel::getItemsByStatus(CollectionStatus status) const
{
    QVector<CollectionItemInfo> result;
    for (const auto &info : m_items) {
        if (info.status == status) {
            result.append(info);
        }
    }
    return result;
}

QVector<CollectionItemInfo> CollectionModel::getAllItems() const
{
    QVector<CollectionItemInfo> result;
    for (const auto &info : m_items) {
        result.append(info);
    }
    return result;
}

QVector<CollectionItemInfo> CollectionModel::searchItems(const QString &keyword) const
{
    QVector<CollectionItemInfo> result;
    QString lowerKeyword = keyword.toLower();
    
    for (const auto &info : m_items) {
        if (info.name.toLower().contains(lowerKeyword) || 
            info.description.toLower().contains(lowerKeyword)) {
            result.append(info);
        }
    }
    return result;
}

int CollectionModel::getTotalItems() const
{
    return m_items.size();
}

int CollectionModel::getCollectedItems() const
{
    int count = 0;
    for (const auto &info : m_items) {
        if (info.status == CollectionStatus::Collected) {
            count++;
        }
    }
    return count;
}

int CollectionModel::getDiscoveredItems() const
{
    int count = 0;
    for (const auto &info : m_items) {
        if (info.status == CollectionStatus::Discovered || 
            info.status == CollectionStatus::Collected) {
            count++;
        }
    }
    return count;
}

int CollectionModel::getItemsByCategory(CollectionCategory category, CollectionStatus status) const
{
    int count = 0;
    for (const auto &info : m_items) {
        if (info.category == category && info.status == status) {
            count++;
        }
    }
    return count;
}

float CollectionModel::getCompletionRate() const
{
    int total = getTotalItems();
    if (total == 0) return 0.0f;
    
    int collected = getCollectedItems();
    return static_cast<float>(collected) / static_cast<float>(total);
}

void CollectionModel::printStatistics() const
{
    qDebug() << "=== 图鉴统计 ===";
    qDebug() << "总物品数:" << getTotalItems();
    qDebug() << "已收集:" << getCollectedItems();
    qDebug() << "已发现:" << getDiscoveredItems();
    qDebug() << "完成率:" << (getCompletionRate() * 100) << "%";
    
    // 按分类统计
    for (int i = 0; i < 4; ++i) {
        CollectionCategory category = static_cast<CollectionCategory>(i);
        int total = getItemsByCategory(category).size();
        int collected = getItemsByCategory(category, CollectionStatus::Collected);
        qDebug() << getCategoryName(category) << ":" << collected << "/" << total;
    }
}

void CollectionModel::fireCollectionUpdate()
{
    emit collectionUpdated();
}

void CollectionModel::ensureItemExists(int itemId)
{
    if (!m_items.contains(itemId)) {
        // 如果物品不存在，创建一个基本信息
        CollectionItemInfo info;
        info.id = itemId;
        info.name = QString("未知物品 #%1").arg(itemId);
        info.description = "未配置的物品";
        info.category = CollectionCategory::Item;
        info.rarity = CollectionRarity::Common;
        info.iconPath = ":/resources/img/unknown.png";
        
        m_items[itemId] = info;
        qDebug() << "创建未知物品信息:" << itemId;
    }
}
