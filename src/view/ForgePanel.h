#ifndef __FORGE_PANEL_H__
#define __FORGE_PANEL_H__

#include <QWidget>
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
#include <QCloseEvent>
#include "../common/ForgeTypes.h"
#include "../common/CommandManager.h"

class PetViewModel;

// 配方卡片widget (保留用于工作升级等其他功能)
class RecipeCard : public QFrame
{
    Q_OBJECT

public:
    explicit RecipeCard(const ForgeRecipe &recipe, PetViewModel *viewModel, QWidget *parent = nullptr);

    void updateMaterials();
    bool canForge() const;

signals:
    void forgeRequested(int recipeId);

private slots:
    void onForgeClicked();

private:
    void setupUI();
    void updateForgeButton();

private:
    ForgeRecipe m_recipe;
    PetViewModel *m_viewModel;
    QLabel *m_nameLabel;
    QLabel *m_descriptionLabel;
    QVBoxLayout *m_materialsLayout;
    QLabel *m_outputLabel;
    QPushButton *m_forgeButton;
    QProgressBar *m_successRateBar;
};

// 工作系统升级卡片
class WorkUpgradeCard : public QFrame
{
    Q_OBJECT

public:
    explicit WorkUpgradeCard(const WorkSystemUpgrade &upgrade, PetViewModel *viewModel, QWidget *parent = nullptr);

    void updateMaterials();
    bool canUpgrade() const;

signals:
    void upgradeRequested(WorkType workType, WorkSystemLevel targetLevel);

private slots:
    void onUpgradeClicked();

private:
    void setupUI();
    void updateUpgradeButton();

private:
    WorkSystemUpgrade m_upgrade;
    PetViewModel *m_viewModel;
    QLabel *m_titleLabel;
    QLabel *m_benefitsLabel;
    QVBoxLayout *m_materialsLayout;
    QPushButton *m_upgradeButton;
};

// 主锻造面板 - 简化为物品合成台
class ForgePanel : public QWidget
{
    Q_OBJECT

public:
    explicit ForgePanel(PetViewModel *viewModel, CommandManager &commandManager, QWidget *parent = nullptr);
    ~ForgePanel();

    void refreshAll();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onRecipeForgeRequested(int recipeId);
    void onWorkUpgradeRequested(WorkType workType, WorkSystemLevel targetLevel);
    void onSynthesisButtonClicked(int recipeId); // 新增：合成按钮点击处理
    void refreshRecipes();
    void refreshWorkUpgrades();

private:
    void setupUI();
    void setupResourceDisplay();  // 新增：设置资源显示区域
    void setupSynthesisButtons(); // 新增：设置合成按钮区域
    void updateResourceDisplay(); // 新增：更新资源显示
    void connectSignals();

private:
    PetViewModel *m_viewModel;
    CommandManager &m_commandManager;

    // 新UI组件 - 简化界面
    QWidget *m_resourceDisplayWidget; // 资源显示区域
    QWidget *m_synthesisWidget;       // 合成按钮区域

    // 资源显示标签
    QVector<QLabel *> m_sunshineLabels; // 阳光材料标签 (5个)
    QVector<QLabel *> m_mineralLabels;  // 矿石材料标签 (5个)
    QVector<QLabel *> m_woodLabels;     // 木材材料标签 (5个)

    // 保留旧组件用于工作升级等功能
    QTabWidget *m_tabWidget;
    QScrollArea *m_recipesScrollArea;
    QWidget *m_recipesWidget;
    QVBoxLayout *m_recipesLayout;
    QScrollArea *m_workUpgradesScrollArea;
    QWidget *m_workUpgradesWidget;
    QVBoxLayout *m_workUpgradesLayout;
    QWidget *m_statsWidget;
    QTextEdit *m_statsTextEdit;

    // 卡片列表
    QVector<RecipeCard *> m_recipeCards;
    QVector<WorkUpgradeCard *> m_workUpgradeCards;
};

#endif // __FORGE_PANEL_H__
