#ifndef __PET_MAIN_WINDOW_H__
#define __PET_MAIN_WINDOW_H__

#include <QWidget>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QSize>
#include <QTimer>
#include <QMovie>
#include "../common/CommandBase.h"
#include "../common/CommandManager.h"
#include "../common/PropertyTrigger.h"

class PetMainWindow : public QWidget
{
public:
    explicit PetMainWindow(CommandManager& command_manager, QWidget *parent = nullptr);
    PetMainWindow(const PetMainWindow&) = delete;
    ~PetMainWindow() noexcept
    {
        // 清理QMovie资源
        if (currentMovie) {
            currentMovie->stop();
            currentMovie->deleteLater();
            currentMovie = nullptr;
        }
    }

    PetMainWindow& operator=(const PetMainWindow&) = delete;

    // Properties
    void set_position(const QPoint *p) noexcept
    {
        m_position_ptr = p;
    }
    
    void set_animation(const QString *p) noexcept
    {
        m_animation_ptr = p;
    }
    
    void set_size(const QSize *p) noexcept
    {
        m_size_ptr = p;
    }

    // Notification
    PropertyNotification get_notification() const noexcept
    {
        return &notification_cb;
    }

    // Methods
    void update_ui();

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    // Callbacks - 使用C风格回调函数，就像Book项目
    static void switch_to_spider_cb(void *pv);
    static void switch_to_cassidy_cb(void *pv);
    static void show_stats_panel_cb(void *pv);
    static void show_backpack_panel_cb(void *pv);
    static void show_collection_panel_cb(void *pv);
    static void show_work_panel_cb(void *pv);
    static void exit_cb(void *pv);
    void updateDragPosition(); // 定时器更新拖动位置

private:
    // Notification
    static void notification_cb(uint32_t id, void *p);

private:
    QLabel *petLabel;
    QMenu *contextMenu;
    QTimer *dragUpdateTimer; // 拖动更新定时器
    
    // 资源管理 - 防止内存泄漏
    QMovie *currentMovie; // 当前的动画对象
    QString currentAnimationPath; // 当前动画路径，用于避免重复加载
    
    // Mouse dragging
    bool isDragging;
    QPoint dragStartPosition;
    QPoint pendingMovePosition; // 待更新的位置
    
    // Properties
    const QPoint* m_position_ptr;
    const QString* m_animation_ptr;
    const QSize* m_size_ptr;

    // Command Manager
    CommandManager& m_command_manager;

    void setupUI();
    void setupContextMenu();
};


#endif
