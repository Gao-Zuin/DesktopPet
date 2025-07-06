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
RecipeCard::RecipeCard(const ForgeRecipe& recipe, PetViewModel* viewModel, QWidget* parent)
    : QFrame(parent)
    , m_recipe(recipe)
    , m_viewModel(viewModel)
{
    setupUI();
    updateMaterials();
}

void RecipeCard::setupUI()
{
    setFrameStyle(QFrame::StyledPanel);
    setStyleSheet("QFrame { border: 1px solid #ccc; border-radius: 8px; padding: 8px; margin: 4px; background-color: #f9f9f9; }");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
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
    QGroupBox* materialsGroup = new QGroupBox("所需材料");
    m_materialsLayout = new QVBoxLayout(materialsGroup);
    mainLayout->addWidget(materialsGroup);
    
    // 产出物品
    QString outputText = "产出: ";
    for (const auto& output : m_recipe.outputs) {
        outputText += QString("物品%1 x%2 ").arg(output.itemId).arg(output.outputCount);
        if (output.successRate < 1.0f) {
            outputText += QString("(成功率: %1%) ").arg(int(output.successRate * 100));
        }
    }
    m_outputLabel = new QLabel(outputText);
    m_outputLabel->setStyleSheet("color: #2E8B57; font-weight: bold;");
    mainLayout->addWidget(m_outputLabel);
    
    // 成功率进度条
    if (!m_recipe.outputs.isEmpty() && m_recipe.outputs.first().successRate < 1.0f) {
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
    QLayoutItem* item;
    while ((item = m_materialsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    
    // 添加材料信息
    for (const auto& material : m_recipe.materials) {
        QHBoxLayout* materialLayout = new QHBoxLayout();
        
        // 获取当前拥有的数量
        int ownedCount = 0;
        if (m_viewModel && m_viewModel->get_backpack_model()) {
            ownedCount = m_viewModel->get_backpack_model()->getItemCount(material.itemId);
        }
        
        QString materialText = QString("物品%1: %2/%3")
            .arg(material.itemId)
            .arg(ownedCount)
            .arg(material.requiredCount);
        
        if (material.isCatalyst) {
            materialText += " (催化剂)";
        }
        
        QLabel* materialLabel = new QLabel(materialText);
        
        // 根据是否足够设置颜色
        if (ownedCount >= material.requiredCount) {
            materialLabel->setStyleSheet("color: #2E8B57;"); // 绿色
        } else {
            materialLabel->setStyleSheet("color: #DC143C;"); // 红色
        }
        
        materialLayout->addWidget(materialLabel);
        m_materialsLayout->addLayout(materialLayout);
    }
    
    updateForgeButton();
}

bool RecipeCard::canForge() const
{
    if (!m_viewModel || !m_viewModel->getForgeModel()) {
        return false;
    }
    
    return m_viewModel->getForgeModel()->canForge(m_recipe.recipeId);
}

void RecipeCard::updateForgeButton()
{
    bool enabled = canForge();
    m_forgeButton->setEnabled(enabled);
    
    if (!enabled) {
        m_forgeButton->setStyleSheet("QPushButton { background-color: #ccc; color: #666; padding: 8px; border-radius: 4px; }");
        m_forgeButton->setText("材料不足");
    } else {
        m_forgeButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; border-radius: 4px; }");
        m_forgeButton->setText("锻造");
    }
}

void RecipeCard::onForgeClicked()
{
    if (canForge()) {
        emit forgeRequested(m_recipe.recipeId);
    }
}

// ============ WorkUpgradeCard 实现 ============
WorkUpgradeCard::WorkUpgradeCard(const WorkSystemUpgrade& upgrade, PetViewModel* viewModel, QWidget* parent)
    : QFrame(parent)
    , m_upgrade(upgrade)
    , m_viewModel(viewModel)
{
    setupUI();
    updateMaterials();
}

void WorkUpgradeCard::setupUI()
{
    setFrameStyle(QFrame::StyledPanel);
    setStyleSheet("QFrame { border: 1px solid #ccc; border-radius: 8px; padding: 8px; margin: 4px; background-color: #f0f8ff; }");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
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
    QGroupBox* materialsGroup = new QGroupBox("升级材料");
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
    QLayoutItem* item;
    while ((item = m_materialsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    
    // 添加材料信息
    for (const auto& material : m_upgrade.upgradeMaterials) {
        QHBoxLayout* materialLayout = new QHBoxLayout();
        
        // 获取当前拥有的数量
        int ownedCount = 0;
        if (m_viewModel && m_viewModel->get_backpack_model()) {
            ownedCount = m_viewModel->get_backpack_model()->getItemCount(material.itemId);
        }
        
        QString materialText = QString("物品%1: %2/%3")
            .arg(material.itemId)
            .arg(ownedCount)
            .arg(material.requiredCount);
        
        if (material.isCatalyst) {
            materialText += " (催化剂)";
        }
        
        QLabel* materialLabel = new QLabel(materialText);
        
        // 根据是否足够设置颜色
        if (ownedCount >= material.requiredCount) {
            materialLabel->setStyleSheet("color: #2E8B57;"); // 绿色
        } else {
            materialLabel->setStyleSheet("color: #DC143C;"); // 红色
        }
        
        materialLayout->addWidget(materialLabel);
        m_materialsLayout->addLayout(materialLayout);
    }
    
    updateUpgradeButton();
}

bool WorkUpgradeCard::canUpgrade() const
{
    if (!m_viewModel || !m_viewModel->getForgeModel()) {
        return false;
    }
    
    return m_viewModel->getForgeModel()->canUpgradeWorkSystem(m_upgrade.workType);
}

void WorkUpgradeCard::updateUpgradeButton()
{
    bool enabled = canUpgrade();
    m_upgradeButton->setEnabled(enabled);
    
    if (!enabled) {
        m_upgradeButton->setStyleSheet("QPushButton { background-color: #ccc; color: #666; padding: 8px; border-radius: 4px; }");
        m_upgradeButton->setText("材料不足");
    } else {
        m_upgradeButton->setStyleSheet("QPushButton { background-color: #1E90FF; color: white; font-weight: bold; padding: 8px; border-radius: 4px; }");
        m_upgradeButton->setText("升级");
    }
}

void WorkUpgradeCard::onUpgradeClicked()
{
    if (canUpgrade()) {
        emit upgradeRequested(m_upgrade.workType, m_upgrade.targetLevel);
    }
}

// ============ ForgePanel 实现 ============
ForgePanel::ForgePanel(PetViewModel* viewModel, CommandManager& commandManager, QWidget* parent)
    : QWidget(parent)
    , m_viewModel(viewModel)
    , m_commandManager(commandManager)
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

void ForgePanel::closeEvent(QCloseEvent* event)
{
    // 隐藏窗口而不是销毁，这样不会导致应用程序退出
    hide();
    event->ignore(); // 忽略关闭事件，阻止窗口销毁
    qDebug() << "ForgePanel closed (hidden)";
}

void ForgePanel::setupUI()
{
    setWindowTitle("锻造台");
    setFixedSize(600, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 创建标签页
    m_tabWidget = new QTabWidget();
    
    setupItemUpgradeTab();
    setupItemEnhanceTab();  // 新增：物品强化标签页
    setupWorkUpgradeTab();
    setupStatsTab();
    
    mainLayout->addWidget(m_tabWidget);
    
    // 关闭按钮
    QPushButton* closeButton = new QPushButton("关闭");
    closeButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; border-radius: 4px; }");
    connect(closeButton, &QPushButton::clicked, this, &QWidget::close);
    mainLayout->addWidget(closeButton);
    
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &ForgePanel::onTabChanged);
}

void ForgePanel::setupItemUpgradeTab()
{
    // 物品升级标签页 (阳光精炼等)
    m_recipesScrollArea = new QScrollArea();
    m_recipesWidget = new QWidget();
    m_recipesLayout = new QVBoxLayout(m_recipesWidget);
    m_recipesLayout->setAlignment(Qt::AlignTop);
    
    m_recipesScrollArea->setWidget(m_recipesWidget);
    m_recipesScrollArea->setWidgetResizable(true);
    m_recipesScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_tabWidget->addTab(m_recipesScrollArea, "阳光精炼");
}

void ForgePanel::setupItemEnhanceTab()
{
    // 物品强化标签页 (通用物品强化)
    QWidget* enhanceWidget = new QWidget();
    QVBoxLayout* enhanceLayout = new QVBoxLayout(enhanceWidget);
    
    // 标题
    QLabel* titleLabel = new QLabel("物品通用强化");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #2E8B57; padding: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    enhanceLayout->addWidget(titleLabel);
    
    // 说明文字
    QLabel* descLabel = new QLabel("选择3个相同品质的物品，消耗对应阳光催化剂，强化为1个更高品质的物品。\n"
                                  "• 普通 → 稀有：需要温暖阳光\n"
                                  "• 稀有 → 史诗：需要炽热阳光\n"
                                  "• 史诗 → 传说：需要神圣阳光");
    descLabel->setStyleSheet("color: #666; font-size: 12px; padding: 10px; background-color: #f0f8ff; border-radius: 5px;");
    descLabel->setWordWrap(true);
    enhanceLayout->addWidget(descLabel);
    
    // 强化选项
    QGroupBox* enhanceGroup = new QGroupBox("选择强化等级");
    QVBoxLayout* enhanceOptionsLayout = new QVBoxLayout(enhanceGroup);
    
    // 普通 → 稀有
    QPushButton* commonToRareBtn = new QPushButton("普通品质强化为稀有品质");
    commonToRareBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 12px; margin: 4px; border-radius: 6px; font-weight: bold; }");
    connect(commonToRareBtn, &QPushButton::clicked, [this]() {
        onItemEnhanceRequested(ItemQuality::Common, ItemQuality::Uncommon);
    });
    enhanceOptionsLayout->addWidget(commonToRareBtn);
    
    // 稀有 → 史诗
    QPushButton* rareToEpicBtn = new QPushButton("稀有品质强化为史诗品质");
    rareToEpicBtn->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 12px; margin: 4px; border-radius: 6px; font-weight: bold; }");
    connect(rareToEpicBtn, &QPushButton::clicked, [this]() {
        onItemEnhanceRequested(ItemQuality::Uncommon, ItemQuality::Rare);
    });
    enhanceOptionsLayout->addWidget(rareToEpicBtn);
    
    // 史诗 → 传说
    QPushButton* epicToLegendaryBtn = new QPushButton("史诗品质强化为传说品质");
    epicToLegendaryBtn->setStyleSheet("QPushButton { background-color: #FF9800; color: white; padding: 12px; margin: 4px; border-radius: 6px; font-weight: bold; }");
    connect(epicToLegendaryBtn, &QPushButton::clicked, [this]() {
        onItemEnhanceRequested(ItemQuality::Rare, ItemQuality::Epic);
    });
    enhanceOptionsLayout->addWidget(epicToLegendaryBtn);
    
    enhanceLayout->addWidget(enhanceGroup);
    enhanceLayout->addStretch();
    
    m_tabWidget->addTab(enhanceWidget, "物品强化");
}

void ForgePanel::setupWorkUpgradeTab()
{
    // 工作系统升级标签页
    m_workUpgradesScrollArea = new QScrollArea();
    m_workUpgradesWidget = new QWidget();
    m_workUpgradesLayout = new QVBoxLayout(m_workUpgradesWidget);
    m_workUpgradesLayout->setAlignment(Qt::AlignTop);
    
    m_workUpgradesScrollArea->setWidget(m_workUpgradesWidget);
    m_workUpgradesScrollArea->setWidgetResizable(true);
    m_workUpgradesScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_tabWidget->addTab(m_workUpgradesScrollArea, "工作升级");
}

void ForgePanel::setupStatsTab()
{
    // 统计信息标签页
    m_statsWidget = new QWidget();
    QVBoxLayout* statsLayout = new QVBoxLayout(m_statsWidget);
    
    m_statsTextEdit = new QTextEdit();
    m_statsTextEdit->setReadOnly(true);
    statsLayout->addWidget(m_statsTextEdit);
    
    m_tabWidget->addTab(m_statsWidget, "锻造统计");
}

void ForgePanel::connectSignals()
{
    // 连接锻造模型的信号
    if (m_viewModel && m_viewModel->getForgeModel()) {
        connect(m_viewModel->getForgeModel(), &ForgeModel::forgeCompleted,
                this, [this](int recipeId, bool success, const QVector<int>& items) {
                    QString message = QString("锻造配方%1 %2! 产出物品数量: %3")
                        .arg(recipeId)
                        .arg(success ? "成功" : "失败")
                        .arg(items.size());
                    qDebug() << message;
                    refreshAll(); // 刷新界面
                });
        
        connect(m_viewModel->getForgeModel(), &ForgeModel::workSystemUpgraded,
                this, [this](WorkType /*workType*/, WorkSystemLevel newLevel) {
                    QString message = QString("工作系统升级成功! 新等级: %1")
                        .arg(static_cast<int>(newLevel));
                    qDebug() << message;
                    refreshAll(); // 刷新界面
                });
    }
}

void ForgePanel::refreshAll()
{
    refreshRecipes();
    refreshWorkUpgrades();
    
    // 更新统计信息
    if (m_viewModel && m_viewModel->getForgeModel()) {
        ForgeModel* forgeModel = m_viewModel->getForgeModel();
        QString statsText = QString(
            "锻造统计信息\n\n"
            "总锻造次数: %1\n"
            "成功次数: %2\n"
            "成功率: %3%\n\n"
            "历史记录数量: %4"
        ).arg(forgeModel->getTotalForgeCount())
         .arg(forgeModel->getSuccessfulForgeCount())
         .arg(int(forgeModel->getForgeSuccessRate() * 100))
         .arg(forgeModel->getForgeHistory().size());
        
        m_statsTextEdit->setPlainText(statsText);
    }
}

void ForgePanel::refreshRecipes()
{
    // 清空现有的配方卡片
    for (auto* card : m_recipeCards) {
        m_recipesLayout->removeWidget(card);
        card->deleteLater();
    }
    m_recipeCards.clear();
    
    // 添加新的配方卡片
    if (m_viewModel && m_viewModel->getForgeModel()) {
        auto recipes = m_viewModel->getForgeModel()->getAvailableRecipes();
        for (const auto& recipe : recipes) {
            RecipeCard* card = new RecipeCard(recipe, m_viewModel);
            connect(card, &RecipeCard::forgeRequested, this, &ForgePanel::onRecipeForgeRequested);
            m_recipesLayout->addWidget(card);
            m_recipeCards.append(card);
        }
    }
}

void ForgePanel::refreshWorkUpgrades()
{
    // 清空现有的升级卡片
    for (auto* card : m_workUpgradeCards) {
        m_workUpgradesLayout->removeWidget(card);
        card->deleteLater();
    }
    m_workUpgradeCards.clear();
    
    // 添加新的升级卡片
    if (m_viewModel && m_viewModel->getForgeModel()) {
        auto upgrades = m_viewModel->getForgeModel()->getAvailableWorkUpgrades();
        for (const auto& upgrade : upgrades) {
            WorkUpgradeCard* card = new WorkUpgradeCard(upgrade, m_viewModel);
            connect(card, &WorkUpgradeCard::upgradeRequested, this, &ForgePanel::onWorkUpgradeRequested);
            m_workUpgradesLayout->addWidget(card);
            m_workUpgradeCards.append(card);
        }
    }
}

void ForgePanel::onRecipeForgeRequested(int recipeId)
{
    ICommandBase* command = m_commandManager.get_command(CommandType::FORGE);
    if (command) {
        ForgeCommandParameter param(recipeId);
        command->exec(&param);
    }
}

void ForgePanel::onWorkUpgradeRequested(WorkType workType, WorkSystemLevel targetLevel)
{
    ICommandBase* command = m_commandManager.get_command(CommandType::FORGE);
    if (command) {
        ForgeCommandParameter param(workType, targetLevel);
        command->exec(&param);
    }
}

void ForgePanel::onTabChanged(int index)
{
    // 切换标签页时刷新数据
    if (index == 0) {
        refreshRecipes();
    } else if (index == 1) {
        refreshWorkUpgrades();
    }
}

void ForgePanel::onItemEnhanceRequested(ItemQuality fromQuality, ItemQuality /*toQuality*/)
{
    // 打开物品选择对话框
    ItemSelectionDialog dialog(m_viewModel, fromQuality, 3, this);
    
    if (dialog.exec() == QDialog::Accepted) {
        auto selectedItems = dialog.getSelectedItems();
        
        if (selectedItems.size() == 0) {
            QMessageBox::warning(this, "错误", "未选择任何物品！");
            return;
        }
        
        // 检查选择的物品总数是否为3
        int totalCount = 0;
        for (const auto& item : selectedItems) {
            totalCount += item.second;
        }
        
        if (totalCount != 3) {
            QMessageBox::warning(this, "错误", QString("需要选择恰好3个物品，当前选择了%1个！").arg(totalCount));
            return;
        }
        
        // 确定要使用的配方ID
        int recipeId = 0;
        switch (fromQuality) {
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
        ICommandBase* command = m_commandManager.get_command(CommandType::FORGE);
        if (command) {
            // 构建自定义材料列表
            QVector<ForgeMaterial> customMaterials;
            
            // 添加用户选择的物品作为材料
            for (const auto& item : selectedItems) {
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
            
            switch (fromQuality) {
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
        } else {
            QMessageBox::warning(this, "错误", "无法找到锻造命令！");
        }
    }
}
