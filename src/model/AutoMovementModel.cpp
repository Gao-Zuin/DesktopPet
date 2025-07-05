#include "AutoMovementModel.h"
#include "PetModel.h"
#include "../common/PropertyIds.h"
#include <QApplication>
#include <QScreen>
#include <QCursor>
#include <QRandomGenerator>
#include <QDebug>
#include <QtMath>
#include <cmath>
#include <Windows.h>
#include <CommCtrl.h>
#include <shellapi.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AutoMovementModel::AutoMovementModel(QObject *parent)
    : QObject(parent)
    , m_isActive(false)
    , m_isPaused(false)
    , m_updateTimer(new QTimer(this))
    , m_pauseTimer(new QTimer(this))
    , m_cornerStayTimer(new QTimer(this))
    , m_interactionTimer(new QTimer(this))
    , m_iconRefreshTimer(new QTimer(this))
    , m_petSize(DEFAULT_PET_WIDTH, DEFAULT_PET_HEIGHT)
    , m_isInteracting(false)
    , m_iconRefreshInterval(5000)  // 5秒刷新一次图标信息
{
    // 初始化定时器
    m_updateTimer->setSingleShot(false);
    m_pauseTimer->setSingleShot(true);
    m_cornerStayTimer->setSingleShot(true);
    m_interactionTimer->setSingleShot(true);
    m_iconRefreshTimer->setSingleShot(false);
    
    // 连接信号槽
    connect(m_updateTimer, &QTimer::timeout, this, &AutoMovementModel::updateMovement);
    connect(m_pauseTimer, &QTimer::timeout, this, &AutoMovementModel::onPauseTimeout);
    connect(m_interactionTimer, &QTimer::timeout, this, &AutoMovementModel::onInteractionComplete);
    connect(m_iconRefreshTimer, &QTimer::timeout, this, &AutoMovementModel::refreshDesktopIconsSlot);
    
    // 初始化屏幕信息
    initializeScreenInfo();
    
    // 设置默认配置
    m_config.mode = AutoMovementMode::RandomMovement;
    m_config.speed = 10;
    m_config.updateInterval = 50;
    m_config.enableRandomPause = false;
    m_config.pauseProbability = 0;
    m_config.pauseDuration = 0;
    m_config.enableIconInteraction = true;
    m_config.iconInteractionProbability = 50;
    m_config.iconDetectionRadius = 50;
    m_config.interactionAnimation = ":/resources/gif/kicking.gif";
    m_config.interactionDuration = 1500;
    
    qDebug() << "AutoMovementModel initialized with screen rect:" << m_screenRect;
    
    // 初始化桌面图标信息（只在构造时做一次）
    refreshDesktopIcons();
    qDebug() << "Loaded" << m_desktopIcons.size() << "desktop icons for interaction";
}

AutoMovementModel::~AutoMovementModel()
{
    stopAutoMovement();
}

void AutoMovementModel::setConfig(const AutoMovementConfig& config)
{
    m_config = config;
    
    // 更新定时器间隔
    if (m_config.updateInterval > 0) {
        m_updateTimer->setInterval(m_config.updateInterval);
    }
    
    qDebug() << "AutoMovement config updated:"
             << "mode=" << static_cast<int>(m_config.mode)
             << "speed=" << m_config.speed
             << "interval=" << m_config.updateInterval;
}

void AutoMovementModel::startAutoMovement()
{
    if (m_isActive || !m_petModel || m_config.mode == AutoMovementMode::Disabled) {
        qDebug() << "Cannot start auto movement - already active:" << m_isActive 
                 << "has model:" << (m_petModel != nullptr) 
                 << "mode:" << static_cast<int>(m_config.mode);
        return;
    }
    
    m_isActive = true;
    m_isPaused = false;
    m_isInteracting = false;
    
    // 设置初始状态
    if (m_petModel) {
        m_lastPosition = m_petModel->get_info()->position;
        m_petSize = m_petModel->get_info()->size;
        m_originalAnimation = m_petModel->get_info()->currentAnimation;
        qDebug() << "Pet current position:" << m_lastPosition << "size:" << m_petSize;
    }
    
    // 设置初始随机目标
    m_currentTarget = getRandomScreenPoint();
    qDebug() << "RandomMovement mode - initial target:" << m_currentTarget;
    
    // 启动定时器
    m_updateTimer->start(m_config.updateInterval);
    
    // 启动图标刷新定时器（低频率）
    if (m_config.enableIconInteraction) {
        m_iconRefreshTimer->start(m_iconRefreshInterval);
        qDebug() << "Icon refresh timer started with interval:" << m_iconRefreshInterval << "ms";
    }
    
    qDebug() << "AutoMovement started successfully with mode:" << static_cast<int>(m_config.mode)
             << "update interval:" << m_config.updateInterval
             << "initial target:" << m_currentTarget;
}

void AutoMovementModel::stopAutoMovement()
{
    if (!m_isActive) {
        return;
    }
    
    m_isActive = false;
    m_isPaused = false;
    
    // 停止所有定时器
    m_updateTimer->stop();
    m_pauseTimer->stop();
    m_cornerStayTimer->stop();
    m_iconRefreshTimer->stop();
    
    qDebug() << "AutoMovement stopped";
}

void AutoMovementModel::setPetModel(std::shared_ptr<PetModel> petModel)
{
    m_petModel = petModel;
    if (m_petModel) {
        m_petSize = m_petModel->get_info()->size;
        m_lastPosition = m_petModel->get_info()->position;
    }
}

void AutoMovementModel::setScreenRect(const QRect& screenRect)
{
    m_screenRect = screenRect;
    qDebug() << "Screen rect set to:" << m_screenRect;
}

void AutoMovementModel::initializeScreenInfo()
{
    // 获取主屏幕信息
    QScreen* primaryScreen = QApplication::primaryScreen();
    if (primaryScreen) {
        m_screenRect = primaryScreen->geometry();
    } else {
        // 默认屏幕大小
        m_screenRect = QRect(0, 0, 1920, 1080);
    }
}

void AutoMovementModel::updateMovement()
{
    if (!m_isActive || !m_petModel || m_isPaused || m_isInteracting) {
        return;
    }
    
    // 根据模式处理移动
    switch (m_config.mode) {
    case AutoMovementMode::RandomMovement:
        processRandomMovement();
        // 如果启用了图标交互功能，检测并与图标交互
        if (m_config.enableIconInteraction) {
            detectAndInteractWithIcons();
        }
        break;
    default:
        break;
    }
}

void AutoMovementModel::onPauseTimeout()
{
    m_isPaused = false;
    qDebug() << "Resume movement after pause";
}

void AutoMovementModel::onInteractionComplete()
{
    m_isInteracting = false;
    restoreOriginalAnimation();
    qDebug() << "Interaction animation completed, resuming movement";
}

void AutoMovementModel::refreshDesktopIconsSlot()
{
    if (m_config.enableIconInteraction) {
        refreshDesktopIcons();
        qDebug() << "Desktop icons refreshed periodically";
    }
}

void AutoMovementModel::processRandomMovement()
{
    QPoint currentPos = m_petModel->get_info()->position;
    
    // 每隔一定时间或到达目标时，随机选择新的移动方向
    QPoint diff = m_currentTarget - currentPos;
    int distance = qSqrt(diff.x() * diff.x() + diff.y() * diff.y());
    
    // 如果接近目标或距离太小，立即选择新的随机目标
    if (distance < m_config.speed * 2) {
        // 生成随机的移动方向
        int angle = getRandomInt(0, 359); // 0-359度
        double radian = angle * M_PI / 180.0;
        
        // 计算移动距离（在屏幕范围内的随机距离）
        int moveDistance = getRandomInt(100, 300);
        
        // 计算新目标位置
        int newX = currentPos.x() + static_cast<int>(moveDistance * cos(radian));
        int newY = currentPos.y() + static_cast<int>(moveDistance * sin(radian));
        
        m_currentTarget = QPoint(newX, newY);
        
        // 确保目标在屏幕范围内
        m_currentTarget = constrainToScreen(m_currentTarget);
        
        qDebug() << "New random target:" << m_currentTarget << "angle:" << angle;
        return;
    }
    
    // 计算下一步位置
    QPoint nextPos = currentPos;
    if (distance > 0) {
        double ratio = static_cast<double>(m_config.speed) / distance;
        nextPos.setX(currentPos.x() + static_cast<int>(diff.x() * ratio));
        nextPos.setY(currentPos.y() + static_cast<int>(diff.y() * ratio));
    }
    
    // 约束到屏幕范围
    nextPos = constrainToScreen(nextPos);
    
    // 如果碰到屏幕边缘，立即改变方向
    if (nextPos.x() <= 0 || nextPos.x() >= m_screenRect.width() - m_petSize.width() ||
        nextPos.y() <= 0 || nextPos.y() >= m_screenRect.height() - m_petSize.height()) {
        
        // 反向移动或随机新方向
        int angle = getRandomInt(0, 359);
        double radian = angle * M_PI / 180.0;
        int moveDistance = getRandomInt(100, 200);
        
        int newX = currentPos.x() + static_cast<int>(moveDistance * cos(radian));
        int newY = currentPos.y() + static_cast<int>(moveDistance * sin(radian));
        
        m_currentTarget = constrainToScreen(QPoint(newX, newY));
        qDebug() << "Hit boundary, new direction:" << m_currentTarget;
    }
    
    // 更新宠物位置
    if (nextPos != currentPos) {
        m_petModel->change_position(nextPos);
        m_lastPosition = nextPos;
    }
}

QPoint AutoMovementModel::getRandomScreenPoint()
{
    int margin = 50; // 距离边缘的安全距离
    int x = getRandomInt(margin, m_screenRect.width() - margin - m_petSize.width());
    int y = getRandomInt(margin, m_screenRect.height() - margin - m_petSize.height());
    return QPoint(x, y);
}

bool AutoMovementModel::isPositionValid(const QPoint& pos)
{
    return m_screenRect.contains(pos) && 
           m_screenRect.contains(QPoint(pos.x() + m_petSize.width(), pos.y() + m_petSize.height()));
}

QPoint AutoMovementModel::constrainToScreen(const QPoint& pos)
{
    QPoint constrainedPos = pos;
    
    // 限制到屏幕范围内
    if (constrainedPos.x() < m_screenRect.left()) {
        constrainedPos.setX(m_screenRect.left());
    } else if (constrainedPos.x() + m_petSize.width() > m_screenRect.right()) {
        constrainedPos.setX(m_screenRect.right() - m_petSize.width());
    }
    
    if (constrainedPos.y() < m_screenRect.top()) {
        constrainedPos.setY(m_screenRect.top());
    } else if (constrainedPos.y() + m_petSize.height() > m_screenRect.bottom()) {
        constrainedPos.setY(m_screenRect.bottom() - m_petSize.height());
    }
    
    return constrainedPos;
}

int AutoMovementModel::getRandomInt(int min, int max)
{
    if (min >= max) return min;
    return QRandomGenerator::global()->bounded(min, max + 1);
}

bool AutoMovementModel::shouldPause()
{
    return QRandomGenerator::global()->bounded(100) < m_config.pauseProbability;
}

void AutoMovementModel::detectAndInteractWithIcons()
{
    if (!m_petModel || m_isInteracting) return;
    
    QPoint currentPos = m_petModel->get_info()->position;
    QPoint petCenter = QPoint(currentPos.x() + m_petSize.width() / 2,
                             currentPos.y() + m_petSize.height() / 2);
    
    // 不再每次都刷新桌面图标位置，而是使用缓存的图标信息
    // refreshDesktopIcons(); // 删除这行，改为定时刷新
    
    // 检测是否接近任何图标
    for (int i = 0; i < m_desktopIcons.size(); ++i) {
        const DesktopIconInfo& icon = m_desktopIcons[i];
        if (!icon.isVisible) continue;
        
        // 检查是否已经与此图标交互过
        if (m_interactedIcons.contains(i)) continue;
        
        if (isNearIcon(petCenter, icon)) {
            // 随机决定是否与图标交互
            if (getRandomInt(0, 100) < m_config.iconInteractionProbability) {
                playInteractionAnimation();
                m_interactedIcons.append(i);
                qDebug() << "Interacting with desktop icon:" << icon.text 
                         << "at position:" << icon.rect.center();
                break; // 一次只与一个图标交互
            }
        }
    }
}

void AutoMovementModel::refreshDesktopIcons()
{
    m_desktopIcons = getDesktopIcons();
}

void AutoMovementModel::playInteractionAnimation()
{
    if (!m_petModel || m_isInteracting) return;
    
    m_isInteracting = true;
    
    // 播放交互动画
    m_petModel->change_animation(m_config.interactionAnimation);
    
    // 启动定时器，在指定时间后恢复原始动画
    m_interactionTimer->start(m_config.interactionDuration);
    
    qDebug() << "Playing interaction animation:" << m_config.interactionAnimation 
             << "for" << m_config.interactionDuration << "ms";
}

void AutoMovementModel::restoreOriginalAnimation()
{
    if (!m_petModel || m_originalAnimation.isEmpty()) return;
    
    // 恢复原始动画
    m_petModel->change_animation(m_originalAnimation);
    
    qDebug() << "Restored original animation:" << m_originalAnimation;
}

QVector<DesktopIconInfo> AutoMovementModel::getDesktopIcons()
{
    QVector<DesktopIconInfo> icons;
    
    // 暂时禁用Windows API调用，只使用模拟图标来避免系统卡死
    // 如果无法获取真实图标，创建一些模拟的桌面图标位置用于测试
    QVector<QPoint> iconPositions = {
        QPoint(50, 50),    // 左上角
        QPoint(50, 150),   // 左侧
        QPoint(50, 250),   // 左侧
        QPoint(150, 50),   // 上方
        QPoint(150, 150),  // 中间偏左
        QPoint(250, 50),   // 上方
        QPoint(250, 150),  // 中间
    };
    
    for (int i = 0; i < iconPositions.size(); ++i) {
        DesktopIconInfo iconInfo;
        iconInfo.rect = QRect(iconPositions[i].x(), iconPositions[i].y(), 48, 48); // 标准图标大小
        iconInfo.hwnd = nullptr;
        iconInfo.index = i;
        iconInfo.isVisible = true;
        iconInfo.text = QString("Test_Icon_%1").arg(i + 1);
        
        icons.append(iconInfo);
    }
    qDebug() << "Using" << icons.size() << "simulated desktop icons for testing";
    
    /*
    // Windows API实现（暂时注释掉以避免系统卡死）
    // 找到桌面窗口
    HWND hDesktop = FindWindow(L"Progman", NULL);
    if (!hDesktop) {
        hDesktop = FindWindow(L"WorkerW", NULL);
    }
    
    if (hDesktop) {
        // 找到桌面图标列表视图
        HWND hDefView = FindWindowEx(hDesktop, NULL, L"SHELLDLL_DefView", NULL);
        if (hDefView) {
            HWND hListView = FindWindowEx(hDefView, NULL, L"SysListView32", NULL);
            if (hListView) {
                // 获取图标数量
                int iconCount = ListView_GetItemCount(hListView);
                qDebug() << "Found" << iconCount << "desktop icons";
                
                for (int i = 0; i < iconCount; ++i) {
                    RECT iconRect;
                    if (ListView_GetItemRect(hListView, i, &iconRect, LVIR_BOUNDS)) {
                        DesktopIconInfo iconInfo;
                        iconInfo.rect = QRect(iconRect.left, iconRect.top, 
                                            iconRect.right - iconRect.left, 
                                            iconRect.bottom - iconRect.top);
                        iconInfo.hwnd = hListView;
                        iconInfo.index = i;
                        iconInfo.isVisible = true;
                        
                        // 获取图标文本
                        wchar_t buffer[256];
                        ListView_GetItemText(hListView, i, 0, buffer, sizeof(buffer) / sizeof(wchar_t));
                        iconInfo.text = QString::fromWCharArray(buffer);
                        
                        icons.append(iconInfo);
                    }
                }
            } else {
                qDebug() << "Could not find SysListView32 window";
            }
        } else {
            qDebug() << "Could not find SHELLDLL_DefView window";
        }
    } else {
        qDebug() << "Could not find desktop window";
    }
    */
    
    return icons;
}

bool AutoMovementModel::isNearIcon(const QPoint& pos, const DesktopIconInfo& icon)
{
    QPoint iconCenter = icon.rect.center();
    int distance = qSqrt(qPow(pos.x() - iconCenter.x(), 2) + qPow(pos.y() - iconCenter.y(), 2));
    return distance <= m_config.iconDetectionRadius;
}


