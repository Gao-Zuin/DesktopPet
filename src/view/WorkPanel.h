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
#include "../model/base/WorkInfo.h"  // 只包含数据结构

// 前向声明
class PetViewModel;

// 打工项控件
class WorkItemWidget : public QWidget {
    Q_OBJECT
public:
    explicit WorkItemWidget(const WorkInfo& workInfo, QWidget *parent = nullptr);
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
    QLabel* m_rewardLabel;      // 奖励信息
    QLabel* m_statusLabel;      // 状态信息
    QProgressBar* m_progressBar; // 进度条
    QPushButton* m_startButton; // 开始按钮
    QPushButton* m_stopButton;  // 停止按钮
    
    bool m_isWorking;           // 是否正在工作
};

// 打工面板主控件
class WorkPanel : public QWidget {
    Q_OBJECT

public:
    explicit WorkPanel(CommandManager& command_manager, PetViewModel& view_model, QWidget *parent = nullptr);
    ~WorkPanel();

    // 更新显示
    void updateDisplay();
    
    // 通知回调
    PropertyNotification getNotification() const noexcept {
        return &notification_cb;
    }

private slots:
    void onStartWork(WorkType type);
    void onStopWork();

private:
    void setupUi();
    void updateWorkItems();
    
    // 通知回调
    static void notification_cb(uint32_t id, void *p);

private:
    // 模型和命令管理器
    CommandManager& m_command_manager;
    PetViewModel& m_view_model;  // 改为引用ViewModel
    
    // UI组件
    QVBoxLayout* m_mainLayout;
    QLabel* m_titleLabel;
    QVector<WorkItemWidget*> m_workItems;
    
    // 状态显示
    QLabel* m_statusLabel;
};

#endif // WORKPANEL_H