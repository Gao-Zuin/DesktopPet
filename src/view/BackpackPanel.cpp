#include "BackpackPanel.h"
#include "../common/PropertyIds.h"
#include "../common/CommandParameters.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMouseEvent>
#include <QDebug>
#include <QPixmapCache>

// 静态图片缓存
static QHash<QString, QPixmap> s_iconCache;

// ===================== ItemSlot 实现 =====================

ItemSlot::ItemSlot(QWidget *parent)
    : QWidget(parent), m_iconLabel(nullptr), m_countLabel(nullptr), m_itemId(0), m_itemCount(0)
{
    setupUi();
    clearItem();
    setMouseTracking(true);
}

void ItemSlot::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void ItemSlot::setupUi()
{
    // 设置格子样式
    setFixedSize(70, 80);
    setStyleSheet("ItemSlot { background-color: #f8f8f8; border: 2px solid #ddd; border-radius: 8px; } ItemSlot:hover { border-color: #4CAF50; background-color: #f0f8ff; }");

    // 图标标签
    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setGeometry(5, 5, 60, 60);
    m_iconLabel->setStyleSheet("border: none; background-color: transparent;");

    // 数量标签 - 放在图标下方，独立的数字框
    m_countLabel = new QLabel(this);
    m_countLabel->setAlignment(Qt::AlignCenter);
    m_countLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'Consolas', 'Monaco', monospace;"  // 等宽字体
        "    font-size: 11px;"
        "    font-weight: bold;"
        "    color: #333333;"
        "    background-color: rgba(255, 255, 255, 0.9);"
        "    border: 1px solid #bbb;"
        "    border-radius: 4px;"
        "    padding: 2px 4px;"
        "    min-width: 20px;"
        "}"
    );
    m_countLabel->setGeometry(5, 65, 60, 18);  // 放在图标下方
}

void ItemSlot::setItem(const BackpackItemInfo &item, const ItemDisplayInfo &displayInfo)
{
    m_itemId = item.itemId;
    m_itemCount = item.count;
    m_itemName = displayInfo.name;
    m_itemDescription = displayInfo.description;
    m_itemCategory = displayInfo.category;
    m_itemRarity = displayInfo.rarity;

    // 设置图标 - 使用缓存优化加载
    QPixmap pixmap;
    
    // 先检查缓存
    if (s_iconCache.contains(displayInfo.iconPath)) {
        pixmap = s_iconCache.value(displayInfo.iconPath);
    } else {
        // 加载并缓存图片
        pixmap = QPixmap(displayInfo.iconPath);
        if (!pixmap.isNull()) {
            // 预先缩放并缓存
            pixmap = pixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            s_iconCache.insert(displayInfo.iconPath, pixmap);
        }
    }
    
    if (!pixmap.isNull()) {
        m_iconLabel->setPixmap(pixmap);
    } else {
        m_iconLabel->setText(displayInfo.iconPath);
    }

    // 设置数量 - 改进数量显示
    if (m_itemCount > 1)
    {
        m_countLabel->setText(QString::number(m_itemCount));
        m_countLabel->show();
    }
    else
    {
        m_countLabel->hide();
    }

    // 设置详细的工具提示
    showDetailedTooltip();

    // 更新样式 - 有物品时的高亮效果
    setStyleSheet("ItemSlot { background-color: #e8f4ff; border: 2px solid #4CAF50; border-radius: 8px; } ItemSlot:hover { border-color: #2196F3; background-color: #f0f8ff; }");
}

void ItemSlot::clearItem()
{
    m_itemId = 0;
    m_itemCount = 0;
    m_itemName.clear();
    m_itemDescription.clear();
    m_itemCategory.clear();
    m_itemRarity.clear();
    m_iconLabel->clear();
    m_countLabel->hide();
    setToolTip("");
    setStyleSheet("ItemSlot { background-color: #f8f8f8; border: 2px solid #ddd; border-radius: 8px; } ItemSlot:hover { border-color: #4CAF50; background-color: #f0f8ff; }");
}

void ItemSlot::enterEvent(QEnterEvent *event)
{
    if (!isEmpty()) {
        showDetailedTooltip();
    }
    QWidget::enterEvent(event);
}

void ItemSlot::leaveEvent(QEvent *event)
{
    // 可以在这里隐藏详细信息，如果需要的话
    QWidget::leaveEvent(event);
}

void ItemSlot::showDetailedTooltip()
{
    if (isEmpty()) return;
    
    QString tooltip = QString("<b>%1</b><br/>").arg(m_itemName);
    tooltip += QString("数量: %1<br/>").arg(m_itemCount);
    if (!m_itemDescription.isEmpty()) {
        tooltip += QString("描述: %1<br/>").arg(m_itemDescription);
    }
    if (!m_itemCategory.isEmpty()) {
        tooltip += QString("类别: %1<br/>").arg(m_itemCategory);
    }
    if (!m_itemRarity.isEmpty()) {
        tooltip += QString("稀有度: %1").arg(m_itemRarity);
    }
    
    setToolTip(tooltip);
}

// ===================== BackpackPanel 实现 =====================

BackpackPanel::BackpackPanel(CommandManager &commandManager, QWidget *parent)
    : QWidget(parent), m_commandManager(commandManager)
{
    setupUi();
    updateDisplay(); // 初始更新显示
}

BackpackPanel::~BackpackPanel()
{
    qDeleteAll(m_slots);
}

void BackpackPanel::setupUi()
{
    setWindowTitle("背包");
    setFixedSize(320, 400);

    // 设置窗口属性和样式
    setWindowFlags(Qt::Tool | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setStyleSheet(
        "QWidget {"
        "    background-color: #f5f5f5;"
        "    font-family: 'Microsoft YaHei', 'SimSun';"
        "}"
        "QGroupBox {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: #333333;"
        "    border: 2px solid #cccccc;"
        "    border-radius: 10px;"
        "    margin-top: 10px;"
        "    padding-top: 10px;"
        "    background-color: white;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "    color: #4CAF50;"
        "}"
        "QLabel {"
        "    color: #666666;"
        "}"
    );

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 背包网格布局
    QGroupBox *gridGroup = new QGroupBox("背包 (4×4)", this);
    m_gridLayout = new QGridLayout(gridGroup);
    m_gridLayout->setSpacing(8);  // 增加间距
    m_gridLayout->setContentsMargins(15, 20, 15, 15);

    // 创建16个物品格子
    for (int row = 0; row < GRID_SIZE; ++row)
    {
        for (int col = 0; col < GRID_SIZE; ++col)
        {
            ItemSlot *slot = new ItemSlot(this);
            m_slots.append(slot);
            m_gridLayout->addWidget(slot, row, col);

            // 使用lambda捕获索引
            connect(slot, &ItemSlot::clicked, [this, index = row * GRID_SIZE + col]()
                    { onSlotClicked(index); });
        }
    }
    gridGroup->setLayout(m_gridLayout);
    mainLayout->addWidget(gridGroup);

    // 状态标签
    m_statusLabel = new QLabel("背包已空", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 13px;"
        "    color: #888888;"
        "    background-color: #e8e8e8;"
        "    border: 1px solid #ddd;"
        "    border-radius: 5px;"
        "    padding: 8px;"
        "}"
    );
    mainLayout->addWidget(m_statusLabel);
}

void BackpackPanel::updateBackpackData(const QVector<BackpackItemInfo>& items)
{
    m_backpackItems = items;
    // 不立即更新显示，等所有数据更新完毕后再更新
}

void BackpackPanel::updateItemDisplayInfo(int itemId, const ItemDisplayInfo& displayInfo)
{
    m_itemDisplayInfos[itemId] = displayInfo;
    // 不立即更新显示，等所有数据更新完毕后再更新
}

void BackpackPanel::refreshDisplay()
{
    updateDisplay();
}

void BackpackPanel::updateDisplay()
{
    updateSlots();
}

void BackpackPanel::updateSlots()
{
    // 清空所有格子
    for (ItemSlot *slot : m_slots)
    {
        slot->clearItem();
    }

    // 填充物品
    int count = qMin(m_backpackItems.size(), m_slots.size());
    for (int i = 0; i < count; ++i)
    {
        const BackpackItemInfo &item = m_backpackItems[i];
        
        // 查找物品显示信息
        if (m_itemDisplayInfos.contains(item.itemId))
        {
            const ItemDisplayInfo &displayInfo = m_itemDisplayInfos[item.itemId];
            m_slots[i]->setItem(item, displayInfo);
        }
        else
        {
            // 如果没有显示信息，使用默认信息
            ItemDisplayInfo defaultInfo;
            defaultInfo.name = QString("物品 %1").arg(item.itemId);
            defaultInfo.iconPath = ":/resources/img/default_item.png";
            defaultInfo.description = "未知物品";
            defaultInfo.category = "未分类";
            defaultInfo.rarity = "普通";
            m_slots[i]->setItem(item, defaultInfo);
        }
    }

    // 更新状态标签
    if (m_backpackItems.isEmpty())
    {
        m_statusLabel->setText("🎒 背包空空如也，去收集一些物品吧！");
    }
    else
    {
        m_statusLabel->setText(QString("📦 物品数量: %1/%2 | 鼠标悬浮查看详情").arg(m_backpackItems.size()).arg(m_slots.size()));
    }
}

void BackpackPanel::onSlotClicked(int index)
{
    if (index < m_backpackItems.size())
    {
        const BackpackItemInfo &item = m_backpackItems[index];
        qDebug() << "背包物品被点击:" << item.itemId << "数量:" << item.count;

        // 通过命令模式发送使用物品命令
        ICommandBase *command = m_commandManager.get_command(CommandType::USE_ITEM);
        if (command)
        {
            UseItemCommandParameter param(item.itemId, 1);
            command->exec(&param);
        }
    }
}

void BackpackPanel::notification_cb(uint32_t id, void *p)
{
    BackpackPanel *panel = static_cast<BackpackPanel *>(p);
    if (!panel)
        return;

    if (id == PROP_ID_BACKPACK_UPDATE)
    {
        // 这里不再直接更新，而是通过外部调用updateBackpackData
        // panel->updateDisplay();
        qDebug() << "BackpackPanel received update notification";
    }
}
