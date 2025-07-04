#include "BackpackPanel.h"
#include "../common/PropertyIds.h"
#include "../common/CommandParameters.h"
#include <QIcon>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QDebug>

// ===================== ItemSlot 实现 =====================

ItemSlot::ItemSlot(QWidget *parent)
    : QWidget(parent), m_iconLabel(nullptr), m_countLabel(nullptr), m_itemId(0), m_itemCount(0)
{
    setupUi();
    clearItem();
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
    setFixedSize(60, 60);
    setStyleSheet("background-color: #f0f0f0; border: 1px solid #cccccc;");

    // 图标标签
    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setGeometry(5, 5, 50, 50);

    // 数量标签
    m_countLabel = new QLabel(this);
    m_countLabel->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    m_countLabel->setStyleSheet("font-size: 10px; color: #333333;");
    m_countLabel->setGeometry(30, 30, 25, 25);
}

void ItemSlot::setItem(const BackpackItemInfo &item, const QString &name, const QString &iconPath)
{
    m_itemId = item.itemId;
    m_itemCount = item.count;

    // 设置图标
    QPixmap pixmap(iconPath);
    if (!pixmap.isNull())
    {
        // 缩放图标适应格子
        pixmap = pixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_iconLabel->setPixmap(pixmap);
    }
    else
    {
        m_iconLabel->setText(iconPath);
    }

    // 设置数量
    if (m_itemCount > 1)
    {
        m_countLabel->setText(QString::number(m_itemCount));
        m_countLabel->show();
    }
    else
    {
        m_countLabel->hide();
    }

    // 设置工具提示
    setToolTip(QString("%1\n数量: %2").arg(name).arg(m_itemCount));

    // 更新样式
    setStyleSheet("background-color: #e8f4ff; border: 1px solid #a0c0e0;");
}

void ItemSlot::clearItem()
{
    m_itemId = 0;
    m_itemCount = 0;
    m_iconLabel->clear();
    m_countLabel->hide();
    setToolTip("");
    setStyleSheet("background-color: #f0f0f0; border: 1px solid #cccccc;");
}

// ===================== BackpackPanel 实现 =====================

BackpackPanel::BackpackPanel(CommandManager &command_manager, BackpackModel &backpack_model, QWidget *parent)
    : QWidget(parent),
      m_command_manager(command_manager),
      m_backpack_model(backpack_model)
{
    loadItemInfoFromCSV(":/resources/csv/item_info.txt");
    setupUi();
    updateDisplay(); // 初始更新显示
}

void BackpackPanel::loadItemInfoFromCSV(const QString &filePath)
{
    m_itemInfos.clear();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning() << "无法打开物品信息文件:" << filePath;
        setWindowTitle("无法打开物品信息文件");
        return;
    }

    QTextStream in(&file);
    bool firstLine = true;

    QString raw = in.readAll();
    QStringList lines = raw.split("\n");

    setWindowTitle(QString::number(lines.size()));
    for (int i = 0; i < lines.size(); i++)
    {
        QString &line = lines[i];
        if (line.isEmpty() || line.startsWith('#'))
        {
            continue;
        }

        // 跳过标题行
        if (firstLine)
        {
            firstLine = false;
            continue;
        }

        // 解析CSV行
        QStringList parts = line.split(',');
        if (parts.size() < 4)
        {
            qWarning() << "无效的物品信息行:" << line;
            continue;
        }

        // 提取字段
        bool ok;
        int id = parts[0].toInt(&ok);
        if (!ok)
        {
            qWarning() << "无效的物品ID:" << parts[0];
            continue;
        }

        QString name = parts[1].trimmed();
        QString desc = parts[2].trimmed();
        QString iconPath = parts[3].trimmed();

        ItemInfo info;
        info.id = id;
        info.name = name;
        info.desc = desc;
        info.iconPath = iconPath;
        // 添加到映射表
        m_itemInfos[id] = info;
    }

    file.close();
    qDebug() << "加载物品信息:" << m_itemInfos.size() << "条记录";
}

BackpackPanel::~BackpackPanel()
{
    qDeleteAll(m_slots);
}

void BackpackPanel::setupUi()
{
    setWindowTitle(windowTitle() + "背包");
    setFixedSize(300, 350);

    // 设置窗口属性
    setWindowFlags(Qt::Tool | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
    setAttribute(Qt::WA_DeleteOnClose, true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 背包网格布局
    QGroupBox *gridGroup = new QGroupBox("背包 (4x4)", this);
    m_gridLayout = new QGridLayout(gridGroup);
    m_gridLayout->setSpacing(5);
    m_gridLayout->setContentsMargins(10, 10, 10, 10);

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
    m_statusLabel->setStyleSheet("font-size: 12px; color: #666666;");
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

    // 获取背包物品列表
    const QVector<BackpackItemInfo> &items = m_backpack_model.getItems();

    // 填充物品
    int count = qMin(items.size(), m_slots.size());
    for (int i = 0; i < count; ++i)
    {
        const BackpackItemInfo &item = items[i];
        QString name, iconPath;
        getItemInfo(item.itemId, name, iconPath);
        m_slots[i]->setItem(item, name, iconPath);
    }

    // 更新状态标签
    if (items.isEmpty())
    {
        m_statusLabel->setText("背包已空");
    }
    else
    {
        m_statusLabel->setText(QString("物品数量: %1/%2").arg(items.size()).arg(m_slots.size()));
    }
}

void BackpackPanel::getItemInfo(int itemId, QString &name, QString &iconPath) const
{
    // 检查是否在映射表中
    if (m_itemInfos.contains(itemId))
    {
        auto info = m_itemInfos[itemId];
        name = info.name;
        iconPath = info.iconPath;
    }
    else
    {
        // 未找到时的默认值
        name = QString("未知物品 #%1").arg(itemId);
        iconPath = ":/resources/img/Test1.png";
    }
}

void BackpackPanel::onSlotClicked(int index)
{
    if (index < m_backpack_model.getItems().size())
    {
        const BackpackItemInfo &item = m_backpack_model.getItems()[index];
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