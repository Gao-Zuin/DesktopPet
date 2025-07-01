#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QTest>
#include <QPoint>
#include <QSize>
#include <memory>
#include <random>

/**
 * View测试辅助工具类
 * 仿照ViewModel测试的风格，提供View测试所需的辅助功能
 */
class ViewTestHelpers {
public:
    /**
     * 确保QApplication实例存在
     * 仿照ViewModel测试中的应用程序管理方式
     */
    static QApplication* ensureQApplication() {
        if (!QApplication::instance()) {
            static int argc = 0;
            static char* argv[] = {nullptr};
            static QApplication app(argc, argv);
            return &app;
        }
        return qobject_cast<QApplication*>(QApplication::instance());
    }
    
    /**
     * 安全处理Qt事件循环
     * 类似ViewModel测试中的事件处理方式
     */
    static void processEvents(int maxTimeMs = 100) {
        QTimer timer;
        timer.setSingleShot(true);
        timer.start(maxTimeMs);
        
        while (timer.isActive()) {
            QApplication::processEvents();
            QTest::qWait(1);
        }
    }
    
    /**
     * 生成测试用的随机位置
     * 仿照ViewModel测试中的数据生成方式
     */
    static QPoint generateTestPosition(int maxX = 500, int maxY = 500) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> distX(0, maxX);
        std::uniform_int_distribution<> distY(0, maxY);
        return QPoint(distX(gen), distY(gen));
    }
    
    /**
     * 生成测试用的随机尺寸
     */
    static QSize generateTestSize(int minSize = 50, int maxSize = 300) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(minSize, maxSize);
        return QSize(dist(gen), dist(gen));
    }
    
    /**
     * 生成测试用的动画文件名
     */
    static QString generateTestAnimation(int index = -1) {
        static std::vector<QString> animations = {
            "spider.gif",
            "cassidy.png",
            "蜘蛛耄耋.gif",
            "test_animation.gif"
        };
        
        if (index >= 0 && index < static_cast<int>(animations.size())) {
            return animations[index];
        }
        
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, animations.size() - 1);
        return animations[dist(gen)];
    }
    
    /**
     * 验证窗口基本状态
     * 类似ViewModel测试中的验证方式
     */
    static bool isWindowValid(QWidget* window) {
        return window != nullptr && 
               window->isWidgetType() &&
               !window->isDestroyed();
    }
    
    /**
     * 模拟用户操作延迟
     */
    static void simulateUserDelay(int minMs = 10, int maxMs = 50) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(minMs, maxMs);
        QTest::qWait(dist(gen));
    }
};

/**
 * 性能测试计时器
 * 仿照ViewModel测试的性能测试方式
 */
class ViewTestTimer {
public:
    ViewTestTimer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    void reset() {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    long long elapsedMs() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
    }
    
    bool hasExceeded(long long maxMs) const {
        return elapsedMs() > maxMs;
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_;
};

/**
 * RAII风格的Qt应用程序管理器
 * 仿照ViewModel测试的资源管理方式
 */
class ViewTestApp {
public:
    ViewTestApp() {
        app_ = ViewTestHelpers::ensureQApplication();
    }
    
    ~ViewTestApp() = default;
    
    QApplication* get() const { return app_; }
    
    void processEvents(int maxTimeMs = 100) {
        ViewTestHelpers::processEvents(maxTimeMs);
    }
    
private:
    QApplication* app_;
};

#endif // TEST_HELPERS_H
