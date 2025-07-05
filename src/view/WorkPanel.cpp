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
    setFixedSize(450, 160); // 增加宽度和高度，为文字留出更多空间

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
        "}");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(20, 15, 20, 15);
    mainLayout->setSpacing(20);

    // 左侧：桌宠形态图片
    m_petFormLabel = new QLabel(this);
    m_petFormLabel->setFixedSize(100, 100);
    m_petFormLabel->setAlignment(Qt::AlignCenter);
    m_petFormLabel->setStyleSheet(
        "QLabel {"
        "    background-color: #f5f5f5;"
        "    border: 2px solid #d0d0d0;"
        "    border-radius: 10px;"
        "    padding: 8px;"
        "}");

    // 设置桌宠形态图片
    QPixmap petPixmap(m_workInfo.petFormImage);
    if (!petPixmap.isNull())
    {
        petPixmap = petPixmap.scaled(84, 84, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_petFormLabel->setPixmap(petPixmap);
    }
    else
    {
        m_petFormLabel->setText(m_workInfo.petForm);
        m_petFormLabel->setStyleSheet(m_petFormLabel->styleSheet() +
                                      "font-size: 16px; font-weight: bold; color: #333333;");
    }
    mainLayout->addWidget(m_petFormLabel);

    // 中间：工作信息 - 增加宽度
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(10);

    // 工作名称 - 加粗字体，高对比度
    m_nameLabel = new QLabel(m_workInfo.name, this);
    m_nameLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 20px;"
        "    font-weight: bold;"
        "    color: #1a1a1a;"
        "    margin: 0;"
        "    padding: 2px 0;"
        "}");
    m_nameLabel->setWordWrap(true);
    infoLayout->addWidget(m_nameLabel);

    // 工作描述 - 确保足够的高度和清晰度
    m_descLabel = new QLabel(m_workInfo.description, this);
    m_descLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 14px;"
        "    font-weight: 500;" // 中等加粗
        "    color: #2c2c2c;"
        "    line-height: 1.5;"
        "    padding: 4px 0;"
        "    background-color: transparent;"
        "}");
    m_descLabel->setWordWrap(true);
    m_descLabel->setMinimumHeight(50); // 确保有足够高度显示多行文字
    m_descLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    infoLayout->addWidget(m_descLabel);

    // 奖励信息 - 醒目的样式，深色模式友好
    m_rewardLabel = new QLabel(QString("💰 奖励经验: +%1").arg(m_workInfo.experienceReward), this);
    m_rewardLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: #1b5e20;" // 深绿色，确保可读性
        "    background-color: #e8f5e8;"
        "    border: 2px solid #4caf50;"
        "    border-radius: 6px;"
        "    padding: 6px 12px;"
        "    margin: 2px 0;"
        "}");
    infoLayout->addWidget(m_rewardLabel);

    // 状态信息 - 加粗字体
    m_statusLabel = new QLabel("📊 状态: 空闲", this);
    m_statusLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "    color: #424242;"
        "    padding: 3px 0;"
        "}");
    infoLayout->addWidget(m_statusLabel);

    // 进度条 - 改进样式
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setVisible(false);
    m_progressBar->setFixedHeight(20);
    m_progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: 2px solid #bdbdbd;"
        "    border-radius: 6px;"
        "    text-align: center;"
        "    font-size: 12px;"
        "    font-weight: bold;"
        "    color: #1a1a1a;"
        "    background-color: #f5f5f5;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "                                      stop:0 #2196f3, stop:1 #1976d2);"
        "    border-radius: 4px;"
        "}");
    infoLayout->addWidget(m_progressBar);

    mainLayout->addLayout(infoLayout);

    // 右侧：操作按钮 - 调整尺寸和样式
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setSpacing(15);

    m_startButton = new QPushButton("🚀 开始工作", this);
    m_startButton->setFixedSize(110, 40);
    m_startButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #1976d2;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 16px;"
        "    border-radius: 8px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1565c0;"
        "    transform: translateY(-2px);"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0d47a1;"
        "    transform: translateY(0px);"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdbdbd;"
        "    color: #757575;"
        "}");
    connect(m_startButton, &QPushButton::clicked, this, &WorkItemWidget::onStartButtonClicked);
    buttonLayout->addWidget(m_startButton);

    m_stopButton = new QPushButton("⏹ 停止工作", this);
    m_stopButton->setFixedSize(110, 40);
    m_stopButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #d32f2f;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 16px;"
        "    border-radius: 8px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c62828;"
        "    transform: translateY(-2px);"
        "}"
        "QPushButton:pressed {"
        "    background-color: #b71c1c;"
        "    transform: translateY(0px);"
        "}"
        "QPushButton:disabled {"
        "    background-color: #bdbdbd;"
        "    color: #757575;"
        "}");
    connect(m_stopButton, &QPushButton::clicked, this, &WorkItemWidget::onStopButtonClicked);
    buttonLayout->addWidget(m_stopButton);

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

    // 状态显示区域 - 改进样式和对比度
    QWidget *statusWidget = new QWidget(this);
    statusWidget->setStyleSheet(
        "QWidget {"
        "    background-color: #f5f5f5;"
        "    border: 2px solid #e0e0e0;"
        "    border-radius: 10px;"
        "    padding: 18px;"
        "}");
    QHBoxLayout *statusLayout = new QHBoxLayout(statusWidget);

    QLabel *statusIcon = new QLabel("📊", this);
    statusIcon->setStyleSheet(
        "QLabel {"
        "    font-size: 24px;"
        "    margin-right: 15px;"
        "    color: #424242;"
        "}");
    statusLayout->addWidget(statusIcon);

    m_statusLabel = new QLabel("当前状态: 空闲", this);
    m_statusLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    color: #1a1a1a;"
        "    padding: 2px 0;"
        "}");
    statusLayout->addWidget(m_statusLabel);
    statusLayout->addStretch();

    // 添加一个小的状态指示器
    QLabel *statusIndicator = new QLabel("●", this);
    statusIndicator->setStyleSheet(
        "QLabel {"
        "    font-size: 20px;"
        "    color: #4caf50;" // 绿色表示正常
        "    margin-left: 10px;"
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