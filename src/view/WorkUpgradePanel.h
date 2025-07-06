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
#include <QMap>
#include "../common/ForgeTypes.h"
#include "../common/CommandManager.h"
#include "../common/PropertyTrigger.h"

// 工作系统升级显示信息结构体 - View层需要的显示信息
struct WorkSystemDisplayInfo
{
    WorkType workType;
    WorkSystemLevel currentLevel;
    QString workTypeName;
    QString currentLevelName;
    QString nextLevelName;
    bool canUpgrade;
    QVector<ForgeMaterial> upgradeMaterials;
    QMap<int, int> ownedMaterials; // itemId -> count
    QMap<int, QString> materialNames; // itemId -> name
    
    WorkSystemDisplayInfo() : workType(WorkType::Photosynthesis), currentLevel(WorkSystemLevel::Basic), canUpgrade(false) {}
};

// 单个工作系统升级卡片 - 解耦，不依赖ViewModel
class WorkSystemUpgradeCard : public QFrame
{
    Q_OBJECT

public:
    explicit WorkSystemUpgradeCard(WorkType workType, QWidget *parent = nullptr);

    void updateDisplayInfo(const WorkSystemDisplayInfo& displayInfo);
    bool canUpgrade() const { return m_canUpgrade; }

signals:
    void upgradeRequested(WorkType workType);

private slots:
    void onUpgradeClicked();

private:
    void setupUI();
    void updateDisplay();
    QString getWorkTypeName(WorkType workType) const;
    QString getLevelName(WorkSystemLevel level) const;

private:
    WorkType m_workType;
    WorkSystemDisplayInfo m_displayInfo;
    bool m_canUpgrade;

    // UI组件
    QLabel *m_titleLabel;
    QLabel *m_currentLevelLabel;
    QLabel *m_materialsTextLabel; // 材料文本标签
    QPushButton *m_upgradeButton;
    QGroupBox *m_materialsGroup;
};

// 主工作升级面板 - 完全解耦，不依赖任何ViewModel
class WorkUpgradePanel : public QWidget
{
    Q_OBJECT

public:
    explicit WorkUpgradePanel(CommandManager &commandManager, QWidget *parent = nullptr);
    ~WorkUpgradePanel();

    // 通知回调接口
    PropertyNotification getNotification() const noexcept
    {
        return &notification_cb;
    }

    // 数据更新接口 - 由外部调用更新数据
    void updateWorkSystemDisplayInfo(const QVector<WorkSystemDisplayInfo>& workSystemsInfo);
    void refreshDisplay(); // 批量更新后调用此方法刷新显示

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onUpgradeRequested(WorkType workType);

private:
    void setupUI();
    void setupOverview();
    void setupUpgradeCards();
    void updateOverview();
    QString getLevelName(WorkSystemLevel level) const;
    
    // 静态通知回调函数
    static void notification_cb(uint32_t id, void *p);

private:
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
    
    // 数据存储 - View层自己管理显示数据
    QVector<WorkSystemDisplayInfo> m_workSystemsInfo;
};

#endif // __WORK_UPGRADE_PANEL_H__