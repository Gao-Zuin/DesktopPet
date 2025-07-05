#include "WorkPanel.h"
#include "../common/PropertyIds.h"
#include "../common/CommandParameters.h"
#include <QIcon>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTextStream>

// ===================== WorkItemWidget 实现 =====================

WorkItemWidget::WorkItemWidget(const WorkInfo& workInfo, QWidget *parent)
    : QWidget(parent), m_workInfo(workInfo), m_isWorking(false)
{
    setupUi();
    updateButtonStates();
}

void WorkItemWidget::setupUi()
{
    setFixedSize(400, 120);
    setStyleSheet("background-color: #f8f9fa; border: 1px solid #dee2e6; border-radius: 8px;");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);

    // 左侧：桌宠形态图片
    m_petFormLabel = new QLabel(this);
    m_petFormLabel->setFixedSize(80, 80);
    m_petFormLabel->setAlignment(Qt::AlignCenter);
    m_petFormLabel->setStyleSheet("background-color: #e9ecef; border: 1px solid #ced4da; border-radius: 4px;");
    
    // 设置桌宠形态图片
    QPixmap petPixmap(m_workInfo.petFormImage);
    if (!petPixmap.isNull()) {
        petPixmap = petPixmap.scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_petFormLabel->setPixmap(petPixmap);
    } else {
        m_petFormLabel->setText(m_workInfo.petForm);
        m_petFormLabel->setStyleSheet(m_petFormLabel->styleSheet() + "font-size: 12px; font-weight: bold;");
    }
    mainLayout->addWidget(m_petFormLabel);

    // 中间：工作信息
    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(5);

    m_nameLabel = new QLabel(m_workInfo.name, this);
    m_nameLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #212529;");
    infoLayout->addWidget(m_nameLabel);

    m_descLabel = new QLabel(m_workInfo.description, this);
    m_descLabel->setStyleSheet("font-size: 12px; color: #6c757d;");
    m_descLabel->setWordWrap(true);
    infoLayout->addWidget(m_descLabel);

    m_rewardLabel = new QLabel(QString("奖励: +%1 经验值").arg(m_workInfo.experienceReward), this);
    m_rewardLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #28a745;");
    infoLayout->addWidget(m_rewardLabel);

    m_statusLabel = new QLabel("状态: 空闲", this);
    m_statusLabel->setStyleSheet("font-size: 12px; color: #495057;");
    infoLayout->addWidget(m_statusLabel);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);
    infoLayout->addWidget(m_progressBar);

    mainLayout->addLayout(infoLayout);

    // 右侧：操作按钮
    QVBoxLayout* buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(10);

    m_startButton = new QPushButton("开始工作", this);
    m_startButton->setStyleSheet("QPushButton { background-color: #007bff; color: white; border: none; padding: 8px 16px; border-radius: 4px; font-weight: bold; } QPushButton:hover { background-color: #0056b3; }");
    connect(m_startButton, &QPushButton::clicked, this, &WorkItemWidget::onStartButtonClicked);
    buttonLayout->addWidget(m_startButton);

    m_stopButton = new QPushButton("停止工作", this);
    m_stopButton->setStyleSheet("QPushButton { background-color: #dc3545; color: white; border: none; padding: 8px 16px; border-radius: 4px; font-weight: bold; } QPushButton:hover { background-color: #c82333; }");
    connect(m_stopButton, &QPushButton::clicked, this, &WorkItemWidget::onStopButtonClicked);
    buttonLayout->addWidget(m_stopButton);

    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
}

void WorkItemWidget::updateWorkStatus(WorkStatus status, WorkType currentType, int remainingTime)
{
    m_isWorking = (status == WorkStatus::Working && currentType == m_workInfo.type);
    
    if (m_isWorking) {
        m_statusLabel->setText(QString("状态: 连续工作中 (%1)").arg(formatTime(remainingTime)));
        m_progressBar->setVisible(true);
        
        // 计算进度百分比
        int progress = 100;
        if (m_workInfo.workDuration > 0) {
            progress = ((m_workInfo.workDuration - remainingTime) * 100) / m_workInfo.workDuration;
        }
        m_progressBar->setValue(progress);
    } else {
        m_statusLabel->setText("状态: 空闲");
        m_progressBar->setVisible(false);
    }
    
    updateButtonStates();
}

void WorkItemWidget::updateButtonStates()
{
    if (m_isWorking) {
        m_startButton->setText("取消打工");
        m_startButton->setEnabled(true);
        m_stopButton->setVisible(false);
    } else {
        m_startButton->setText("开始工作");
        m_startButton->setEnabled(true);
        m_stopButton->setVisible(false);
    }
}

QString WorkItemWidget::formatTime(int seconds)
{
    int minutes = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
}

void WorkItemWidget::onStartButtonClicked()
{
    if (m_isWorking) {
        // 如果正在工作，则发送停止工作信号
        emit stopWorkRequested();
    } else {
        // 如果空闲，则发送开始工作信号
        emit startWorkRequested(m_workInfo.type);
    }
}

void WorkItemWidget::onStopButtonClicked()
{
    emit stopWorkRequested();
}

// ===================== WorkPanel 实现 =====================

WorkPanel::WorkPanel(CommandManager& command_manager, WorkModel& work_model, QWidget *parent)
    : QWidget(parent), m_work_model(work_model), m_command_manager(command_manager)
{
    setupUi();
    updateDisplay();
}

WorkPanel::~WorkPanel()
{
    qDeleteAll(m_workItems);
}

void WorkPanel::setupUi()
{
    setWindowTitle("打工系统");
    setFixedSize(450, 400);
    setWindowFlags(Qt::Tool | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
    setAttribute(Qt::WA_DeleteOnClose, true);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(15, 15, 15, 15);

    // 标题
    m_titleLabel = new QLabel("选择打工方式", this);
    m_titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #212529; margin-bottom: 10px;");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_titleLabel);

    // 工作项容器
    QGroupBox* workGroup = new QGroupBox("可选工作", this);
    QVBoxLayout* workLayout = new QVBoxLayout(workGroup);
    workLayout->setSpacing(10);

    // 创建工作项控件
    const QVector<WorkInfo>& workTypes = m_work_model.getWorkTypes();
    for (const WorkInfo& workInfo : workTypes) {
        WorkItemWidget* workItem = new WorkItemWidget(workInfo, this);
        connect(workItem, &WorkItemWidget::startWorkRequested, this, &WorkPanel::onStartWork);
        connect(workItem, &WorkItemWidget::stopWorkRequested, this, &WorkPanel::onStopWork);
        
        m_workItems.append(workItem);
        workLayout->addWidget(workItem);
    }

    workLayout->addStretch();
    m_mainLayout->addWidget(workGroup);

    // 状态显示
    m_statusLabel = new QLabel("当前状态: 空闲", this);
    m_statusLabel->setStyleSheet("font-size: 14px; color: #495057; padding: 10px; background-color: #f8f9fa; border-radius: 4px;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_mainLayout->addWidget(m_statusLabel);
}

void WorkPanel::updateDisplay()
{
    updateWorkItems();
}

void WorkPanel::updateWorkItems()
{
    WorkStatus status = m_work_model.getCurrentStatus();
    WorkType currentType = m_work_model.getCurrentWorkType();
    int remainingTime = m_work_model.getRemainingTime();

    // 更新每个工作项的状态
    for (WorkItemWidget* item : m_workItems) {
        item->updateWorkStatus(status, currentType, remainingTime);
    }

    // 更新整体状态显示
    if (status == WorkStatus::Working) {
        const WorkInfo* workInfo = m_work_model.getWorkInfo(currentType);
        if (workInfo) {
            m_statusLabel->setText(QString("当前状态: 正在进行 %1").arg(workInfo->name));
        }
    } else {
        m_statusLabel->setText("当前状态: 空闲");
    }
}

void WorkPanel::onStartWork(WorkType type)
{
    qDebug() << "开始工作类型:" << static_cast<int>(type);
    
    // 发送开始工作命令
    ICommandBase* command = m_command_manager.get_command(CommandType::START_WORK);
    if (command) {
        StartWorkCommandParameter param(static_cast<int>(type));
        command->exec(&param);
    }
}

void WorkPanel::onStopWork()
{
    qDebug() << "停止工作";
    
    // 发送停止工作命令
    ICommandBase* command = m_command_manager.get_command(CommandType::STOP_WORK);
    if (command) {
        StopWorkCommandParameter param;
        command->exec(&param);
    }
}

void WorkPanel::notification_cb(uint32_t id, void *p)
{
    WorkPanel* panel = static_cast<WorkPanel*>(p);
    if (!panel)
        return;

    if (id == PROP_ID_WORK_STATUS_UPDATE) {
        panel->updateDisplay();
    }
}