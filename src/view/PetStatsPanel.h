#ifndef __PET_STATS_PANEL_H__
#define __PET_STATS_PANEL_H__

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "../common/CommandBase.h"
#include "../common/CommandManager.h"
#include "../common/PropertyTrigger.h"

class PetStatsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit PetStatsPanel(CommandManager& command_manager, QWidget *parent = nullptr);
    ~PetStatsPanel();

    void update_display();

    // Properties - 类似于PetMainWindow的设计
    void set_level(int level) noexcept
    {
        m_level = level;
    }
    
    void set_experience(int exp) noexcept
    {
        m_experience = exp;
    }
    
    void set_experience_to_next_level(int exp) noexcept
    {
        m_experience_to_next_level = exp;
    }
    
    void set_money(int money) noexcept
    {
        m_money = money;
    }
    
    void set_name(const QString& name) noexcept
    {
        m_name = name;
    }

    // Notification
    PropertyNotification get_notification() const noexcept
    {
        return &notification_cb;
    }

private slots:
    void on_add_exp_clicked();
    void on_add_money_clicked();

private:
    void setup_ui();
    void connect_signals();
    
    // Notification callback
    static void notification_cb(uint32_t id, void *p);

private:
    // Data
    int m_level;
    int m_experience;
    int m_experience_to_next_level;
    int m_money;
    QString m_name;
    
    // Command Manager
    CommandManager& m_command_manager;
    
    // UI组件
    QLabel* m_name_label;
    QLabel* m_level_label;
    QLabel* m_money_label;
    QProgressBar* m_exp_progress;
    QLabel* m_exp_label;
    
    // 测试按钮
    QPushButton* m_add_exp_btn;
    QPushButton* m_add_money_btn;
    
    // 布局
    QVBoxLayout* m_main_layout;
    QGroupBox* m_stats_group;
    QGroupBox* m_test_group;
};

#endif // __PET_STATS_PANEL_H__
