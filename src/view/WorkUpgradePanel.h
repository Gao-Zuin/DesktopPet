#ifndef __WORK_UPGRADE_PANEL_H__
#define __WORK_UPGRADE_PANEL_H__

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QFrame>
#include <QGroupBox>
#include <QCloseEvent>
#include "../common/ForgeTypes.h"
#include "../common/CommandManager.h"

class PetViewModel;

// 单个工作系统升级卡片
class WorkSystemUpgradeCard : public QFrame
{
    Q_OBJECT

public:
    explicit WorkSystemUpgradeCard(WorkType workType, PetViewModel *viewModel, QWidget *parent = nullptr);

    void updateDisplay();
    bool canUpgrade() const;

signals:
    void upgradeRequested(WorkType workType);

private slots:
    void onUpgradeClicked();

private:
    void setupUI();
    void updateMaterials();
    void updateBenefits();  // 重新添加updateBenefits方法声明
    QString getWorkTypeName(WorkType workType) const;
    QString getLevelName(WorkSystemLevel level) const;

private:
    WorkType m_workType;
    PetViewModel *m_viewModel;

    // UI组件
    QLabel *m_titleLabel;
    QLabel *m_currentLevelLabel;
    QLabel *m_materialsTextLabel; // 材料文本标签
    QVBoxLayout *m_materialsLayout;
    QPushButton *m_upgradeButton;
    QProgressBar *m_progressBar;
    QGroupBox *m_materialsGroup;
};

// 主工作升级面板
class WorkUpgradePanel : public QWidget
{
    Q_OBJECT

public:
    explicit WorkUpgradePanel(PetViewModel *viewModel, CommandManager &commandManager, QWidget *parent = nullptr);
    ~WorkUpgradePanel();

    void refreshAll();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onUpgradeRequested(WorkType workType);
    void onWorkSystemUpgraded(WorkType workType, WorkSystemLevel newLevel);

private:
    void setupUI();
    void setupOverview();
    void setupUpgradeCards();
    void connectSignals();
    void updateOverview();
    QString getLevelName(WorkSystemLevel level) const; // 添加缺失的方法声明

private:
    PetViewModel *m_viewModel;
    CommandManager &m_commandManager;

    // UI组件
    QWidget *m_overviewWidget;
    QWidget *m_upgradeCardsWidget;
    QVBoxLayout *m_upgradeCardsLayout;

    // 概览标签
    QLabel *m_photosynthesisLevelLabel;
    QLabel *m_miningLevelLabel;
    QLabel *m_adventureLevelLabel;

    // 升级卡片
    QVector<WorkSystemUpgradeCard *> m_upgradeCards;
};

#endif // __WORK_UPGRADE_PANEL_H__