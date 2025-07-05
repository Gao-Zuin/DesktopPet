#include "PetMainWindow.h"
#include "../common/PropertyIds.h"
#include "../common/EventMgr.h"
#include "../common/EventDefine.h"
#include "../common/CommandParameters.h"
#include <QApplication>
#include <QScreen>
#include <QHBoxLayout>
#include <QMovie>
#include <QPixmap>
#include <QDebug>

PetMainWindow::PetMainWindow(CommandManager& command_manager, QWidget *parent)
    : QWidget(parent), petLabel(nullptr), contextMenu(nullptr), dragUpdateTimer(nullptr), currentMovie(nullptr), isDragging(false), m_position_ptr(nullptr), m_animation_ptr(nullptr), m_size_ptr(nullptr), m_command_manager(command_manager)
{
    setupUI();
    setupContextMenu();

    // 初始化拖动更新定时器
    dragUpdateTimer = new QTimer(this);
    dragUpdateTimer->setSingleShot(true);
    dragUpdateTimer->setInterval(16); // 约60fps的更新频率
    connect(dragUpdateTimer, &QTimer::timeout, this, &PetMainWindow::updateDragPosition);
}

void PetMainWindow::setupUI()
{
    // 设置窗口属性
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    // 创建布局和标签
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    petLabel = new QLabel(this);
    petLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(petLabel);

    // 修复：使用成员变量管理QMovie对象
    currentMovie = new QMovie(":/resources/gif/spider.gif", QByteArray(), this);
    currentAnimationPath = ":/resources/gif/spider.gif";
    petLabel->setMovie(currentMovie);
    currentMovie->start();

    resize(200, 200);
}

void TestTriggerEvent(void *pv){
    TestEvent event;
    event.TestInt = 5;
    event.TestString = "你好啊";
    EventMgr::GetInstance().SendEvent(event);
}

void PetMainWindow::setupContextMenu()
{
    contextMenu = new QMenu(this);

    // 显示数值面板
    QAction *statsAction = contextMenu->addAction("显示数值面板");
    connect(statsAction, &QAction::triggered, [this]()
            { show_stats_panel_cb(this); });

    QAction *backpackAction = contextMenu->addAction("显示背包");
    connect(backpackAction, &QAction::triggered, [this]()
            { show_backpack_panel_cb(this); });

    QAction *collectionAction = contextMenu->addAction("显示图鉴");
    connect(collectionAction, &QAction::triggered, [this]()
            { show_collection_panel_cb(this); });

    contextMenu->addSeparator();

    // 切换桌宠菜单
    QMenu *petMenu = contextMenu->addMenu("Switch Pet");

    // 使用callback函数，就像Book项目中的按钮回调
    QAction *spiderAction = petMenu->addAction("Spider");
    connect(spiderAction, &QAction::triggered, [this]()
            { switch_to_spider_cb(this); });

    QAction *cassidyAction = petMenu->addAction("Cassidy");
    connect(cassidyAction, &QAction::triggered, [this]()
            { switch_to_cassidy_cb(this); });

    QAction *testEvent = petMenu->addAction("TestEvent");
    connect(testEvent, &QAction::triggered, [this]()
            { TestTriggerEvent(this); });

    contextMenu->addSeparator();
    QAction *exitAction = contextMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, [this]()
            { exit_cb(this); });
}

void PetMainWindow::update_ui()
{
    if (m_position_ptr)
    {
        move(*m_position_ptr);
    }

    if (m_size_ptr)
    {
        resize(*m_size_ptr);
    }

    // 更新动画或图片 - 修复内存泄漏问题
    if (m_animation_ptr && !m_animation_ptr->isEmpty())
    {
        // 检查是否需要更新动画（避免重复加载相同动画）
        if (currentAnimationPath != *m_animation_ptr)
        {
            currentAnimationPath = *m_animation_ptr;

            if (m_animation_ptr->endsWith(".gif"))
            {
                // 清理旧的QMovie对象
                if (currentMovie)
                {
                    currentMovie->stop();
                    currentMovie->deleteLater();
                    currentMovie = nullptr;
                }

                // 创建新的QMovie对象
                currentMovie = new QMovie(*m_animation_ptr, QByteArray(), this);
                petLabel->setMovie(currentMovie);
                currentMovie->start();
            }
            else
            {
                // 处理静态图片 - 清理QMovie对象
                if (currentMovie)
                {
                    currentMovie->stop();
                    currentMovie->deleteLater();
                    currentMovie = nullptr;
                }

                // 清除petLabel的movie，设置静态图片
                petLabel->setMovie(nullptr);

                QPixmap pixmap(*m_animation_ptr);
                if (!pixmap.isNull() && m_size_ptr)
                {
                    // 根据设定的大小缩放图片，保持宽高比
                    pixmap = pixmap.scaled(*m_size_ptr, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                }
                petLabel->setPixmap(pixmap);
            }
        }
    }
}

void PetMainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    contextMenu->exec(event->globalPos());
}

void PetMainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isDragging = true;
        dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void PetMainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && (event->buttons() & Qt::LeftButton))
    {
        QPoint newPos = event->globalPosition().toPoint() - dragStartPosition;

        // 立即更新UI位置，但延迟通知Model
        move(newPos);
        pendingMovePosition = newPos;

        // 启动定时器来节流Model更新
        if (!dragUpdateTimer->isActive())
        {
            dragUpdateTimer->start();
        }

        event->accept();
    }
}

void PetMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isDragging = false;

        // 确保最后的位置被更新到Model
        if (dragUpdateTimer->isActive())
        {
            dragUpdateTimer->stop();
            updateDragPosition();
        }

        event->accept();
    }
}

// Callbacks - 按照Book项目的模式实现
void PetMainWindow::switch_to_spider_cb(void *pv)
{
    PetMainWindow *pThis = (PetMainWindow *)pv;
    ICommandBase* command = pThis->m_command_manager.get_command(CommandType::SWITCH_PET);
    if (command)
    {
        SwitchPetCommandParameter param(PetType::Spider);
        command->exec(&param);
    }
}

void PetMainWindow::switch_to_cassidy_cb(void *pv)
{
    PetMainWindow *pThis = (PetMainWindow *)pv;
    ICommandBase* command = pThis->m_command_manager.get_command(CommandType::SWITCH_PET);
    if (command)
    {
        SwitchPetCommandParameter param(PetType::Cassidy);
        command->exec(&param);
    }
}

void PetMainWindow::show_stats_panel_cb(void *pv)
{
    PetMainWindow *pThis = (PetMainWindow *)pv;
    ICommandBase* command = pThis->m_command_manager.get_command(CommandType::SHOW_STATS_PANEL);
    if (command)
    {
        ShowStatsPanelCommandParameter param;
        command->exec(&param);
    }
}

void PetMainWindow::show_backpack_panel_cb(void *pv)
{
    PetMainWindow *pThis = (PetMainWindow *)pv;
    ICommandBase* command = pThis->m_command_manager.get_command(CommandType::SHOW_BACKPACK_PANEL);
    if (command)
    {
        ShowBackpackPanelCommandParameter param;
        command->exec(&param);
    }
}

void PetMainWindow::show_collection_panel_cb(void *pv)
{
    PetMainWindow *pThis = (PetMainWindow *)pv;
    ICommandBase* command = pThis->m_command_manager.get_command(CommandType::SHOW_COLLECTION_PANEL);
    if (command)
    {
        ShowCollectionPanelCommandParameter param;
        command->exec(&param);
    }
}

void PetMainWindow::exit_cb(void *pv)
{
    QApplication::quit();
}

void PetMainWindow::updateDragPosition()
{
    // 只有在拖动时才更新Model
    ICommandBase* command = m_command_manager.get_command(CommandType::MOVE_PET);
    if (command)
    {
        MoveCommandParameter param(pendingMovePosition);
        command->exec(&param);
    }
}

void PetMainWindow::notification_cb(uint32_t id, void *p)
{
    PetMainWindow *pThis = (PetMainWindow *)p;

    switch (id)
    {
    case PROP_ID_PET_POSITION:
        // 位置变化时只更新位置，不重新加载动画
        if (pThis->m_position_ptr) {
            pThis->move(*pThis->m_position_ptr);
        }
        break;
    case PROP_ID_PET_SIZE:
        // 尺寸变化时只更新尺寸，不重新加载动画
        if (pThis->m_size_ptr) {
            pThis->resize(*pThis->m_size_ptr);
        }
        break;
    case PROP_ID_PET_ANIMATION:
    case PROP_ID_PET_STATE:
    case PROP_ID_PET_VISIBLE:
    case PROP_ID_PET_TYPE:
        // 只有动画、状态、可见性和类型变化时才需要完整的UI更新
        pThis->update_ui();
        break;
    default:
        break;
    }
}
