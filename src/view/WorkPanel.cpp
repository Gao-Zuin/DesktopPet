#include "WorkPanel.h"
#include "../viewmodel/PetViewModel.h"
#include "../common/PropertyIds.h"
#include "../common/CommandParameters.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTextStream>

// ===================== WorkItemWidget 实现 =====================

WorkItemWidget::WorkItemWidget(const WorkInfo &workInfo, QWidget *parent)
    : QWidget(parent), m_workInfo(workInfo), m_isWorking(false)
{
    setupUi();
    updateButtonStates();
}

void WorkItemWidget::setupUi()
{
    setFixedSize(450, 140); // 再次增加高度，从120改为140

    // 改进样式，确保在深色模式下也有良好的对比度
    setStyleSheet(
        "WorkItemWidget {"
        "    background-color: #ffffff;"
        "    border: 2px solid #e0e0e0;"
        "    border-radius: 12px;"
        "    margin: 8px;"
        "}"
        "WorkItemWidget:hover {"
        "    border-color: #1976d2;"
        "    background-color: #f8f9fa;"
        "    box-shadow: 0 4px 12px rgba(25, 118, 210, 0.2);"
        "}"
    );

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15); // 增加上下边距
    mainLayout->setSpacing(15);

    // 左侧：桌宠形态图片
    m_petFormLabel = new QLabel(this);
    m_petFormLabel->setFixedSize(80, 80);
    m_petFormLabel->setAlignment(Qt::AlignCenter);
    m_petFormLabel->setStyleSheet(
        "QLabel {"
        "    background-color: #f5f5f5;"
        "    border: 2px solid #d0d0d0;"
        "    border-radius: 8px;"
        "    padding: 6px;"
        "}"
    );

    // 设置桌宠形态图片
    QPixmap petPixmap(m_workInfo.petFormImage);
    if (!petPixmap.isNull())
    {
        petPixmap = petPixmap.scaled(68, 68, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_petFormLabel->setPixmap(petPixmap);
    }
    else
    {
        m_petFormLabel->setText(m_workInfo.petForm);
        m_petFormLabel->setStyleSheet(m_petFormLabel->styleSheet() + 
            "font-size: 14px; font-weight: bold; color: #333333;");
    }
    mainLayout->addWidget(m_petFormLabel);

    // 中间：工作信息 - 重新调整布局
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(8); // 增加间距
    infoLayout->setContentsMargins(0, 0, 0, 0);

    // 工作名称
    m_nameLabel = new QLabel(m_workInfo.name, this);
    m_nameLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    color: #1a1a1a;"
        "    margin: 0;"
        "    padding: 0;"
        "}"
    );
    m_nameLabel->setWordWrap(true);
    m_nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    infoLayout->addWidget(m_nameLabel);

    // 工作描述 - 关键修复：给描述更多空间
    m_descLabel = new QLabel(m_workInfo.description, this);
    m_descLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 12px;"
        "    font-weight: 500;"
        "    color: #2c2c2c;"
        "    line-height: 1.4;"
        "    padding: 0;"
        "    margin: 0;"
        "    background-color: transparent;"
        "}"
    );
    m_descLabel->setWordWrap(true);
    m_descLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_descLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_descLabel->setMinimumHeight(40); // 确保至少有40px高度
    m_descLabel->setMaximumHeight(60); // 限制最大高度，防止过度拉伸
    infoLayout->addWidget(m_descLabel);

    // 创建底部信息的水平布局
    QHBoxLayout *bottomInfoLayout = new QHBoxLayout();
    bottomInfoLayout->setSpacing(10);
    bottomInfoLayout->setContentsMargins(0, 0, 0, 0);

    // 奖励信息 - 紧凑设计
    m_rewardLabel = new QLabel(QString("💰 奖励经验: +%1").arg(m_workInfo.experienceReward), this);
    m_rewardLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 11px;"
        "    font-weight: bold;"
        "    color: #1b5e20;"
        "    background-color: #e8f5e8;"
        "    border: 1px solid #4caf50;"
        "    border-radius: 4px;"
        "    padding: 2px 6px;"
        "    margin: 0;"
        "}"
    );
    m_rewardLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    bottomInfoLayout->addWidget(m_rewardLabel);

    // 状态信息 - 紧凑设计
    m_statusLabel = new QLabel("📊 状态: 空闲", this);
    m_statusLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 11px;"
        "    font-weight: bold;"
        "    color: #424242;"
        "    padding: 0;"
        "    margin: 0;"
        "}"
    );
    m_statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    bottomInfoLayout->addWidget(m_statusLabel);

    infoLayout->addLayout(bottomInfoLayout);

    // 进度条 - 紧凑设计
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);
    m_progressBar->setFixedHeight(14); // 进一步减少高度
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 1px solid #bdbdbd;"
        "    border-radius: 3px;"
        "    text-align: center;"
        "    font-size: 9px;"
        "    font-weight: bold;"
        "    color: #1a1a1a;"
        "    background-color: #f5f5f5;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                                      stop:0 #2196f3, stop:1 #1976d2);"
        "    border-radius: 2px;"
        "}"
    );
    infoLayout->addWidget(m_progressBar);

    // 设置中间区域占用更多空间
    mainLayout->addLayout(infoLayout, 1);

    // 右侧：操作按钮
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(10);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    m_startButton = new QPushButton("🚀 开始", this);
    m_startButton->setFixedSize(80, 35); // 稍微增加高度
    m_startButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #1976d2;"
        "    color: white;"
        "    border: none;"
        "    padding: 8px 10px;"
        "    border-radius: 6px;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1565c0;"
        "    transform: translateY(-1px);"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0d47a1;"
        "    transform: translateY(0px);"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdbdbd;"
        "    color: #757575;"
        "}"
    );
    connect(m_startButton, &QPushButton::clicked, this, &WorkItemWidget::onStartButtonClicked);
    buttonLayout->addWidget(m_startButton);

    m_stopButton = new QPushButton("⏹ 停止", this);
    m_stopButton->setFixedSize(80, 35);
    m_stopButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #d32f2f;"
        "    color: white;"
        "    border: none;"
        "    padding: 8px 10px;"
        "    border-radius: 6px;"
        "    font-weight: bold;"
        "    font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c62828;"
        "    transform: translateY(-1px);"
        "}"
        "QPushButton:pressed {"
        "    background-color: #b71c1c;"
        "    transform: translateY(0px);"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdbdbd;"
        "    color: #757575;"
        "}"
    );
    connect(m_stopButton, &QPushButton::clicked, this, &WorkItemWidget::onStopButtonClicked);
    buttonLayout->addWidget(m_stopButton);

    // 添加弹性空间，让按钮居中对齐
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
}

void WorkItemWidget::updateWorkStatus(WorkStatus status, WorkType currentType, int remainingTime)
{
    m_isWorking = (status == WorkStatus::Working && currentType == m_workInfo.type);

    if (m_isWorking)
    {
        m_statusLabel->setText(QString("状态: 连续工作中 (%1)").arg(formatTime(remainingTime)));
        m_progressBar->setVisible(true);

        // 计算进度百分比
        int progress = 100;
        if (m_workInfo.workDuration > 0)
        {
            progress = ((m_workInfo.workDuration - remainingTime) * 100) / m_workInfo.workDuration;
        }
        m_progressBar->setValue(progress);
    }
    else
    {
        m_statusLabel->setText("状态: 空闲");
        m_progressBar->setVisible(false);
    }

    updateButtonStates();
}

void WorkItemWidget::updateButtonStates()
{
    if (m_isWorking)
    {
        m_startButton->setText("取消打工");
        m_startButton->setEnabled(true);
        m_stopButton->setVisible(false);
    }
    else
    {
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
    if (m_isWorking)
    {
        // 如果正在工作，则发送停止工作信号
        emit stopWorkRequested();
    }
    else
    {
        // 如果空闲，则发送开始工作信号
        emit startWorkRequested(m_workInfo.type);
    }
}

void WorkItemWidget::onStopButtonClicked()
{
    emit stopWorkRequested();
}

// ===================== WorkPanel 实现 =====================

WorkPanel::WorkPanel(CommandManager &command_manager, PetViewModel &view_model, QWidget *parent)
    : QWidget(parent), m_command_manager(command_manager), m_view_model(view_model)
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
    setWindowTitle("💼 打工系统");
    setMinimumSize(520, 550); // 增加最小尺寸以适应新的WorkItem大小
    setMaximumSize(650, 900); // 增加最大尺寸
    setWindowFlags(Qt::Tool | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
    setAttribute(Qt::WA_DeleteOnClose, true);

    // 设置整体窗口样式，深色模式友好
    setStyleSheet(
        "WorkPanel {"
        "    background-color: #fafafa;"
        "    border: 2px solid #e0e0e0;"
        "    border-radius: 8px;"
        "}");

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    // 标题区域 - 改进深色模式兼容性
    QWidget *headerWidget = new QWidget(this);
    headerWidget->setStyleSheet(
        "QWidget {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                                      stop:0 #1976d2, stop:1 #1565c0);"
        "    border-radius: 12px;"
        "    padding: 20px;"
        "}");
    QVBoxLayout *headerLayout = new QVBoxLayout(headerWidget);

    m_titleLabel = new QLabel("💼 打工系统", this);
    m_titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 24px;"
        "    font-weight: bold;"
        "    color: white;"
        "    margin: 0;"
        "    padding: 5px 0;"
        "}");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(m_titleLabel);

    QLabel *subtitleLabel = new QLabel("选择适合的工作类型，通过努力获得经验值和奖励", this);
    subtitleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 14px;"
        "    font-weight: 500;"
        "    color: #e3f2fd;"
        "    margin: 0;"
        "    padding: 2px 0;"
        "}");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(subtitleLabel);

    m_mainLayout->addWidget(headerWidget);

    // 工作项容器 - 使用滚动区域，改进样式
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "    border: 2px solid #e0e0e0;"
        "    border-radius: 8px;"
        "    background-color: #ffffff;"
        "    padding: 5px;"
        "}"
        "QScrollBar:vertical {"
        "    background-color: #f5f5f5;"
        "    width: 14px;"
        "    border-radius: 7px;"
        "    margin: 2px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: #bdbdbd;"
        "    border-radius: 6px;"
        "    min-height: 30px;"
        "    margin: 1px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background-color: #9e9e9e;"
        "}"
        "QScrollBar::handle:vertical:pressed {"
        "    background-color: #757575;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    border: none;"
        "    background: none;"
        "}");

    QWidget *scrollWidget = new QWidget();
    scrollWidget->setStyleSheet("background-color: transparent;");
    QVBoxLayout *workLayout = new QVBoxLayout(scrollWidget);
    workLayout->setSpacing(20); // 增加间距
    workLayout->setContentsMargins(15, 15, 15, 15);

    // 创建工作项控件
    const QVector<WorkInfo> &workTypes = m_view_model.get_work_types();
    for (int i = 0; i < workTypes.size(); ++i)
    {
        const WorkInfo &workInfo = workTypes[i];
        WorkItemWidget *workItem = new WorkItemWidget(workInfo, this);
        connect(workItem, &WorkItemWidget::startWorkRequested, this, &WorkPanel::onStartWork);
        connect(workItem, &WorkItemWidget::stopWorkRequested, this, &WorkPanel::onStopWork);

        m_workItems.append(workItem);
        workLayout->addWidget(workItem);

        // 在工作项之间添加更明显的分隔线（除了最后一个）
        if (i < workTypes.size() - 1)
        {
            QFrame *separator = new QFrame();
            separator->setFrameShape(QFrame::HLine);
            separator->setFrameShadow(QFrame::Sunken);
            separator->setFixedHeight(2);
            separator->setStyleSheet(
                "QFrame {"
                "    background-color: #e0e0e0;"
                "    border: none;"
                "    margin: 10px 30px;"
                "}");
            workLayout->addWidget(separator);
        }
    }

    workLayout->addStretch();
    scrollArea->setWidget(scrollWidget);
    m_mainLayout->addWidget(scrollArea);

    // 状态显示区域 - 减少高度到原来的2/5
    QWidget *statusWidget = new QWidget(this);
    statusWidget->setFixedHeight(40); // 设置固定高度，大约是原来的2/5
    statusWidget->setStyleSheet(
        "QWidget {"
        "    background-color: #f5f5f5;"
        "    border: 2px solid #e0e0e0;"
        "    border-radius: 8px;"
        "    padding: 8px 15px;" // 减少内边距
        "}");
    QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);
    statusLayout->setContentsMargins(0, 0, 0, 0); // 移除额外边距
    statusLayout->setSpacing(10);                 // 减少间距

    QLabel *statusIcon = new QLabel("📊", this);
    statusIcon->setStyleSheet(
        "QLabel {"
        "    font-size: 16px;" // 减小图标尺寸
        "    color: #424242;"
        "}");
    statusLayout->addWidget(statusIcon);

    m_statusLabel = new QLabel("当前状态: 空闲", this);
    m_statusLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 14px;" // 减小字体
        "    font-weight: bold;"
        "    color: #1a1a1a;"
        "}");
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();

    // 添加一个小的状态指示器
    QLabel *statusIndicator = new QLabel("●", this);
    statusIndicator->setStyleSheet(
        "QLabel {"
        "    font-size: 14px;" // 减小指示器尺寸
        "    color: #4caf50;"  // 绿色表示正常
        "}");
    statusLayout->addWidget(statusIndicator);

    m_mainLayout->addWidget(statusWidget);
}

void WorkPanel::updateDisplay()
{
    updateWorkItems();
}

void WorkPanel::updateWorkItems()
{
    WorkStatus status = m_view_model.get_current_work_status();
    WorkType currentType = m_view_model.get_current_work_type();
    int remainingTime = m_view_model.get_work_remaining_time();

    // 更新每个工作项的状态
    for (WorkItemWidget *item : m_workItems)
    {
        item->updateWorkStatus(status, currentType, remainingTime);
    }

    // 更新整体状态显示
    if (status == WorkStatus::Working)
    {
        const QVector<WorkInfo> &workTypes = m_view_model.get_work_types();
        const WorkInfo *workInfo = nullptr;
        for (const auto &info : workTypes)
        {
            if (info.type == currentType)
            {
                workInfo = &info;
                break;
            }
        }
        if (workInfo)
        {
            m_statusLabel->setText(QString("当前状态: 正在进行 %1").arg(workInfo->name));
        }
    }
    else
    {
        m_statusLabel->setText("当前状态: 空闲");
    }
}

void WorkPanel::onStartWork(WorkType type)
{
    qDebug() << "开始工作类型:" << static_cast<int>(type);

    // 发送开始工作命令
    ICommandBase *command = m_command_manager.get_command(CommandType::START_WORK);
    if (command)
    {
        StartWorkCommandParameter param(static_cast<int>(type));
        command->exec(&param);
    }
}

void WorkPanel::onStopWork()
{
    qDebug() << "停止工作";

    // 发送停止工作命令
    ICommandBase *command = m_command_manager.get_command(CommandType::STOP_WORK);
    if (command)
    {
        StopWorkCommandParameter param;
        command->exec(&param);
    }
}

void WorkPanel::notification_cb(uint32_t id, void *p)
{
    WorkPanel *panel = static_cast<WorkPanel *>(p);
    if (!panel)
        return;

    if (id == PROP_ID_WORK_STATUS_UPDATE)
    {
        panel->updateDisplay();
    }
}