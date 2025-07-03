#include "PetStatsPanel.h"
#include "../common/PropertyIds.h"
#include "../common/CommandParameters.h"
#include <QApplication>

PetStatsPanel::PetStatsPanel(CommandManager& command_manager, QWidget *parent)
    : QWidget(parent)
    , m_level(1)
    , m_experience(0)
    , m_experience_to_next_level(100)
    , m_money(0)
    , m_name("Pet")
    , m_command_manager(command_manager)
    , m_name_label(nullptr)
    , m_level_label(nullptr)
    , m_money_label(nullptr)
    , m_exp_progress(nullptr)
    , m_exp_label(nullptr)
    , m_add_exp_btn(nullptr)
    , m_add_money_btn(nullptr)
    , m_main_layout(nullptr)
    , m_stats_group(nullptr)
    , m_test_group(nullptr)
{
    setup_ui();
    connect_signals();
    update_display();
}

PetStatsPanel::~PetStatsPanel()
{
}

void PetStatsPanel::setup_ui()
{
    setWindowTitle("宠物数值面板");
    setFixedSize(300, 400);
    
    // 设置窗口属性 - 使用Tool窗口避免程序退出
    setWindowFlags(Qt::Tool | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
    setAttribute(Qt::WA_DeleteOnClose, true);
    
    // 主布局
    m_main_layout = new QVBoxLayout(this);
    
    // 数值显示组
    m_stats_group = new QGroupBox("宠物数值", this);
    QVBoxLayout* stats_layout = new QVBoxLayout(m_stats_group);
    
    // 名称
    m_name_label = new QLabel("名称: ", m_stats_group);
    m_name_label->setStyleSheet("font-size: 14px; font-weight: bold;");
    stats_layout->addWidget(m_name_label);
    
    // 等级
    m_level_label = new QLabel("等级: 1", m_stats_group);
    m_level_label->setStyleSheet("font-size: 14px; color: #4CAF50;");
    stats_layout->addWidget(m_level_label);
    
    // 经验值
    m_exp_label = new QLabel("经验值: 0/100", m_stats_group);
    m_exp_label->setStyleSheet("font-size: 12px;");
    stats_layout->addWidget(m_exp_label);
    
    // 经验值进度条
    m_exp_progress = new QProgressBar(m_stats_group);
    m_exp_progress->setMinimum(0);
    m_exp_progress->setMaximum(100);
    m_exp_progress->setValue(0);
    m_exp_progress->setStyleSheet(
        "QProgressBar {"
        "   border: 2px solid #4CAF50;"
        "   border-radius: 5px;"
        "   text-align: center;"
        "   background-color: #f0f0f0;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #4CAF50;"
        "   border-radius: 3px;"
        "}"
    );
    stats_layout->addWidget(m_exp_progress);
    
    // 金钱
    m_money_label = new QLabel("金钱: 0", m_stats_group);
    m_money_label->setStyleSheet("font-size: 14px; color: #FF9800;");
    stats_layout->addWidget(m_money_label);
    
    m_main_layout->addWidget(m_stats_group);
    
    // 测试按钮组
    m_test_group = new QGroupBox("测试操作", this);
    QVBoxLayout* test_layout = new QVBoxLayout(m_test_group);
    
    m_add_exp_btn = new QPushButton("增加经验 (+10)", m_test_group);
    m_add_exp_btn->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1976D2;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #0D47A1;"
        "}"
    );
    test_layout->addWidget(m_add_exp_btn);
    
    m_add_money_btn = new QPushButton("增加金钱 (+50)", m_test_group);
    m_add_money_btn->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF9800;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F57C00;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #E65100;"
        "}"
    );
    test_layout->addWidget(m_add_money_btn);
    
    m_main_layout->addWidget(m_test_group);
    
    // 添加弹性空间
    m_main_layout->addStretch();
}

void PetStatsPanel::connect_signals()
{
    // 连接按钮信号
    QObject::connect(m_add_exp_btn, &QPushButton::clicked,
                    this, &PetStatsPanel::on_add_exp_clicked);
    QObject::connect(m_add_money_btn, &QPushButton::clicked,
                    this, &PetStatsPanel::on_add_money_clicked);
}

void PetStatsPanel::update_display()
{
    // 更新名称
    m_name_label->setText(QString("名称: %1").arg(m_name));
    
    // 更新等级
    m_level_label->setText(QString("等级: %1").arg(m_level));
    
    // 更新经验值
    m_exp_label->setText(QString("经验值: %1/%2").arg(m_experience).arg(m_experience_to_next_level));
    
    // 更新进度条
    if (m_experience_to_next_level > 0) {
        m_exp_progress->setMaximum(m_experience_to_next_level);
        m_exp_progress->setValue(m_experience);
    }
    
    // 更新金钱
    m_money_label->setText(QString("金钱: %1").arg(m_money));
}

void PetStatsPanel::notification_cb(uint32_t id, void *p)
{
    PetStatsPanel* panel = static_cast<PetStatsPanel*>(p);
    
    // 检查面板是否仍然有效
    if (!panel) return;
    
    switch (id) {
    case PROP_ID_PET_LEVEL:
    case PROP_ID_PET_EXPERIENCE:
    case PROP_ID_PET_MONEY:
    case PROP_ID_PET_TYPE:
        // 数据更新由PetApp的app_notification_cb处理
        // 这里不需要做任何事情，因为PetApp会在收到通知后更新面板数据
        break;
    default:
        break;
    }
}

void PetStatsPanel::on_add_exp_clicked()
{
    ICommandBase* command = m_command_manager.get_command(CommandType::ADD_EXPERIENCE);
    if (command) {
        AddExperienceCommandParameter param(10);
        command->exec(&param);
    }
}

void PetStatsPanel::on_add_money_clicked()
{
    ICommandBase* command = m_command_manager.get_command(CommandType::ADD_MONEY);
    if (command) {
        AddMoneyCommandParameter param(50);
        command->exec(&param);
    }
}
