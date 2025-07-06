#include "ItemSelectionDialog.h"
#include "../viewmodel/PetViewModel.h"
#include "../model/BackpackModel.h"
#include "../model/CollectionModel.h"
#include <QApplication>
#include <QGridLayout>
#include <QCheckBox>
#include <QMessageBox>
#include <QDebug>

// ============ ItemSelectionWidget 实现 ============
ItemSelectionWidget::ItemSelectionWidget(int itemId, const QString &itemName, int ownedCount, QWidget *parent)
    : QWidget(parent), m_itemId(itemId), m_itemName(itemName), m_ownedCount(ownedCount), m_maxSelectableCount(ownedCount), m_selected(false)
{
    setupUI();
}

void ItemSelectionWidget::setupUI()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(8, 4, 8, 4);

    // 复选框
    m_checkBox = new QCheckBox();
    connect(m_checkBox, &QCheckBox::toggled, this, &ItemSelectionWidget::onSelectionChanged);
    layout->addWidget(m_checkBox);

    // 物品名称
    m_nameLabel = new QLabel(m_itemName);
    m_nameLabel->setMinimumWidth(150);
    layout->addWidget(m_nameLabel);

    // 拥有数量
    m_countLabel = new QLabel(QString("拥有: %1").arg(m_ownedCount));
    m_countLabel->setMinimumWidth(80);
    layout->addWidget(m_countLabel);

    // 选择数量
    m_countSpinBox = new QSpinBox();
    m_countSpinBox->setMinimum(0);
    m_countSpinBox->setMaximum(m_ownedCount);
    m_countSpinBox->setValue(0);
    m_countSpinBox->setEnabled(false);
    connect(m_countSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ItemSelectionWidget::onSelectionChanged);
    layout->addWidget(m_countSpinBox);

    layout->addStretch();

    setStyleSheet("ItemSelectionWidget { border: 1px solid #ddd; border-radius: 4px; margin: 2px; background-color: #f9f9f9; }");
}

void ItemSelectionWidget::onSelectionChanged()
{
    bool isChecked = m_checkBox->isChecked();
    m_selected = isChecked;
    m_countSpinBox->setEnabled(isChecked);

    if (!isChecked)
    {
        m_countSpinBox->setValue(0);
    }
    else if (m_countSpinBox->value() == 0)
    {
        m_countSpinBox->setValue(1);
    }

    updateUI();
    emit selectionChanged();
}

void ItemSelectionWidget::updateUI()
{
    if (m_selected)
    {
        setStyleSheet("ItemSelectionWidget { border: 2px solid #4CAF50; border-radius: 4px; margin: 2px; background-color: #e8f5e8; }");
    }
    else
    {
        setStyleSheet("ItemSelectionWidget { border: 1px solid #ddd; border-radius: 4px; margin: 2px; background-color: #f9f9f9; }");
    }
}

int ItemSelectionWidget::getSelectedCount() const
{
    return m_selected ? m_countSpinBox->value() : 0;
}

void ItemSelectionWidget::setMaxSelectableCount(int maxCount)
{
    m_maxSelectableCount = qMin(maxCount, m_ownedCount);
    m_countSpinBox->setMaximum(m_maxSelectableCount);
}

// ============ ItemSelectionDialog 实现 ============
ItemSelectionDialog::ItemSelectionDialog(PetViewModel *viewModel, ItemQuality targetQuality, int requiredCount, QWidget *parent)
    : QDialog(parent), m_viewModel(viewModel), m_targetQuality(targetQuality), m_requiredCount(requiredCount), m_currentSelectedCount(0)
{
    setupUI();
    loadAvailableItems();
    updateConfirmButton();
}

ItemSelectionDialog::~ItemSelectionDialog()
{
}

void ItemSelectionDialog::setupUI()
{
    setWindowTitle("选择要消耗的物品");
    setModal(true);
    resize(500, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 信息标签
    QString qualityName;
    switch (m_targetQuality)
    {
    case ItemQuality::Common:
        qualityName = "普通";
        break;
    case ItemQuality::Uncommon:
        qualityName = "稀有";
        break;
    case ItemQuality::Rare:
        qualityName = "史诗";
        break;
    case ItemQuality::Epic:
        qualityName = "传说";
        break;
    default:
        qualityName = "未知";
        break;
    }

    m_infoLabel = new QLabel(QString("请选择 %1 个 %2 品质的物品进行强化:").arg(m_requiredCount).arg(qualityName));
    m_infoLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #333; padding: 10px;");
    mainLayout->addWidget(m_infoLabel);

    // 当前选择计数
    m_selectedCountLabel = new QLabel("已选择: 0 / " + QString::number(m_requiredCount));
    m_selectedCountLabel->setStyleSheet("color: #666; padding: 5px;");
    mainLayout->addWidget(m_selectedCountLabel);

    // 滚动区域
    m_scrollArea = new QScrollArea();
    m_scrollWidget = new QWidget();
    m_scrollLayout = new QVBoxLayout(m_scrollWidget);
    m_scrollLayout->setAlignment(Qt::AlignTop);

    m_scrollArea->setWidget(m_scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mainLayout->addWidget(m_scrollArea);

    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_cancelButton = new QPushButton("取消");
    m_cancelButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px 16px; border-radius: 4px; font-weight: bold; }");
    connect(m_cancelButton, &QPushButton::clicked, this, &ItemSelectionDialog::onCancelClicked);
    buttonLayout->addWidget(m_cancelButton);

    m_confirmButton = new QPushButton("确认");
    m_confirmButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px 16px; border-radius: 4px; font-weight: bold; }");
    connect(m_confirmButton, &QPushButton::clicked, this, &ItemSelectionDialog::onConfirmClicked);
    buttonLayout->addWidget(m_confirmButton);

    mainLayout->addLayout(buttonLayout);
}

void ItemSelectionDialog::loadAvailableItems()
{
    if (!m_viewModel || !m_viewModel->get_backpack_model() || !m_viewModel->get_collection_model())
    {
        return;
    }

    BackpackModel *backpackModel = m_viewModel->get_backpack_model();
    CollectionModel *collectionModel = m_viewModel->get_collection_model();

    const auto &backpackItems = backpackModel->getItems();

    for (const auto &backpackItem : backpackItems)
    {
        int itemId = backpackItem.itemId;
        int count = backpackItem.count;

        if (count <= 0)
            continue;

        // 获取物品信息
        CollectionItemInfo itemInfo = collectionModel->getItemInfo(itemId);
        if (itemInfo.id == 0)
            continue; // 物品不存在

        // 将 CollectionRarity 正确映射到 ItemQuality
        ItemQuality itemQuality;
        switch (itemInfo.rarity)
        {
        case CollectionRarity::Common:
            itemQuality = ItemQuality::Common;
            break;
        case CollectionRarity::Rare:
            itemQuality = ItemQuality::Uncommon; // 稀有对应Uncommon
            break;
        case CollectionRarity::Epic:
            itemQuality = ItemQuality::Rare; // 史诗对应Rare
            break;
        case CollectionRarity::Legendary:
            itemQuality = ItemQuality::Epic; // 传说对应Epic
            break;
        default:
            continue; // 跳过未知品质
        }

        if (itemQuality != m_targetQuality)
            continue;

        // 排除阳光材料（ID 6-10）
        if (itemId >= 6 && itemId <= 10)
            continue;

        // 创建选择项
        ItemSelectionWidget *selectionWidget = new ItemSelectionWidget(itemId, itemInfo.name, count, this);
        connect(selectionWidget, &ItemSelectionWidget::selectionChanged, this, &ItemSelectionDialog::onSelectionChanged);

        m_selectionWidgets.append(selectionWidget);
        m_scrollLayout->addWidget(selectionWidget);
    }

    if (m_selectionWidgets.isEmpty())
    {
        QLabel *noItemsLabel = new QLabel("没有符合条件的物品可供选择");
        noItemsLabel->setStyleSheet("color: #999; text-align: center; padding: 20px;");
        noItemsLabel->setAlignment(Qt::AlignCenter);
        m_scrollLayout->addWidget(noItemsLabel);
    }
}

void ItemSelectionDialog::onSelectionChanged()
{
    m_currentSelectedCount = 0;

    for (ItemSelectionWidget *widget : m_selectionWidgets)
    {
        m_currentSelectedCount += widget->getSelectedCount();
    }

    m_selectedCountLabel->setText(QString("已选择: %1 / %2").arg(m_currentSelectedCount).arg(m_requiredCount));

    // 更新确认按钮状态
    updateConfirmButton();
}

void ItemSelectionDialog::updateConfirmButton()
{
    bool canConfirm = (m_currentSelectedCount == m_requiredCount);
    m_confirmButton->setEnabled(canConfirm);

    if (canConfirm)
    {
        m_confirmButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px 16px; border-radius: 4px; font-weight: bold; }");
    }
    else
    {
        m_confirmButton->setStyleSheet("QPushButton { background-color: #ccc; color: #666; padding: 8px 16px; border-radius: 4px; font-weight: bold; }");
    }
}

QVector<QPair<int, int>> ItemSelectionDialog::getSelectedItems() const
{
    QVector<QPair<int, int>> selectedItems;

    for (ItemSelectionWidget *widget : m_selectionWidgets)
    {
        int count = widget->getSelectedCount();
        if (count > 0)
        {
            selectedItems.append(qMakePair(widget->getItemId(), count));
        }
    }

    return selectedItems;
}

void ItemSelectionDialog::onConfirmClicked()
{
    if (m_currentSelectedCount != m_requiredCount)
    {
        QMessageBox::warning(this, "选择不完整",
                             QString("请选择恰好 %1 个物品！当前已选择: %2").arg(m_requiredCount).arg(m_currentSelectedCount));
        return;
    }

    accept();
}

void ItemSelectionDialog::onCancelClicked()
{
    reject();
}
