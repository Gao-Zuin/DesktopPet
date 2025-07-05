# 图鉴系统设计方案

## 🎯 系统概述

### 核心概念
- **图鉴系统**：展示收集到的物品/材料/皮肤等的图片和信息
- **收集机制**：通过打工、任务等方式获得物品，自动解锁图鉴
- **与背包结合**：背包中的物品自动在图鉴中解锁

## 📋 功能设计

### 1. 图鉴分类
```
图鉴系统
├── 材料图鉴
│   ├── 基础材料（木材、石头、金属等）
│   ├── 稀有材料（宝石、魔法水晶等）
│   └── 特殊材料（活动限定、隐藏材料等）
├── 物品图鉴
│   ├── 工具（锤子、铲子等）
│   ├── 装备（帽子、衣服等）
│   └── 消耗品（食物、药水等）
├── 皮肤图鉴
│   ├── 宠物皮肤
│   ├── 主题皮肤
│   └── 特殊皮肤
└── 成就图鉴
    ├── 打工成就
    ├── 收集成就
    └── 特殊成就
```

### 2. 图鉴状态
- **未发现**：灰色轮廓，显示"？？？"
- **已发现**：显示物品图标和名称，但信息不完整
- **已收集**：完整显示所有信息，包括获得时间、数量等

### 3. 与背包系统的结合
```cpp
// 背包获得物品时自动解锁图鉴
void BackpackModel::addItem(int itemId, int count) {
    // 原有逻辑...
    
    // 自动解锁图鉴
    CollectionManager::getInstance().unlockItem(itemId);
    
    // 更新图鉴状态
    if (count > 0) {
        CollectionManager::getInstance().setItemCollected(itemId, true);
    }
}
```

## 🏗️ 技术实现

### 1. 数据结构设计

#### 图鉴物品信息
```cpp
// src/model/base/CollectionInfo.h
enum class CollectionCategory {
    Material,      // 材料
    Item,         // 物品
    Skin,         // 皮肤
    Achievement   // 成就
};

enum class CollectionRarity {
    Common,       // 普通
    Rare,         // 稀有
    Epic,         // 史诗
    Legendary     // 传说
};

enum class CollectionStatus {
    Unknown,      // 未发现
    Discovered,   // 已发现
    Collected     // 已收集
};

struct CollectionItemInfo {
    int id;
    QString name;
    QString description;
    QString iconPath;
    QString detailImagePath;      // 详细图片路径
    CollectionCategory category;
    CollectionRarity rarity;
    CollectionStatus status;
    QDateTime firstObtainedTime;  // 首次获得时间
    int totalObtained;           // 总获得数量
    bool isHidden;               // 是否为隐藏物品
};
```

#### 图鉴模型
```cpp
// src/model/CollectionModel.h
class CollectionModel : public QObject {
    Q_OBJECT

public:
    CollectionModel(QObject *parent = nullptr);
    
    // 基本操作
    void loadFromFile(const QString &filename = "collection_data.json");
    void saveToFile(const QString &filename = "collection_data.json") const;
    
    // 解锁和收集
    bool unlockItem(int itemId);
    bool collectItem(int itemId, int count = 1);
    
    // 查询
    CollectionItemInfo getItemInfo(int itemId) const;
    QVector<CollectionItemInfo> getItemsByCategory(CollectionCategory category) const;
    QVector<CollectionItemInfo> getItemsByRarity(CollectionRarity rarity) const;
    
    // 统计
    int getTotalItems() const;
    int getCollectedItems() const;
    int getDiscoveredItems() const;
    float getCompletionRate() const;
    
signals:
    void itemUnlocked(int itemId);
    void itemCollected(int itemId);
    void collectionUpdated();

private:
    QMap<int, CollectionItemInfo> m_items;
    void loadItemsFromCSV(const QString &csvPath);
    void fireCollectionUpdate();
};
```

### 2. 界面设计

#### 图鉴主界面
```cpp
// src/view/CollectionPanel.h
class CollectionPanel : public QWidget {
    Q_OBJECT

public:
    CollectionPanel(CollectionModel &model, QWidget *parent = nullptr);

private slots:
    void onCategoryChanged(int category);
    void onItemClicked(int itemId);
    void onSearchTextChanged(const QString &text);
    void updateDisplay();

private:
    void setupUi();
    void updateItemGrid();
    void updateStatistics();
    
    CollectionModel &m_collection_model;
    QTabWidget *m_categoryTabs;
    QGridLayout *m_itemGrid;
    QLineEdit *m_searchEdit;
    QLabel *m_statsLabel;
    QVector<CollectionItemWidget*> m_itemWidgets;
};
```

#### 图鉴物品控件
```cpp
// src/view/CollectionItemWidget.h
class CollectionItemWidget : public QWidget {
    Q_OBJECT

public:
    CollectionItemWidget(const CollectionItemInfo &info, QWidget *parent = nullptr);
    void updateInfo(const CollectionItemInfo &info);

signals:
    void clicked(int itemId);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    CollectionItemInfo m_info;
    void drawItem(QPainter &painter);
    void drawRarityBorder(QPainter &painter);
    void drawStatus(QPainter &painter);
};
```

### 3. 数据存储

#### CSV 配置文件
```csv
# resources/csv/collection_items.csv
id,name,description,category,rarity,iconPath,detailImagePath,isHidden
1,木材,基础建造材料,0,0,/resources/img/materials/wood.png,/resources/img/materials/wood_detail.png,false
2,铁矿,坚硬的金属材料,0,1,/resources/img/materials/iron.png,/resources/img/materials/iron_detail.png,false
3,魔法水晶,蕴含魔力的特殊材料,0,2,/resources/img/materials/crystal.png,/resources/img/materials/crystal_detail.png,false
101,小蜘蛛皮肤,默认蜘蛛皮肤,2,0,/resources/img/skins/spider_default.png,/resources/img/skins/spider_default_detail.png,false
102,卡西迪皮肤,默认卡西迪皮肤,2,0,/resources/img/skins/cassidy_default.png,/resources/img/skins/cassidy_default_detail.png,false
```

#### JSON 存储格式
```json
{
    "version": "1.0",
    "collections": {
        "1": {
            "status": 2,
            "firstObtainedTime": "2024-01-01T10:00:00",
            "totalObtained": 15
        },
        "2": {
            "status": 1,
            "firstObtainedTime": "",
            "totalObtained": 0
        }
    },
    "statistics": {
        "totalItems": 50,
        "collectedItems": 12,
        "discoveredItems": 25,
        "completionRate": 0.24
    }
}
```

## 🔗 与现有系统的集成

### 1. 与背包系统结合
```cpp
// 修改 BackpackModel.cpp
void BackpackModel::addItem(int itemId, int count) {
    // 原有添加逻辑...
    
    // 自动解锁图鉴
    auto collectionModel = CollectionManager::getInstance().getModel();
    if (collectionModel) {
        collectionModel->unlockItem(itemId);
        if (count > 0) {
            collectionModel->collectItem(itemId, count);
        }
    }
    
    fireBackpackUpdate();
}
```

### 2. 与工作系统结合
```cpp
// 新增工作系统相关代码
class WorkManager {
public:
    struct WorkResult {
        int exp;
        int money;
        QVector<QPair<int, int>> materials;  // 物品ID和数量
    };
    
    WorkResult doWork(int workType, int duration) {
        WorkResult result;
        // 工作逻辑...
        
        // 添加材料到背包（自动解锁图鉴）
        for (const auto &material : result.materials) {
            BackpackModel::getInstance().addItem(material.first, material.second);
        }
        
        return result;
    }
};
```

### 3. 菜单集成
```cpp
// 修改 PetMainWindow.cpp 添加图鉴菜单
void PetMainWindow::createContextMenu() {
    // 原有菜单...
    
    m_context_menu->addSeparator();
    
    QAction *collectionAction = m_context_menu->addAction("查看图鉴");
    connect(collectionAction, &QAction::triggered, [this]() {
        ICommandBase *command = m_command_manager.get_command(CommandType::SHOW_COLLECTION_PANEL);
        if (command) {
            command->exec(nullptr);
        }
    });
}
```

## 🎮 用户体验设计

### 1. 视觉效果
- **稀有度颜色**：
  - 普通：灰色边框
  - 稀有：蓝色边框
  - 史诗：紫色边框
  - 传说：金色边框

- **状态指示**：
  - 未发现：灰色轮廓，问号图标
  - 已发现：半透明显示，"NEW"标签
  - 已收集：完整显示，收集时间戳

### 2. 交互设计
- **分类标签**：材料、物品、皮肤、成就
- **筛选功能**：按稀有度、状态筛选
- **搜索功能**：按名称搜索
- **详情弹窗**：点击物品显示详细信息

### 3. 统计信息
- **总体进度**：已收集/总数量 (24/50)
- **完成率**：48%
- **稀有度统计**：各稀有度的收集情况

## 📁 文件结构

```
src/
├── model/
│   ├── CollectionModel.h/cpp
│   └── base/
│       └── CollectionInfo.h
├── view/
│   ├── CollectionPanel.h/cpp
│   ├── CollectionItemWidget.h/cpp
│   └── CollectionDetailDialog.h/cpp
├── viewmodel/
│   └── commands/
│       └── ShowCollectionPanelCommand.h/cpp
└── common/
    └── CollectionManager.h/cpp

resources/
├── csv/
│   └── collection_items.csv
└── img/
    ├── materials/
    ├── items/
    └── skins/
```

## 🔄 实现优先级

### 第一阶段（MVP）
1. ✅ CollectionModel 基础功能
2. ✅ CollectionPanel 基础界面
3. ✅ 与背包系统集成
4. ✅ 基础数据存储

### 第二阶段（增强）
1. ✅ 详情弹窗
2. ✅ 搜索和筛选
3. ✅ 稀有度视觉效果
4. ✅ 统计信息

### 第三阶段（完善）
1. ✅ 成就系统
2. ✅ 工作系统集成
3. ✅ 动画效果
4. ✅ 社交分享

这个设计既保持了与现有系统的兼容性，又为未来的扩展预留了空间。要开始实现吗？
