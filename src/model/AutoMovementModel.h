#ifndef __AUTO_MOVEMENT_MODEL_H__
#define __AUTO_MOVEMENT_MODEL_H__

#include "../common/PropertyTrigger.h"
#include "../common/Types.h"
#include <QTimer>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QScreen>
#include <memory>
#include <QVector>
#include <Windows.h>
#include <CommCtrl.h>

class PetModel;

/**
 * 桌面图标信息结构
 */
struct DesktopIconInfo {
    QRect rect;          // 图标的位置和大小
    QString text;        // 图标的文本
    HWND hwnd;           // 图标所在的窗口句柄
    int index;           // 图标在列表中的索引
    bool isVisible;      // 图标是否可见
    
    DesktopIconInfo() : hwnd(nullptr), index(-1), isVisible(true) {}
};


/**
 * 自动移动配置结构
 */
struct AutoMovementConfig {
    AutoMovementMode mode = AutoMovementMode::Disabled;
    int speed = 10;                    // 移动速度（像素/帧）
    int updateInterval = 50;          // 更新间隔（毫秒）
    bool enableRandomPause = false;   // 是否启用随机暂停（已禁用）
    int pauseProbability = 0;         // 暂停概率（百分比）
    int pauseDuration = 0;            // 暂停持续时间（毫秒）
    bool enableIconInteraction = true; // 是否启用图标交互功能
    int iconInteractionProbability = 50; // 图标交互的概率（百分比）
    int iconDetectionRadius = 50;     // 图标检测半径（像素）
    QString interactionAnimation = ":/resources/gif/kicking.gif"; // 交互时播放的动画
    int interactionDuration = 1000;   // 交互动画持续时间（毫秒）
};

/**
 * 自动移动模型类
 * 负责管理桌宠的自动移动行为
 */
class AutoMovementModel : public QObject
{
    Q_OBJECT

public:
    explicit AutoMovementModel(QObject *parent = nullptr);
    ~AutoMovementModel();

    // 禁用拷贝构造和赋值
    AutoMovementModel(const AutoMovementModel&) = delete;
    AutoMovementModel& operator=(const AutoMovementModel&) = delete;

    // 配置管理
    void setConfig(const AutoMovementConfig& config);
    const AutoMovementConfig& getConfig() const { return m_config; }

    // 启动/停止自动移动
    void startAutoMovement();
    void stopAutoMovement();
    bool isAutoMovementActive() const { return m_isActive; }

    // 设置关联的宠物模型
    void setPetModel(std::shared_ptr<PetModel> petModel);

    // 设置屏幕区域（可选，默认使用主屏幕）
    void setScreenRect(const QRect& screenRect);

    // 获取当前状态
    AutoMovementMode getCurrentMode() const { return m_config.mode; }
    QPoint getCurrentTarget() const { return m_currentTarget; }

    // 通知系统
    PropertyTrigger& getTrigger() { return m_trigger; }
    
    // 桌面图标管理
    void refreshDesktopIcons();
    int getInteractedIconCount() const { return m_interactedIcons.size(); }

private slots:
    void updateMovement();
    void onPauseTimeout();
    void onInteractionComplete();
    void refreshDesktopIconsSlot();

private:
    // 初始化
    void initializeScreenInfo();
    
    // 移动处理方法
    void processRandomMovement();
    
    // 桌面图标相关方法
    void detectAndInteractWithIcons();
    void playInteractionAnimation();
    void restoreOriginalAnimation();
    QVector<DesktopIconInfo> getDesktopIcons();
    bool isNearIcon(const QPoint& pos, const DesktopIconInfo& icon);
    
    // 碰撞检测和路径规划
    bool isPositionValid(const QPoint& pos);
    QPoint constrainToScreen(const QPoint& pos);
    QPoint getRandomScreenPoint();
    
    // 随机数生成
    int getRandomInt(int min, int max);
    bool shouldPause();

private:
    // 配置和状态
    AutoMovementConfig m_config;
    bool m_isActive;
    bool m_isPaused;
    
    // 关联模型
    std::shared_ptr<PetModel> m_petModel;
    
    // 定时器
    QTimer* m_updateTimer;
    QTimer* m_pauseTimer;
    QTimer* m_cornerStayTimer;
    QTimer* m_interactionTimer;
    
    // 屏幕信息
    QRect m_screenRect;
    QSize m_petSize;
    
    // 移动状态
    QPoint m_currentTarget;
    QPoint m_lastPosition;
    
    // 桌面图标管理
    QVector<DesktopIconInfo> m_desktopIcons;
    QVector<int> m_interactedIcons;  // 已交互的图标索引列表
    bool m_isInteracting;            // 是否正在进行交互
    QString m_originalAnimation;     // 原始动画路径
    QTimer* m_iconRefreshTimer;      // 图标刷新定时器
    int m_iconRefreshInterval;       // 图标刷新间隔（毫秒）
    
    // 通知系统
    PropertyTrigger m_trigger;
    
    // 常量
    static const int DEFAULT_PET_WIDTH = 200;
    static const int DEFAULT_PET_HEIGHT = 200;
};

#endif // __AUTO_MOVEMENT_MODEL_H__
