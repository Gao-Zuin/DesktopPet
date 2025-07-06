#include "WorkUpgradePanel.h"
#include "../viewmodel/PetViewModel.h"
#include "../model/ForgeModel.h"
#include "../model/WorkModel.h"
#include "../model/BackpackModel.h"
#include "../common/CommandParameters.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QFrame>
#include <QGroupBox>
#include <QMessageBox>
#include <QDebug>

// ============ WorkSystemUpgradeCard 实现 ============
WorkSystemUpgradeCard::WorkSystemUpgradeCard(WorkType workType, PetViewModel *viewModel, QWidget *parent)
    : QFrame(parent), m_workType(workType), m_viewModel(viewModel)
{
    setupUI();
    updateDisplay();
}

void WorkSystemUpgradeCard::setupUI()
{
    setFrameStyle(QFrame::StyledPanel);
    setStyleSheet("QFrame { border: 2px solid #4CAF50; border-radius: 12px; padding: 15px; margin: 8px; background-color: #f8fff8; }");
    setMinimumHeight(300); // 减少高度
    setMaximumHeight(350); // 减少高度

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12); // 增加间距

    // 工作类型标题 - 增加高度
    m_titleLabel = new QLabel(getWorkTypeName(m_workType));
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 20px; color: #2E7D32; padding: 15px; "
                                "background-color: #E8F5E8; border-radius: 8px;");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setMinimumHeight(60); // 增加标题高度
    mainLayout->addWidget(m_titleLabel);

    // 当前等级显示 - 增加高度
    m_currentLevelLabel = new QLabel();
    m_currentLevelLabel->setStyleSheet("font-size: 16px; color: #1976D2; padding: 12px; "
                                       "background-color: #E3F2FD; border-radius: 6px;");
    m_currentLevelLabel->setAlignment(Qt::AlignCenter);
    m_currentLevelLabel->setMinimumHeight(45); // 增加高度
    mainLayout->addWidget(m_currentLevelLabel);

    // 删除升级收益显示的文本框，直接删除这部分代码

    // 所需材料 - 改为简单的文本显示
    m_materialsGroup = new QGroupBox("升级所需材料");
    m_materialsGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #5D4037; padding: 8px; font-size: 14px; } "
                                    "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 8px 0 8px; }");

    // 使用简单的文本标签而不是复杂的滚动区域
    m_materialsTextLabel = new QLabel("正在加载材料信息...");
    m_materialsTextLabel->setStyleSheet("color: #333; font-size: 12px; padding: 10px; "
                                        "background-color: #FAFAFA; border-radius: 4px; "
                                        "border: 1px solid #E0E0E0;");
    m_materialsTextLabel->setWordWrap(true);
    m_materialsTextLabel->setMinimumHeight(60); // 设置最小高度
    m_materialsTextLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QVBoxLayout *groupLayout = new QVBoxLayout(m_materialsGroup);
    groupLayout->addWidget(m_materialsTextLabel);

    mainLayout->addWidget(m_materialsGroup);

    // 升级按钮 - 增加高度
    m_upgradeButton = new QPushButton("升级");
    m_upgradeButton->setStyleSheet(
        "QPushButton { "
        "background-color: #4CAF50; color: white; font-weight: bold; "
        "padding: 15px; border-radius: 8px; font-size: 16px; min-height: 25px; } "
        "QPushButton:hover { background-color: #45a049; } "
        "QPushButton:disabled { background-color: #ccc; color: #666; }");
    connect(m_upgradeButton, &QPushButton::clicked, this, &WorkSystemUpgradeCard::onUpgradeClicked);
    mainLayout->addWidget(m_upgradeButton);

    mainLayout->addStretch();
}

void WorkSystemUpgradeCard::updateDisplay()
{
    updateBenefits();
    updateMaterials();
}

void WorkSystemUpgradeCard::updateBenefits()
{
    if (!m_viewModel || !m_viewModel->get_work_model())
    {
        return;
    }

    WorkModel *workModel = m_viewModel->get_work_model();
    WorkSystemLevel currentLevel = workModel->getWorkSystemLevel(m_workType);

    // 更新当前等级显示
    m_currentLevelLabel->setText(QString("当前等级: %1").arg(getLevelName(currentLevel)));

    // 不再使用已删除的 m_benefitsLabel，所有升级效果信息都通过材料显示区域显示
}

void WorkSystemUpgradeCard::updateMaterials()
{
    if (!m_viewModel || !m_viewModel->getForgeModel() || !m_viewModel->get_backpack_model())
    {
        m_upgradeButton->setEnabled(false);
        m_upgradeButton->setText("数据加载中...");
        m_upgradeButton->setStyleSheet("QPushButton { background-color: #ccc; color: #666; padding: 15px; border-radius: 8px; }");

        m_materialsTextLabel->setText("正在加载材料数据...");
        m_materialsTextLabel->setStyleSheet("color: #999; font-style: italic; padding: 10px;");
        return;
    }

    WorkModel *workModel = m_viewModel->get_work_model();
    ForgeModel *forgeModel = m_viewModel->getForgeModel();
    BackpackModel *backpackModel = m_viewModel->get_backpack_model();

    if (!workModel)
    {
        m_materialsTextLabel->setText("❌ 工作系统未初始化");
        m_materialsTextLabel->setStyleSheet("color: #FF5722; font-weight: bold; padding: 10px;");
        m_upgradeButton->setEnabled(false);
        m_upgradeButton->setText("系统错误");
        return;
    }

    WorkSystemLevel currentLevel = workModel->getWorkSystemLevel(m_workType);

    // 检查是否已达到最高等级
    if (currentLevel >= WorkSystemLevel::Master)
    {
        m_materialsTextLabel->setText("🏆 已达到最高等级！无需更多材料。");
        m_materialsTextLabel->setStyleSheet("color: #FF9800; font-weight: bold; font-size: 14px; padding: 10px; "
                                            "background-color: #FFF3E0; border-radius: 4px;");

        m_upgradeButton->setEnabled(false);
        m_upgradeButton->setText("🏆 已达最高等级");
        m_upgradeButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; font-weight: bold; "
                                       "padding: 15px; border-radius: 8px; font-size: 16px; }");
        return;
    }

    // 获取可用的升级配方
    auto availableUpgrades = forgeModel->getAvailableWorkUpgrades();
    WorkSystemUpgrade targetUpgrade;
    bool foundUpgrade = false;

    for (const auto &upgrade : availableUpgrades)
    {
        if (upgrade.workType == m_workType)
        {
            targetUpgrade = upgrade;
            foundUpgrade = true;
            break;
        }
    }

    if (!foundUpgrade)
    {
        m_materialsTextLabel->setText("🔒 暂无可用升级配方");
        m_materialsTextLabel->setStyleSheet("color: #757575; font-style: italic; padding: 10px; "
                                            "background-color: #FAFAFA; border: 1px dashed #BDBDBD; border-radius: 4px;");

        m_upgradeButton->setEnabled(false);
        m_upgradeButton->setText("🔒 暂无升级配方");
        m_upgradeButton->setStyleSheet("QPushButton { background-color: #BDBDBD; color: #757575; font-weight: bold; "
                                       "padding: 15px; border-radius: 8px; font-size: 16px; }");
        return;
    }

    // 构建材料需求文本
    QString materialsText = "升级所需材料:\n";
    bool canUpgrade = true;

    if (targetUpgrade.upgradeMaterials.isEmpty())
    {
        materialsText = "🎉 免费升级！无需任何材料。";
        m_materialsTextLabel->setStyleSheet("color: #4CAF50; font-weight: bold; padding: 10px; "
                                            "background-color: #E8F5E8; border-radius: 4px;");
    }
    else
    {
        for (const auto &material : targetUpgrade.upgradeMaterials)
        {
            int ownedCount = backpackModel->getItemCount(material.itemId);
            QString itemName = backpackModel->getItemName(material.itemId);

            // 添加图标
            QString iconText = "📦";
            if (itemName.contains("阳光"))
                iconText = "☀️";
            else if (itemName.contains("矿石"))
                iconText = "⛏️";
            else if (itemName.contains("木"))
                iconText = "🌳";
            else if (itemName.contains("稀有"))
                iconText = "💎";
            else if (itemName.contains("传说"))
                iconText = "🌟";

            materialsText += QString("• %1 %2: %3/%4").arg(iconText).arg(itemName).arg(ownedCount).arg(material.requiredCount);

            if (material.isCatalyst)
            {
                materialsText += " (催化剂)";
            }

            if (ownedCount >= material.requiredCount)
            {
                materialsText += " ✅";
            }
            else
            {
                materialsText += " ❌";
                canUpgrade = false;
            }

            materialsText += "\n";
        }

        // 设置材料文本样式
        if (canUpgrade)
        {
            m_materialsTextLabel->setStyleSheet("color: #333; font-size: 12px; padding: 10px; "
                                                "background-color: #F1F8E9; border: 1px solid #4CAF50; border-radius: 4px;");
        }
        else
        {
            m_materialsTextLabel->setStyleSheet("color: #333; font-size: 12px; padding: 10px; "
                                                "background-color: #FFEBEE; border: 1px solid #F44336; border-radius: 4px;");
        }
    }

    m_materialsTextLabel->setText(materialsText);

    // 更新升级按钮状态
    m_upgradeButton->setEnabled(canUpgrade);
    if (canUpgrade)
    {
        m_upgradeButton->setText("🚀 立即升级");
        m_upgradeButton->setStyleSheet(
            "QPushButton { "
            "background-color: #4CAF50; color: white; font-weight: bold; "
            "padding: 15px; border-radius: 8px; font-size: 16px; } "
            "QPushButton:hover { background-color: #45a049; }");
    }
    else
    {
        m_upgradeButton->setText("❌ 材料不足");
        m_upgradeButton->setStyleSheet(
            "QPushButton { "
            "background-color: #E0E0E0; color: #757575; font-weight: bold; "
            "padding: 15px; border-radius: 8px; font-size: 16px; border: 2px solid #BDBDBD; }");
    }
}

bool WorkSystemUpgradeCard::canUpgrade() const
{
    if (!m_viewModel || !m_viewModel->getForgeModel())
    {
        return false;
    }
    return m_viewModel->getForgeModel()->canUpgradeWorkSystem(m_workType);
}

void WorkSystemUpgradeCard::onUpgradeClicked()
{
    if (canUpgrade())
    {
        emit upgradeRequested(m_workType);
    }
}

QString WorkSystemUpgradeCard::getWorkTypeName(WorkType workType) const
{
    switch (workType)
    {
    case WorkType::Photosynthesis:
        return "🌱 光合作用";
    case WorkType::Mining:
        return "⛏️ 挖矿";
    case WorkType::Adventure:
        return "🌲 冒险探索";
    default:
        return "未知工作";
    }
}

QString WorkSystemUpgradeCard::getLevelName(WorkSystemLevel level) const
{
    switch (level)
    {
    case WorkSystemLevel::Basic:
        return "基础级";
    case WorkSystemLevel::Advanced:
        return "进阶级";
    case WorkSystemLevel::Expert:
        return "专家级";
    case WorkSystemLevel::Master:
        return "大师级";
    default:
        return "未知等级";
    }
}

// ============ WorkUpgradePanel 实现 ============
WorkUpgradePanel::WorkUpgradePanel(PetViewModel *viewModel, CommandManager &commandManager, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel), m_commandManager(commandManager)
{
    setupUI();
    connectSignals();
    refreshAll();
}

WorkUpgradePanel::~WorkUpgradePanel()
{
}

void WorkUpgradePanel::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
    qDebug() << "WorkUpgradePanel closed (hidden)";
}

void WorkUpgradePanel::setupUI()
{
    setWindowTitle("打工系统升级");
    setFixedSize(1000, 800); // 增加窗口宽度和高度

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15); // 增加间距

    // 标题
    QLabel *titleLabel = new QLabel("🔧 打工系统升级中心");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 24px; color: #1976D2; padding: 15px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFixedHeight(60); // 设置固定高度
    mainLayout->addWidget(titleLabel);

    // 概览区域
    setupOverview();
    mainLayout->addWidget(m_overviewWidget);

    // 升级卡片区域
    setupUpgradeCards();

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(m_upgradeCardsWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setMinimumHeight(500); // 设置滚动区域最小高度
    mainLayout->addWidget(scrollArea);

    // 关闭按钮
    QPushButton *closeButton = new QPushButton("关闭");
    closeButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 12px 30px; border-radius: 6px; font-weight: bold; font-size: 14px; }");
    closeButton->setFixedHeight(40);
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
    mainLayout->addWidget(closeButton);
}

void WorkUpgradePanel::setupOverview()
{
    m_overviewWidget = new QWidget();
    m_overviewWidget->setStyleSheet("QWidget { background-color: #e3f2fd; border: 2px solid #1976D2; border-radius: 10px; margin: 5px; }");
    m_overviewWidget->setMinimumHeight(120); // 设置概览区域最小高度
    m_overviewWidget->setMaximumHeight(150); // 设置概览区域最大高度

    QVBoxLayout *overviewLayout = new QVBoxLayout(m_overviewWidget);
    overviewLayout->setSpacing(10); // 增加间距

    QLabel *overviewTitle = new QLabel("📊 当前工作系统等级概览");
    overviewTitle->setStyleSheet("font-weight: bold; font-size: 18px; color: #0D47A1; padding: 10px;");
    overviewTitle->setAlignment(Qt::AlignCenter);
    overviewTitle->setFixedHeight(50); // 设置标题固定高度
    overviewLayout->addWidget(overviewTitle);

    QGridLayout *levelsGrid = new QGridLayout();
    levelsGrid->setSpacing(15); // 增加网格间距

    // 光合作用等级
    QLabel *photoLabel = new QLabel("🌱 光合作用:");
    photoLabel->setStyleSheet("font-weight: bold; color: #2E7D32; font-size: 14px; padding: 5px;");
    m_photosynthesisLevelLabel = new QLabel("基础级");
    m_photosynthesisLevelLabel->setStyleSheet("color: #1976D2; font-weight: bold; font-size: 14px; padding: 5px; "
                                              "background-color: #E8F5E8; border-radius: 4px; min-height: 25px;");
    m_photosynthesisLevelLabel->setAlignment(Qt::AlignCenter);
    levelsGrid->addWidget(photoLabel, 0, 0);
    levelsGrid->addWidget(m_photosynthesisLevelLabel, 0, 1);

    // 挖矿等级
    QLabel *miningLabel = new QLabel("⛏️ 挖矿:");
    miningLabel->setStyleSheet("font-weight: bold; color: #5D4037; font-size: 14px; padding: 5px;");
    m_miningLevelLabel = new QLabel("基础级");
    m_miningLevelLabel->setStyleSheet("color: #1976D2; font-weight: bold; font-size: 14px; padding: 5px; "
                                      "background-color: #FFF3E0; border-radius: 4px; min-height: 25px;");
    m_miningLevelLabel->setAlignment(Qt::AlignCenter);
    levelsGrid->addWidget(miningLabel, 0, 2);
    levelsGrid->addWidget(m_miningLevelLabel, 0, 3);

    // 冒险等级
    QLabel *adventureLabel = new QLabel("🌲 冒险探索:");
    adventureLabel->setStyleSheet("font-weight: bold; color: #FF8F00; font-size: 14px; padding: 5px;");
    m_adventureLevelLabel = new QLabel("基础级");
    m_adventureLevelLabel->setStyleSheet("color: #1976D2; font-weight: bold; font-size: 14px; padding: 5px; "
                                         "background-color: #E3F2FD; border-radius: 4px; min-height: 25px;");
    m_adventureLevelLabel->setAlignment(Qt::AlignCenter);
    levelsGrid->addWidget(adventureLabel, 0, 4);
    levelsGrid->addWidget(m_adventureLevelLabel, 0, 5);

    overviewLayout->addLayout(levelsGrid);
}

void WorkUpgradePanel::setupUpgradeCards()
{
    m_upgradeCardsWidget = new QWidget();

    // 使用水平布局而不是垂直布局，让三个卡片并排显示
    QHBoxLayout *cardsLayout = new QHBoxLayout(m_upgradeCardsWidget);
    cardsLayout->setSpacing(20);                     // 增加卡片间距
    cardsLayout->setContentsMargins(20, 20, 20, 20); // 增加边距

    // 创建三个工作系统的升级卡片
    QVector<WorkType> workTypes = {WorkType::Photosynthesis, WorkType::Mining, WorkType::Adventure};

    for (WorkType workType : workTypes)
    {
        WorkSystemUpgradeCard *card = new WorkSystemUpgradeCard(workType, m_viewModel, this);
        card->setMinimumWidth(300); // 设置卡片最小宽度
        card->setMaximumWidth(350); // 设置卡片最大宽度
        connect(card, &WorkSystemUpgradeCard::upgradeRequested, this, &WorkUpgradePanel::onUpgradeRequested);
        cardsLayout->addWidget(card);
        m_upgradeCards.append(card);
    }

    // 不需要 addStretch，因为我们使用固定宽度
}

void WorkUpgradePanel::connectSignals()
{
    if (m_viewModel && m_viewModel->getForgeModel())
    {
        connect(m_viewModel->getForgeModel(), &ForgeModel::workSystemUpgraded,
                this, &WorkUpgradePanel::onWorkSystemUpgraded);
    }
}

void WorkUpgradePanel::refreshAll()
{
    updateOverview();

    // 刷新所有升级卡片
    for (auto *card : m_upgradeCards)
    {
        card->updateDisplay();
    }
}

void WorkUpgradePanel::updateOverview()
{
    if (!m_viewModel || !m_viewModel->get_work_model())
    {
        return;
    }

    WorkModel *workModel = m_viewModel->get_work_model();

    // 更新等级显示
    WorkSystemLevel photoLevel = workModel->getWorkSystemLevel(WorkType::Photosynthesis);
    WorkSystemLevel miningLevel = workModel->getWorkSystemLevel(WorkType::Mining);
    WorkSystemLevel adventureLevel = workModel->getWorkSystemLevel(WorkType::Adventure);

    m_photosynthesisLevelLabel->setText(getLevelName(photoLevel));
    m_miningLevelLabel->setText(getLevelName(miningLevel));
    m_adventureLevelLabel->setText(getLevelName(adventureLevel));
}

QString WorkUpgradePanel::getLevelName(WorkSystemLevel level) const
{
    switch (level)
    {
    case WorkSystemLevel::Basic:
        return "基础级";
    case WorkSystemLevel::Advanced:
        return "进阶级";
    case WorkSystemLevel::Expert:
        return "专家级";
    case WorkSystemLevel::Master:
        return "大师级";
    default:
        return "未知等级";
    }
}

void WorkUpgradePanel::onUpgradeRequested(WorkType workType)
{
    if (!m_viewModel || !m_viewModel->get_work_model() || !m_viewModel->getForgeModel())
    {
        QMessageBox::warning(this, "错误", "系统数据未加载完成！");
        return;
    }

    // 获取当前等级和目标等级
    WorkModel *workModel = m_viewModel->get_work_model();
    WorkSystemLevel currentLevel = workModel->getWorkSystemLevel(workType);
    WorkSystemLevel targetLevel = static_cast<WorkSystemLevel>(static_cast<int>(currentLevel) + 1);

    if (targetLevel > WorkSystemLevel::Master)
    {
        QMessageBox::information(this, "提示", "该工作系统已达到最高等级！");
        return;
    }

    // 执行升级命令
    ICommandBase *command = m_commandManager.get_command(CommandType::FORGE);
    if (command)
    {
        ForgeCommandParameter param(workType, targetLevel);
        command->exec(&param);
    }
    else
    {
        QMessageBox::warning(this, "错误", "无法找到升级命令！");
    }
}

void WorkUpgradePanel::onWorkSystemUpgraded(WorkType workType, WorkSystemLevel newLevel)
{
    Q_UNUSED(workType)
    Q_UNUSED(newLevel)

    // 升级成功，刷新界面
    refreshAll();

    // 显示升级成功消息
    QString workTypeName;
    switch (workType)
    {
    case WorkType::Photosynthesis:
        workTypeName = "光合作用";
        break;
    case WorkType::Mining:
        workTypeName = "挖矿";
        break;
    case WorkType::Adventure:
        workTypeName = "冒险探索";
        break;
    default:
        workTypeName = "未知工作";
        break;
    }

    QString levelName = getLevelName(newLevel);
    QMessageBox::information(this, "升级成功",
                             QString("🎉 %1 系统已成功升级到 %2！").arg(workTypeName).arg(levelName));
}