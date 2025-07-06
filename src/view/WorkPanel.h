#ifndef WORKPANEL_H
#define WORKPANEL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>
#include <QTimer>
#include <QPixmap>
#include <QVector>
#include <QScrollArea>
#include <QFrame>
#include "../common/CommandManager.h"
#include "../common/PropertyTrigger.h"
#include "../common/base/WorkInfo.h" // 只包含数据结构

// 工作状态信息结构体 - View层需要的显示信息
struct WorkStatusInfo
{
    WorkStatus status;
    WorkType currentType;
    int remainingTime;
    QString statusText;
    
    WorkStatusInfo() : status(WorkStatus::Idle), currentType(WorkType::Photosynthesis), remainingTime(0) {}
    WorkStatusInfo(WorkStatus s, WorkType t, int time, const QString& text)
        : status(s), currentType(t), remainingTime(time), statusText(text) {}
};

// 打工项控件
class WorkItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WorkItemWidget(const WorkInfo &workInfo, QWidget *parent = nullptr);
    void updateWorkStatus(WorkStatus status, WorkType currentType, int remainingTime);
    WorkType getWorkType() const { return m_workInfo.type; }

signals:
    void startWorkRequested(WorkType type);
    void stopWorkRequested();

private slots:
    void onStartButtonClicked();
    void onStopButtonClicked();

private:
    void setupUi();
    void updateButtonStates();
    QString formatTime(int seconds);

private:
    WorkInfo m_workInfo;
    
    // UI组件
    QLabel* m_petFormLabel;     // 桌宠形态图片
    QLabel* m_nameLabel;        // 工作名称
    QLabel* m_descLabel;        // 工作描述
    QLabel* m_rewardLabel;      // 经验值奖励信息
    QLabel* m_itemRewardLabel;  // 物品奖励信息
    QLabel* m_timeLabel;        // 工作时间标签
    QLabel* m_rarityLabel;      // 稀有度概率标签
    QProgressBar* m_progressBar; // 进度条
    QPushButton* m_startButton; // 开始按钮
    QPushButton* m_stopButton;  // 停止按钮
    
    bool m_isWorking;           // 是否正在工作
};

// 打工面板主控件 - 完全解耦，不依赖任何ViewModel
class WorkPanel : public QWidget
{
    Q_OBJECT

public:
    explicit WorkPanel(CommandManager &command_manager, QWidget *parent = nullptr);
    ~WorkPanel();

    // 通知回调接口
    PropertyNotification getNotification() const noexcept
    {
        return &notification_cb;
    }

    // 数据更新接口 - 由外部调用更新数据
    void updateWorkTypes(const QVector<WorkInfo>& workTypes);
    void updateWorkStatus(const WorkStatusInfo& statusInfo);
    void refreshDisplay(); // 批量更新后调用此方法刷新显示

private slots:
    void onStartWork(WorkType type);
    void onStopWork();

private:
    void setupUi();
    void updateWorkItems();
    void updateStatusDisplay();
    
    // 静态通知回调函数
    static void notification_cb(uint32_t id, void *p);

private:
    CommandManager &m_command_manager;
    
    // UI组件
    QVBoxLayout *m_mainLayout;
    QLabel *m_titleLabel;
    QVector<WorkItemWidget *> m_workItems;
    QLabel *m_statusLabel;
    
    // 数据存储 - View层自己管理显示数据
    QVector<WorkInfo> m_workTypes;
    WorkStatusInfo m_currentStatus;
};

#endif // WORKPANEL_H