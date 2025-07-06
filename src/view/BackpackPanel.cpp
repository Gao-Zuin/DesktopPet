#include "BackpackPanel.h"
#include "../viewmodel/PetViewModel.h"
#include "../common/PropertyIds.h"
#include "../common/CommandParameters.h"
#include <QIcon>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTextStream>

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
    // 设置格子样式 - 增加大小以容纳数字框
    setFixedSize(70, 80);
    setStyleSheet("ItemSlot { background-color: #f8f8f8; border: 2px solid #ddd; border-radius: 8px; } ItemSlot:hover { border-color: #4CAF50; background-color: #f0f8ff; }");

    // 图标标签 - 调整位置给数字框留出空间
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

void ItemSlot::setItem(const BackpackItemInfo &item, const QString &name, const QString &iconPath, const QString &description, const QString &category, const QString &rarity)
{
    m_itemId = item.itemId;
    m_itemCount = item.count;
    m_itemName = name;
    m_itemDescription = description;
    m_itemCategory = category;
    m_itemRarity = rarity;

    // 设置图标
    QPixmap pixmap(iconPath);
    if (!pixmap.isNull())
    {
        // 缩放图标适应格子 - 调整大小以配合新的布局
        pixmap = pixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_iconLabel->setPixmap(pixmap);
    }
    else
    {
        m_iconLabel->setText(iconPath);
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

BackpackPanel::BackpackPanel(CommandManager &command_manager, PetViewModel &view_model, QWidget *parent)
    : QWidget(parent),
      m_command_manager(command_manager),
      m_view_model(view_model)
{
    // 不再需要加载item_info.txt，直接从图鉴系统获取物品信息
    setupUi();
    updateDisplay(); // 初始更新显示
}

void BackpackPanel::getItemInfo(int itemId, QString &name, QString &iconPath) const
{
    // 从背包模型获取物品信息（背包模型会从图鉴系统获取）
    auto backpackModel = m_view_model.get_backpack_model();
    if (backpackModel) {
        name = backpackModel->getItemName(itemId);
        iconPath = backpackModel->getItemIcon(itemId);
        return;
    }
    
    // 如果背包模型不可用，使用默认值
    name = QString("物品 %1").arg(itemId);
    iconPath = ":/resources/img/default_item.png";
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

    // 通过ViewModel获取背包物品列表
    const QVector<BackpackItemInfo> &items = m_view_model.get_backpack_items();

    // 填充物品
    int count = qMin(items.size(), m_slots.size());
    for (int i = 0; i < count; ++i)
    {
        const BackpackItemInfo &item = items[i];
        QString name, iconPath;
        getItemInfo(item.itemId, name, iconPath);
        
        // 从背包模型获取详细信息
        auto backpackModel = m_view_model.get_backpack_model();
        QString description, category, rarity;
        if (backpackModel) {
            description = backpackModel->getItemDescription(item.itemId);
            
            // 获取类别和稀有度的中文名称
            CollectionCategory cat = backpackModel->getItemCategory(item.itemId);
            CollectionRarity rare = backpackModel->getItemRarity(item.itemId);
            
            switch (cat) {
                case CollectionCategory::Material: category = "材料"; break;
                case CollectionCategory::Item: category = "物品"; break;
                case CollectionCategory::Skin: category = "皮肤"; break;
                case CollectionCategory::Achievement: category = "成就"; break;
            }
            
            switch (rare) {
                case CollectionRarity::Common: rarity = "普通"; break;
                case CollectionRarity::Rare: rarity = "稀有"; break;
                case CollectionRarity::Epic: rarity = "史诗"; break;
                case CollectionRarity::Legendary: rarity = "传说"; break;
            }
        }
        
        m_slots[i]->setItem(item, name, iconPath, description, category, rarity);
    }

    // 更新状态标签
    if (items.isEmpty())
    {
        m_statusLabel->setText("🎒 背包空空如也，去收集一些物品吧！");
    }
    else
    {
        m_statusLabel->setText(QString("📦 物品数量: %1/%2 | 鼠标悬浮查看详情").arg(items.size()).arg(m_slots.size()));
    }
}

void BackpackPanel::onSlotClicked(int index)
{
    // 通过ViewModel获取背包物品列表
    const QVector<BackpackItemInfo> &items = m_view_model.get_backpack_items();

    if (index < items.size())
    {
        const BackpackItemInfo &item = items[index];
        qDebug() << "背包物品被点击:" << item.itemId << "数量:" << item.count;

        // 发送使用物品命令
        ICommandBase *command = m_command_manager.get_command(CommandType::USE_ITEM);
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
        panel->updateDisplay();
    }
}