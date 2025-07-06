#include "ForgePanel.h"
#include "../viewmodel/PetViewModel.h"
#include "../model/ForgeModel.h"
#include "../model/BackpackModel.h"
#include "../common/CommandParameters.h"
#include "ItemSelectionDialog.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QTabWidget>
#include <QFrame>
#include <QGroupBox>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QDebug>

// ============ RecipeCard 实现 ============
RecipeCard::RecipeCard(const ForgeRecipe &recipe, PetViewModel *viewModel, QWidget *parent)
    : QFrame(parent), m_recipe(recipe), m_viewModel(viewModel)
{
    setupUI();
    updateMaterials();
}

void RecipeCard::setupUI()
{
    setFrameStyle(QFrame::StyledPanel);
    setStyleSheet("QFrame { border: 1px solid #ccc; border-radius: 8px; padding: 8px; margin: 4px; background-color: #f9f9f9; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 配方名称
    m_nameLabel = new QLabel(m_recipe.name);
    m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #333;");
    mainLayout->addWidget(m_nameLabel);

    // 配方描述
    m_descriptionLabel = new QLabel(m_recipe.description);
    m_descriptionLabel->setStyleSheet("color: #666; font-size: 12px;");
    m_descriptionLabel->setWordWrap(true);
    mainLayout->addWidget(m_descriptionLabel);

    // 所需材料
    QGroupBox *materialsGroup = new QGroupBox("所需材料");
    m_materialsLayout = new QVBoxLayout(materialsGroup);
    mainLayout->addWidget(materialsGroup);

    // 产出物品
    QString outputText = "产出: ";
    for (const auto &output : m_recipe.outputs)
    {
        // 获取产出物品名称 - 特殊处理通用物品
        QString outputItemName;
        if (output.itemId == 0)
        {
            // 通用物品，根据配方类型显示特殊名称
            if (m_recipe.recipeId == 5)
            {
                outputItemName = "1个随机稀有品质物品";
            }
            else if (m_recipe.recipeId == 6)
            {
                outputItemName = "1个随机史诗品质物品";
            }
            else if (m_recipe.recipeId == 7)
            {
                outputItemName = "1个随机传说品质物品";
            }
            else
            {
                outputItemName = "随机物品";
            }
            outputText += QString("%1 ").arg(outputItemName);
        }
        else
        {
            // 具体物品
            if (m_viewModel && m_viewModel->get_backpack_model())
            {
                outputItemName = m_viewModel->get_backpack_model()->getItemName(output.itemId);
            }
            else
            {
                outputItemName = "未知物品";
            }
            outputText += QString("%1 x%2 ").arg(outputItemName).arg(output.outputCount);
        }

        if (output.successRate < 1.0f)
        {
            outputText += QString("(成功率: %1%) ").arg(int(output.successRate * 100));
        }
    }
    m_outputLabel = new QLabel(outputText);
    m_outputLabel->setStyleSheet("color: #2E8B57; font-weight: bold;");
    mainLayout->addWidget(m_outputLabel);

    // 成功率进度条
    if (!m_recipe.outputs.isEmpty() && m_recipe.outputs.first().successRate < 1.0f)
    {
        m_successRateBar = new QProgressBar();
        m_successRateBar->setRange(0, 100);
        m_successRateBar->setValue(int(m_recipe.outputs.first().successRate * 100));
        m_successRateBar->setFormat("成功率: %p%");
        mainLayout->addWidget(m_successRateBar);
    }

    // 锻造按钮
    m_forgeButton = new QPushButton("锻造");
    m_forgeButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; border-radius: 4px; }");
    connect(m_forgeButton, &QPushButton::clicked, this, &RecipeCard::onForgeClicked);
    mainLayout->addWidget(m_forgeButton);

    updateForgeButton();
}

void RecipeCard::updateMaterials()
{
    // 清空现有的材料显示
    QLayoutItem *item;
    while ((item = m_materialsLayout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }

    // 添加材料信息
    for (const auto &material : m_recipe.materials)
    {
        QHBoxLayout *materialLayout = new QHBoxLayout();

        // 获取当前拥有的数量
        int ownedCount = 0;
        if (m_viewModel && m_viewModel->get_backpack_model())
        {
            ownedCount = m_viewModel->get_backpack_model()->getItemCount(material.itemId);
        }

        // 获取物品名称 - 特殊处理通用物品
        QString itemName;
        if (material.itemId == 0)
        {
            // 通用物品，根据配方类型显示特殊名称
            if (m_recipe.recipeId == 5)
            {
                itemName = "任意普通品质物品";
            }
            else if (m_recipe.recipeId == 6)
            {
                itemName = "任意稀有品质物品";
            }
            else if (m_recipe.recipeId == 7)
            {
                itemName = "任意史诗品质物品";
            }
            else
            {
                itemName = "任意物品";
            }
            ownedCount = -1; // 通用物品不显示具体数量
        }
        else
        {
            // 具体物品
            if (m_viewModel && m_viewModel->get_backpack_model())
            {
                itemName = m_viewModel->get_backpack_model()->getItemName(material.itemId);
            }
            else
            {
                itemName = "未知物品";
            }
        }

        QString materialText;
        if (ownedCount == -1)
        {
            // 通用物品不显示数量
            materialText = QString("%1: 需要%2个")
                               .arg(itemName)
                               .arg(material.requiredCount);
        }
        else
        {
            // 具体物品显示拥有数量
            materialText = QString("%1: %2/%3")
                               .arg(itemName)
                               .arg(ownedCount)
                               .arg(material.requiredCount);
        }

        if (material.isCatalyst)
        {
            materialText += " (催化剂)";
        }

        QLabel *materialLabel = new QLabel(materialText);

        // 根据是否足够设置颜色
        if (ownedCount == -1 || ownedCount >= material.requiredCount)
        {
            materialLabel->setStyleSheet("color: #2E8B57;"); // 绿色
        }
        else
        {
            materialLabel->setStyleSheet("color: #DC143C;"); // 红色
        }

        materialLayout->addWidget(materialLabel);
        m_materialsLayout->addLayout(materialLayout);
    }

    updateForgeButton();
}

bool RecipeCard::canForge() const
{
    if (!m_viewModel || !m_viewModel->getForgeModel())
    {
        return false;
    }

    return m_viewModel->getForgeModel()->canForge(m_recipe.recipeId);
}

void RecipeCard::updateForgeButton()
{
    bool enabled = canForge();
    m_forgeButton->setEnabled(enabled);

    if (!enabled)
    {
        m_forgeButton->setStyleSheet("QPushButton { background-color: #ccc; color: #666; padding: 8px; border-radius: 4px; }");
        m_forgeButton->setText("材料不足");
    }
    else
    {
        m_forgeButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; border-radius: 4px; }");
        m_forgeButton->setText("锻造");
    }
}

void RecipeCard::onForgeClicked()
{
    if (canForge())
    {
        emit forgeRequested(m_recipe.recipeId);
    }
}

// ============ WorkUpgradeCard 实现 ============
WorkUpgradeCard::WorkUpgradeCard(const WorkSystemUpgrade &upgrade, PetViewModel *viewModel, QWidget *parent)
    : QFrame(parent), m_upgrade(upgrade), m_viewModel(viewModel)
{
    setupUI();
    updateMaterials();
}

void WorkUpgradeCard::setupUI()
{
    setFrameStyle(QFrame::StyledPanel);
    setStyleSheet("QFrame { border: 1px solid #ccc; border-radius: 8px; padding: 8px; margin: 4px; background-color: #f0f8ff; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 升级标题
    QString titleText = QString("工作系统升级: %1 → %2")
                            .arg(static_cast<int>(m_upgrade.currentLevel))
                            .arg(static_cast<int>(m_upgrade.targetLevel));
    m_titleLabel = new QLabel(titleText);
    m_titleLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #1E90FF;");
    mainLayout->addWidget(m_titleLabel);

    // 升级收益
    QString benefitsText = QString("收益:\n• 掉落率提升: +%1%\n• 品质加成: +%2%\n• 解锁新物品: %3个")
                               .arg(int((m_upgrade.dropRateMultiplier - 1.0f) * 100))
                               .arg(int(m_upgrade.qualityBonus * 100))
                               .arg(m_upgrade.unlockedItems.size());
    m_benefitsLabel = new QLabel(benefitsText);
    m_benefitsLabel->setStyleSheet("color: #2E8B57; font-size: 12px;");
    mainLayout->addWidget(m_benefitsLabel);

    // 所需材料
    QGroupBox *materialsGroup = new QGroupBox("升级材料");
    m_materialsLayout = new QVBoxLayout(materialsGroup);
    mainLayout->addWidget(materialsGroup);

    // 升级按钮
    m_upgradeButton = new QPushButton("升级");
    m_upgradeButton->setStyleSheet("QPushButton { background-color: #1E90FF; color: white; font-weight: bold; padding: 8px; border-radius: 4px; }");
    connect(m_upgradeButton, &QPushButton::clicked, this, &WorkUpgradeCard::onUpgradeClicked);
    mainLayout->addWidget(m_upgradeButton);

    updateUpgradeButton();
}

void WorkUpgradeCard::updateMaterials()
{
    // 清空现有的材料显示
    QLayoutItem *item;
    while ((item = m_materialsLayout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }

    // 添加材料信息
    for (const auto &material : m_upgrade.upgradeMaterials)
    {
        QHBoxLayout *materialLayout = new QHBoxLayout();

        // 获取当前拥有的数量
        int ownedCount = 0;
        if (m_viewModel && m_viewModel->get_backpack_model())
        {
            ownedCount = m_viewModel->get_backpack_model()->getItemCount(material.itemId);
        }

        // 获取物品名称
        QString itemName = "未知物品";
        if (m_viewModel && m_viewModel->get_backpack_model())
        {
            itemName = m_viewModel->get_backpack_model()->getItemName(material.itemId);
        }

        QString materialText = QString("%1: %2/%3")
                                   .arg(itemName)
                                   .arg(ownedCount)
                                   .arg(material.requiredCount);

        if (material.isCatalyst)
        {
            materialText += " (催化剂)";
        }

        QLabel *materialLabel = new QLabel(materialText);

        // 根据是否足够设置颜色
        if (ownedCount >= material.requiredCount)
        {
            materialLabel->setStyleSheet("color: #2E8B57;"); // 绿色
        }
        else
        {
            materialLabel->setStyleSheet("color: #DC143C;"); // 红色
        }

        materialLayout->addWidget(materialLabel);
        m_materialsLayout->addLayout(materialLayout);
    }

    updateUpgradeButton();
}

bool WorkUpgradeCard::canUpgrade() const
{
    if (!m_viewModel || !m_viewModel->getForgeModel())
    {
        return false;
    }

    return m_viewModel->getForgeModel()->canUpgradeWorkSystem(m_upgrade.workType);
}

void WorkUpgradeCard::updateUpgradeButton()
{
    bool enabled = canUpgrade();
    m_upgradeButton->setEnabled(enabled);

    if (!enabled)
    {
        m_upgradeButton->setStyleSheet("QPushButton { background-color: #ccc; color: #666; padding: 8px; border-radius: 4px; }");
        m_upgradeButton->setText("材料不足");
    }
    else
    {
        m_upgradeButton->setStyleSheet("QPushButton { background-color: #1E90FF; color: white; font-weight: bold; padding: 8px; border-radius: 4px; }");
        m_upgradeButton->setText("升级");
    }
}

void WorkUpgradeCard::onUpgradeClicked()
{
    if (canUpgrade())
    {
        emit upgradeRequested(m_upgrade.workType, m_upgrade.targetLevel);
    }
}

// ============ ForgePanel 实现 ============
ForgePanel::ForgePanel(PetViewModel *viewModel, CommandManager &commandManager, QWidget *parent)
    : QWidget(parent), m_viewModel(viewModel), m_commandManager(commandManager)
{
    qDebug() << "ForgePanel::ForgePanel constructor called";

    setupUI();
    connectSignals();
    refreshAll();

    qDebug() << "ForgePanel constructor completed";
}

ForgePanel::~ForgePanel()
{
}

void ForgePanel::closeEvent(QCloseEvent *event)
{
    // 隐藏窗口而不是销毁，这样不会导致应用程序退出
    hide();
    event->ignore(); // 忽略关闭事件，阻止窗口销毁
    qDebug() << "ForgePanel closed (hidden)";
}

void ForgePanel::setupUI()
{
    setWindowTitle("物品合成台");
    setFixedSize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 顶部资源显示区域
    setupResourceDisplay();
    mainLayout->addWidget(m_resourceDisplayWidget);

    // 中间合成按钮区域
    setupSynthesisButtons();
    mainLayout->addWidget(m_synthesisWidget);

    // 底部关闭按钮
    QPushButton *closeButton = new QPushButton("关闭");
    closeButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; border-radius: 4px; }");
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
    mainLayout->addWidget(closeButton);
}

void ForgePanel::setupResourceDisplay()
{
    m_resourceDisplayWidget = new QWidget();
    QVBoxLayout *resourceLayout = new QVBoxLayout(m_resourceDisplayWidget);

    // 标题
    QLabel *titleLabel = new QLabel("拥有的材料");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #2E8B57; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    resourceLayout->addWidget(titleLabel);

    // 资源显示网格
    QGridLayout *gridLayout = new QGridLayout();

    // 阳光材料显示
    QLabel *sunshineLabel = new QLabel("阳光材料:");
    sunshineLabel->setStyleSheet("font-weight: bold; color: #FF8C00;");
    gridLayout->addWidget(sunshineLabel, 0, 0);

    // 阳光数量标签
    m_sunshineLabels.resize(5);
    QStringList sunshineNames = {"微光阳光", "温暖阳光", "炽热阳光", "灿烂阳光", "神圣阳光"};
    for (int i = 0; i < 5; i++)
    {
        m_sunshineLabels[i] = new QLabel(QString("%1: 0").arg(sunshineNames[i]));
        m_sunshineLabels[i]->setStyleSheet("color: #333; padding: 2px;");
        gridLayout->addWidget(m_sunshineLabels[i], 0, i + 1);
    }

    // 矿石材料显示
    QLabel *mineralLabel = new QLabel("矿石材料:");
    mineralLabel->setStyleSheet("font-weight: bold; color: #8B4513;");
    gridLayout->addWidget(mineralLabel, 1, 0);

    // 矿石数量标签
    m_mineralLabels.resize(5);
    QStringList mineralNames = {"粗糙矿石", "普通矿石", "优质矿石", "稀有矿石", "传说矿石"};
    for (int i = 0; i < 5; i++)
    {
        m_mineralLabels[i] = new QLabel(QString("%1: 0").arg(mineralNames[i]));
        m_mineralLabels[i]->setStyleSheet("color: #333; padding: 2px;");
        gridLayout->addWidget(m_mineralLabels[i], 1, i + 1);
    }

    // 木材材料显示
    QLabel *woodLabel = new QLabel("木材材料:");
    woodLabel->setStyleSheet("font-weight: bold; color: #228B22;");
    gridLayout->addWidget(woodLabel, 2, 0);

    // 木材数量标签
    m_woodLabels.resize(5);
    QStringList woodNames = {"枯木", "普通木材", "优质木材", "稀有木材", "神木"};
    for (int i = 0; i < 5; i++)
    {
        m_woodLabels[i] = new QLabel(QString("%1: 0").arg(woodNames[i]));
        m_woodLabels[i]->setStyleSheet("color: #333; padding: 2px;");
        gridLayout->addWidget(m_woodLabels[i], 2, i + 1);
    }

    resourceLayout->addLayout(gridLayout);

    // 设置背景样式
    m_resourceDisplayWidget->setStyleSheet("QWidget { background-color: #f0f8ff; border: 2px solid #4682B4; border-radius: 8px; margin: 5px; }");
}

void ForgePanel::setupSynthesisButtons()
{
    m_synthesisWidget = new QWidget();
    QVBoxLayout *synthesisLayout = new QVBoxLayout(m_synthesisWidget);

    // 标题
    QLabel *titleLabel = new QLabel("物品合成");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #2E8B57; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    synthesisLayout->addWidget(titleLabel);

    // 合成按钮网格
    QGridLayout *buttonGrid = new QGridLayout();

    // 阳光合成按钮 (第1行)
    QLabel *sunshineRowLabel = new QLabel("阳光合成:");
    sunshineRowLabel->setStyleSheet("font-weight: bold; color: #FF8C00; padding: 10px;");
    buttonGrid->addWidget(sunshineRowLabel, 0, 0);

    QStringList sunshineButtonNames = {"合成温暖阳光", "合成炽热阳光", "合成灿烂阳光", "合成神圣阳光"};
    QStringList sunshineButtonColors = {"#FFE4B5", "#FFA500", "#FF6347", "#FFD700"};
    for (int i = 0; i < 4; i++)
    {
        QPushButton *btn = new QPushButton(sunshineButtonNames[i]);
        btn->setStyleSheet(QString("QPushButton { background-color: %1; color: #333; padding: 10px; margin: 2px; border-radius: 6px; font-weight: bold; }")
                               .arg(sunshineButtonColors[i]));
        int recipeId = i + 1; // 配方ID 1-4
        connect(btn, &QPushButton::clicked, [this, recipeId]()
                { onSynthesisButtonClicked(recipeId); });
        buttonGrid->addWidget(btn, 0, i + 1);
    }

    // 矿石合成按钮 (第2行)
    QLabel *mineralRowLabel = new QLabel("矿石合成:");
    mineralRowLabel->setStyleSheet("font-weight: bold; color: #8B4513; padding: 10px;");
    buttonGrid->addWidget(mineralRowLabel, 1, 0);

    QStringList mineralButtonNames = {"合成普通矿石", "合成优质矿石", "合成稀有矿石", "合成传说矿石"};
    QStringList mineralButtonColors = {"#D2B48C", "#CD853F", "#A0522D", "#8B4513"};
    for (int i = 0; i < 4; i++)
    {
        QPushButton *btn = new QPushButton(mineralButtonNames[i]);
        btn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; padding: 10px; margin: 2px; border-radius: 6px; font-weight: bold; }")
                               .arg(mineralButtonColors[i]));
        int recipeId = i + 5; // 配方ID 5-8
        connect(btn, &QPushButton::clicked, [this, recipeId]()
                { onSynthesisButtonClicked(recipeId); });
        buttonGrid->addWidget(btn, 1, i + 1);
    }

    // 木材合成按钮 (第3行)
    QLabel *woodRowLabel = new QLabel("木材合成:");
    woodRowLabel->setStyleSheet("font-weight: bold; color: #228B22; padding: 10px;");
    buttonGrid->addWidget(woodRowLabel, 2, 0);

    QStringList woodButtonNames = {"合成普通木材", "合成优质木材", "合成稀有木材", "合成神木"};
    QStringList woodButtonColors = {"#90EE90", "#32CD32", "#228B22", "#006400"};
    for (int i = 0; i < 4; i++)
    {
        QPushButton *btn = new QPushButton(woodButtonNames[i]);
        btn->setStyleSheet(QString("QPushButton { background-color: %1; color: white; padding: 10px; margin: 2px; border-radius: 6px; font-weight: bold; }")
                               .arg(woodButtonColors[i]));
        int recipeId = i + 9; // 配方ID 9-12
        connect(btn, &QPushButton::clicked, [this, recipeId]()
                { onSynthesisButtonClicked(recipeId); });
        buttonGrid->addWidget(btn, 2, i + 1);
    }

    synthesisLayout->addLayout(buttonGrid);

    // 设置背景样式
    m_synthesisWidget->setStyleSheet("QWidget { background-color: #f9f9f9; border: 2px solid #ddd; border-radius: 8px; margin: 5px; }");
}

void ForgePanel::connectSignals()
{
    // 连接锻造模型的信号
    if (m_viewModel && m_viewModel->getForgeModel())
    {
        connect(m_viewModel->getForgeModel(), &ForgeModel::forgeCompleted,
                this, [this](int recipeId, bool success, const QVector<int> &items)
                {
                    QString message = QString("锻造配方%1 %2! 产出物品数量: %3")
                                          .arg(recipeId)
                                          .arg(success ? "成功" : "失败")
                                          .arg(items.size());
                    qDebug() << message;
                    refreshAll(); // 刷新界面
                });

        connect(m_viewModel->getForgeModel(), &ForgeModel::workSystemUpgraded,
                this, [this](WorkType /*workType*/, WorkSystemLevel newLevel)
                {
                    QString message = QString("工作系统升级成功! 新等级: %1")
                                          .arg(static_cast<int>(newLevel));
                    qDebug() << message;
                    refreshAll(); // 刷新界面
                });
    }
}

void ForgePanel::refreshAll()
{
    // 更新资源显示
    updateResourceDisplay();

    // 如果需要保留工作升级功能，可以取消下面的注释
    // refreshRecipes();
    // refreshWorkUpgrades();

    qDebug() << "ForgePanel: Refreshed synthesis interface";
}

void ForgePanel::refreshRecipes()
{
    // 清空现有的配方卡片
    for (auto *card : m_recipeCards)
    {
        m_recipesLayout->removeWidget(card);
        card->deleteLater();
    }
    m_recipeCards.clear();

    // 添加新的配方卡片
    if (m_viewModel && m_viewModel->getForgeModel())
    {
        auto recipes = m_viewModel->getForgeModel()->getAvailableRecipes();
        for (const auto &recipe : recipes)
        {
            RecipeCard *card = new RecipeCard(recipe, m_viewModel);
            connect(card, &RecipeCard::forgeRequested, this, &ForgePanel::onRecipeForgeRequested);
            m_recipesLayout->addWidget(card);
            m_recipeCards.append(card);
        }
    }
}

void ForgePanel::refreshWorkUpgrades()
{
    // 清空现有的升级卡片
    for (auto *card : m_workUpgradeCards)
    {
        m_workUpgradesLayout->removeWidget(card);
        card->deleteLater();
    }
    m_workUpgradeCards.clear();

    // 添加新的升级卡片
    if (m_viewModel && m_viewModel->getForgeModel())
    {
        auto upgrades = m_viewModel->getForgeModel()->getAvailableWorkUpgrades();
        for (const auto &upgrade : upgrades)
        {
            WorkUpgradeCard *card = new WorkUpgradeCard(upgrade, m_viewModel);
            connect(card, &WorkUpgradeCard::upgradeRequested, this, &ForgePanel::onWorkUpgradeRequested);
            m_workUpgradesLayout->addWidget(card);
            m_workUpgradeCards.append(card);
        }
    }
}

void ForgePanel::onRecipeForgeRequested(int recipeId)
{
    ICommandBase *command = m_commandManager.get_command(CommandType::FORGE);
    if (command)
    {
        ForgeCommandParameter param(recipeId);
        command->exec(&param);
    }
}

void ForgePanel::onWorkUpgradeRequested(WorkType workType, WorkSystemLevel targetLevel)
{
    ICommandBase *command = m_commandManager.get_command(CommandType::FORGE);
    if (command)
    {
        ForgeCommandParameter param(workType, targetLevel);
        command->exec(&param);
    }
}

void ForgePanel::onSynthesisButtonClicked(int recipeId)
{
    // 先检查是否有足够材料
    if (m_viewModel && m_viewModel->getForgeModel())
    {
        ForgeModel *forgeModel = m_viewModel->getForgeModel();

        // 检查材料是否足够
        if (!forgeModel->canForge(recipeId))
        {
            // 材料不足时显示警告
            ForgeRecipe recipe = forgeModel->getRecipeById(recipeId);
            QMessageBox::warning(this, "材料不足",
                                 QString("合成 %1 需要更多材料！").arg(recipe.name));
            return;
        }
    }

    // 执行合成操作
    ICommandBase *command = m_commandManager.get_command(CommandType::FORGE);
    if (command)
    {
        ForgeCommandParameter param(recipeId);
        command->exec(&param);

        // 合成成功时不显示弹窗，只刷新资源显示
        updateResourceDisplay();
    }
    else
    {
        QMessageBox::warning(this, "错误", "无法找到锻造命令！");
    }
}

void ForgePanel::updateResourceDisplay()
{
    // 打开物品选择对话框
    ItemSelectionDialog dialog(m_viewModel, fromQuality, 3, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        auto selectedItems = dialog.getSelectedItems();

        if (selectedItems.size() == 0)
        {
            QMessageBox::warning(this, "错误", "未选择任何物品！");
            return;
        }

        // 检查选择的物品总数是否为3
        int totalCount = 0;
        for (const auto &item : selectedItems)
        {
            totalCount += item.second;
        }

        if (totalCount != 3)
        {
            QMessageBox::warning(this, "错误", QString("需要选择恰好3个物品，当前选择了%1个！").arg(totalCount));
            return;
        }

        // 确定要使用的配方ID
        int recipeId = 0;
        switch (fromQuality)
        {
        case ItemQuality::Common:
            recipeId = 5;
            break;
        case ItemQuality::Uncommon:
            recipeId = 6;
            break;
        case ItemQuality::Rare:
            recipeId = 7;
            break;
        default:
            QMessageBox::warning(this, "错误", "不支持的品质强化！");
            return;
        }

        // 执行锻造命令
        ICommandBase *command = m_commandManager.get_command(CommandType::FORGE);
        if (command)
        {
            // 构建自定义材料列表
            QVector<ForgeMaterial> customMaterials;

            // 添加用户选择的物品作为材料
            for (const auto &item : selectedItems)
            {
                ForgeMaterial material;
                material.itemId = item.first;
                material.requiredCount = item.second;
                material.isCatalyst = false;
                customMaterials.append(material);
            }

            // 添加催化剂（阳光）
            ForgeMaterial catalyst;
            catalyst.isCatalyst = true;
            catalyst.requiredCount = 1;

            switch (fromQuality)
            {
            case ItemQuality::Common:
                catalyst.itemId = 7; // 温暖阳光
                break;
            case ItemQuality::Uncommon:
                catalyst.itemId = 8; // 炽热阳光
                break;
            case ItemQuality::Rare:
                catalyst.itemId = 10; // 神圣阳光
                break;
            default:
                catalyst.itemId = 7; // 默认温暖阳光
                break;
            }
            customMaterials.append(catalyst);

            // 创建包含自定义材料的锻造参数
            ForgeCommandParameter param(recipeId, customMaterials);
            command->exec(&param);

            QMessageBox::information(this, "锻造请求", "物品强化请求已提交！");
        }
        else
        {
            QMessageBox::warning(this, "错误", "无法找到锻造命令！");
        }
    }
}
