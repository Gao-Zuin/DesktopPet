#include "CollectionPanel.h"
#include "CollectionItemWidget.h"
#include <QMessageBox>

CollectionPanel::CollectionPanel(CollectionModel &model, QWidget *parent)
    : QWidget(parent)
    , m_collection_model(model)
    , m_currentCategory(CollectionCategory::Material)
    , m_currentRarity(CollectionRarity::Common)
    , m_currentStatus(CollectionStatus::Unknown)
{
    setupUi();
    updateDisplay();
    
    // 连接信号
    connect(&m_collection_model, &CollectionModel::collectionUpdated, 
            this, &CollectionPanel::onCollectionUpdated);
    connect(&m_collection_model, &CollectionModel::itemUnlocked, 
            this, &CollectionPanel::onCollectionUpdated);
    connect(&m_collection_model, &CollectionModel::itemCollected, 
            this, &CollectionPanel::onCollectionUpdated);
    
    qDebug() << "CollectionPanel 构造完成";
}

CollectionPanel::~CollectionPanel()
{
    qDeleteAll(m_itemWidgets);
}

void CollectionPanel::setupUi()
{
    setWindowTitle("图鉴系统");
    setFixedSize(800, 600);
    setWindowFlags(Qt::Tool | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
    setAttribute(Qt::WA_DeleteOnClose, true);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);
    
    // 控制面板
    QGroupBox *controlGroup = new QGroupBox("筛选和搜索", this);
    m_controlLayout = new QHBoxLayout(controlGroup);
    
    // 分类筛选
    QLabel *categoryLabel = new QLabel("分类:", this);
    m_categoryCombo = new QComboBox(this);
    m_categoryCombo->addItem("全部", -1);
    m_categoryCombo->addItem("材料", static_cast<int>(CollectionCategory::Material));
    m_categoryCombo->addItem("物品", static_cast<int>(CollectionCategory::Item));
    m_categoryCombo->addItem("皮肤", static_cast<int>(CollectionCategory::Skin));
    m_categoryCombo->addItem("成就", static_cast<int>(CollectionCategory::Achievement));
    connect(m_categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CollectionPanel::onCategoryChanged);
    
    // 稀有度筛选
    QLabel *rarityLabel = new QLabel("稀有度:", this);
    m_rarityCombo = new QComboBox(this);
    m_rarityCombo->addItem("全部", -1);
    m_rarityCombo->addItem("普通", static_cast<int>(CollectionRarity::Common));
    m_rarityCombo->addItem("稀有", static_cast<int>(CollectionRarity::Rare));
    m_rarityCombo->addItem("史诗", static_cast<int>(CollectionRarity::Epic));
    m_rarityCombo->addItem("传说", static_cast<int>(CollectionRarity::Legendary));
    connect(m_rarityCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CollectionPanel::onRarityFilterChanged);
    
    // 状态筛选
    QLabel *statusLabel = new QLabel("状态:", this);
    m_statusCombo = new QComboBox(this);
    m_statusCombo->addItem("全部", -1);
    m_statusCombo->addItem("未发现", static_cast<int>(CollectionStatus::Unknown));
    m_statusCombo->addItem("已发现", static_cast<int>(CollectionStatus::Discovered));
    m_statusCombo->addItem("已收集", static_cast<int>(CollectionStatus::Collected));
    connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CollectionPanel::onStatusFilterChanged);
    
    // 搜索框
    QLabel *searchLabel = new QLabel("搜索:", this);
    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("输入物品名称或描述...");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &CollectionPanel::onSearchTextChanged);
    
    // 刷新按钮
    m_refreshButton = new QPushButton("刷新", this);
    connect(m_refreshButton, &QPushButton::clicked, this, &CollectionPanel::onRefreshClicked);
    
    m_controlLayout->addWidget(categoryLabel);
    m_controlLayout->addWidget(m_categoryCombo);
    m_controlLayout->addWidget(rarityLabel);
    m_controlLayout->addWidget(m_rarityCombo);
    m_controlLayout->addWidget(statusLabel);
    m_controlLayout->addWidget(m_statusCombo);
    m_controlLayout->addWidget(searchLabel);
    m_controlLayout->addWidget(m_searchEdit);
    m_controlLayout->addWidget(m_refreshButton);
    m_controlLayout->addStretch();
    
    m_mainLayout->addWidget(controlGroup);
    
    // 统计信息
    QGroupBox *statsGroup = new QGroupBox("统计信息", this);
    QVBoxLayout *statsLayout = new QVBoxLayout(statsGroup);
    
    m_statsLabel = new QLabel("正在加载...", this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    
    statsLayout->addWidget(m_statsLabel);
    statsLayout->addWidget(m_progressBar);
    
    m_mainLayout->addWidget(statsGroup);
    
    // 物品网格
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_itemGridWidget = new QWidget();
    m_itemGridLayout = new QGridLayout(m_itemGridWidget);
    m_itemGridLayout->setSpacing(5);
    m_itemGridLayout->setContentsMargins(10, 10, 10, 10);
    
    m_scrollArea->setWidget(m_itemGridWidget);
    m_mainLayout->addWidget(m_scrollArea);
    
    setLayout(m_mainLayout);
}

void CollectionPanel::updateDisplay()
{
    updateItemGrid();
    updateStatistics();
}

void CollectionPanel::updateItemGrid()
{
    // 清空现有的物品控件
    qDeleteAll(m_itemWidgets);
    m_itemWidgets.clear();
    
    // 清空布局
    QLayoutItem *item;
    while ((item = m_itemGridLayout->takeAt(0)) != nullptr) {
        delete item;
    }
    
    // 获取过滤后的物品列表
    QVector<CollectionItemInfo> items = m_collection_model.getAllItems();
    
    // 应用过滤器
    QVector<CollectionItemInfo> filteredItems;
    for (const auto &info : items) {
        bool match = true;
        
        // 分类过滤
        if (m_categoryCombo->currentData().toInt() != -1) {
            if (info.category != static_cast<CollectionCategory>(m_categoryCombo->currentData().toInt())) {
                match = false;
            }
        }
        
        // 稀有度过滤
        if (m_rarityCombo->currentData().toInt() != -1) {
            if (info.rarity != static_cast<CollectionRarity>(m_rarityCombo->currentData().toInt())) {
                match = false;
            }
        }
        
        // 状态过滤
        if (m_statusCombo->currentData().toInt() != -1) {
            if (info.status != static_cast<CollectionStatus>(m_statusCombo->currentData().toInt())) {
                match = false;
            }
        }
        
        // 搜索过滤
        if (!m_searchKeyword.isEmpty()) {
            if (!info.name.contains(m_searchKeyword, Qt::CaseInsensitive) &&
                !info.description.contains(m_searchKeyword, Qt::CaseInsensitive)) {
                match = false;
            }
        }
        
        if (match) {
            filteredItems.append(info);
        }
    }
    
    // 按ID排序
    std::sort(filteredItems.begin(), filteredItems.end(), 
              [](const CollectionItemInfo &a, const CollectionItemInfo &b) {
                  return a.id < b.id;
              });
    
    // 创建物品控件
    int row = 0, col = 0;
    for (const auto &info : filteredItems) {
        CollectionItemWidget *widget = new CollectionItemWidget(info, this);
        connect(widget, &CollectionItemWidget::clicked, this, &CollectionPanel::onItemClicked);
        
        m_itemWidgets.append(widget);
        m_itemGridLayout->addWidget(widget, row, col);
        
        col++;
        if (col >= GRID_COLUMNS) {
            col = 0;
            row++;
        }
    }
    
    // 添加弹性空间
    m_itemGridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding), 
                             row + 1, 0, 1, GRID_COLUMNS);
    
    qDebug() << "更新物品网格，显示" << filteredItems.size() << "个物品";
}

void CollectionPanel::updateStatistics()
{
    int total = m_collection_model.getTotalItems();
    int collected = m_collection_model.getCollectedItems();
    int discovered = m_collection_model.getDiscoveredItems();
    float completionRate = m_collection_model.getCompletionRate();
    
    QString statsText = QString("总物品: %1 | 已收集: %2 | 已发现: %3 | 完成率: %4%")
                        .arg(total)
                        .arg(collected)
                        .arg(discovered)
                        .arg(QString::number(completionRate * 100, 'f', 1));
    
    m_statsLabel->setText(statsText);
    m_progressBar->setValue(static_cast<int>(completionRate * 100));
}

void CollectionPanel::onCategoryChanged(int index)
{
    Q_UNUSED(index)
    updateDisplay();
}

void CollectionPanel::onRarityFilterChanged(int index)
{
    Q_UNUSED(index)
    updateDisplay();
}

void CollectionPanel::onStatusFilterChanged(int index)
{
    Q_UNUSED(index)
    updateDisplay();
}

void CollectionPanel::onSearchTextChanged(const QString &text)
{
    m_searchKeyword = text;
    updateDisplay();
}

void CollectionPanel::onItemClicked(int itemId)
{
    qDebug() << "点击图鉴物品:" << itemId;
    showItemDetail(itemId);
}

void CollectionPanel::onCollectionUpdated()
{
    updateDisplay();
}

void CollectionPanel::onRefreshClicked()
{
    m_collection_model.printStatistics();
    updateDisplay();
}

void CollectionPanel::showItemDetail(int itemId)
{
    CollectionItemInfo info = m_collection_model.getItemInfo(itemId);
    if (info.id == 0) return;
    
    QString detailText = QString("物品详情\n\n"
                                "名称: %1\n"
                                "描述: %2\n"
                                "分类: %3\n"
                                "稀有度: %4\n"
                                "状态: %5\n"
                                "获得数量: %6\n"
                                "首次获得: %7")
                        .arg(info.name)
                        .arg(info.description)
                        .arg(getCategoryName(info.category))
                        .arg(getRarityName(info.rarity))
                        .arg(getStatusName(info.status))
                        .arg(info.totalObtained)
                        .arg(info.firstObtainedTime.isValid() ? 
                             info.firstObtainedTime.toString("yyyy-MM-dd hh:mm:ss") : "未获得");
    
    QMessageBox::information(this, "图鉴详情", detailText);
}
