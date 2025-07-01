#include "PetMainWindow.h"
#include "../common/PropertyIds.h"
#include "../common/CommandParameters.h"
#include <QApplication>
#include <QScreen>
#include <QHBoxLayout>
#include <QMovie>
#include <QPixmap>
#include <QDebug>

PetMainWindow::PetMainWindow(QWidget *parent)
    : QWidget(parent)
    , petLabel(nullptr)
    , contextMenu(nullptr)
    , dragUpdateTimer(nullptr)
    , isDragging(false)
    , m_position_ptr(nullptr)
    , m_animation_ptr(nullptr)
    , m_size_ptr(nullptr)
    , m_move_command(nullptr)
    , m_switch_pet_command(nullptr)
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
    
    // 默认加载蜘蛛GIF
    QMovie *movie = new QMovie(":/resources/gif/spider.gif");
    petLabel->setMovie(movie);
    movie->start();
    
    resize(150, 150);
}

void PetMainWindow::setupContextMenu()
{
    contextMenu = new QMenu(this);
    
    // 切换桌宠菜单
    QMenu *petMenu = contextMenu->addMenu("Switch Pet");
    
    // 使用callback函数，就像Book项目中的按钮回调
    QAction *spiderAction = petMenu->addAction("Spider");
    connect(spiderAction, &QAction::triggered, [this]() { switch_to_spider_cb(this); });
    
    QAction *cassidyAction = petMenu->addAction("Cassidy");
    connect(cassidyAction, &QAction::triggered, [this]() { switch_to_cassidy_cb(this); });
    
    contextMenu->addSeparator();
    QAction *exitAction = contextMenu->addAction("Exit");
    connect(exitAction, &QAction::triggered, [this]() { exit_cb(this); });
}

void PetMainWindow::update_ui()
{
    if (m_position_ptr) {
        move(*m_position_ptr);
    }
    
    if (m_size_ptr) {
        resize(*m_size_ptr);
    }
    
    // 更新动画或图片
    if (m_animation_ptr && !m_animation_ptr->isEmpty()) {
        if (m_animation_ptr->endsWith(".gif")) {
            QMovie *movie = new QMovie(*m_animation_ptr);
            petLabel->setMovie(movie);
            movie->start();
        } else {
            QPixmap pixmap(*m_animation_ptr);
            petLabel->setPixmap(pixmap);
        }
    }
}

void PetMainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    contextMenu->exec(event->globalPos());
}

void PetMainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        dragStartPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void PetMainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = event->globalPosition().toPoint() - dragStartPosition;
        
        // 立即更新UI位置，但延迟通知Model
        move(newPos);
        pendingMovePosition = newPos;
        
        // 启动定时器来节流Model更新
        if (!dragUpdateTimer->isActive()) {
            dragUpdateTimer->start();
        }
        
        event->accept();
    }
}

void PetMainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        
        // 确保最后的位置被更新到Model
        if (dragUpdateTimer->isActive()) {
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
    if (pThis->m_switch_pet_command) {
        SwitchPetCommandParameter param(PetType::Spider);
        pThis->m_switch_pet_command->exec(&param);
    }
}

void PetMainWindow::switch_to_cassidy_cb(void *pv)
{
    PetMainWindow *pThis = (PetMainWindow *)pv;
    if (pThis->m_switch_pet_command) {
        SwitchPetCommandParameter param(PetType::Cassidy);
        pThis->m_switch_pet_command->exec(&param);
    }
}

void PetMainWindow::exit_cb(void *pv)
{
    QApplication::quit();
}

void PetMainWindow::updateDragPosition()
{
    // 只有在拖动时才更新Model
    if (m_move_command) {
        MoveCommandParameter param(pendingMovePosition);
        m_move_command->exec(&param);
    }
}

void PetMainWindow::notification_cb(uint32_t id, void *p)
{
    PetMainWindow* pThis = (PetMainWindow*)p;
    
    switch (id) {
    case PROP_ID_PET_POSITION:
    case PROP_ID_PET_STATE:
    case PROP_ID_PET_ANIMATION:
    case PROP_ID_PET_VISIBLE:
    case PROP_ID_PET_SIZE:
        pThis->update_ui();
        break;
    default:
        break;
    }
}

#include "PetMainWindow.moc"
