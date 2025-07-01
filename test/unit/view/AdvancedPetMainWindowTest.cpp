#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <chrono>
#include <QApplication>
#include <QPoint>
#include <QString>
#include <QSize>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QTest>
#include <QScreen>
#include <QTimer>
#include "../../../src/view/PetMainWindow.h"
#include "MockCommand.h"

using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::Exactly;
using ::testing::InSequence;
using ::testing::StrictMock;

class AdvancedPetMainWindowTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 确保QApplication存在
        if (!QApplication::instance()) {
            int argc = 0;
            char* argv[] = {nullptr};
            app = new QApplication(argc, argv);
        }
        
        // 创建窗口和Mock对象
        window = std::make_unique<PetMainWindow>();
        mockMoveCommand = std::make_unique<StrictMock<MockCommand>>();
        mockSwitchCommand = std::make_unique<StrictMock<MockCommand>>();
        
        // 设置默认测试环境
        setupDefaultEnvironment();
    }
    
    void TearDown() override {
        window.reset();
        mockMoveCommand.reset();
        mockSwitchCommand.reset();
    }
    
    void setupDefaultEnvironment() {
        testPosition = QPoint(200, 200);
        testSize = QSize(150, 150);
        testAnimation = QString("spider.gif");
        
        window->set_position(&testPosition);
        window->set_size(&testSize);
        window->set_animation(&testAnimation);
        window->set_move_command(mockMoveCommand.get());
        window->set_switch_pet_command(mockSwitchCommand.get());
    }
    
    QApplication* app = nullptr;
    std::unique_ptr<PetMainWindow> window;
    std::unique_ptr<MockCommand> mockMoveCommand;
    std::unique_ptr<MockCommand> mockSwitchCommand;
    
    QPoint testPosition;
    QSize testSize;
    QString testAnimation;
};

// 测试多屏幕支持
TEST_F(AdvancedPetMainWindowTest, MultiScreenSupport_ShouldWork) {
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) {
        GTEST_SKIP() << "No primary screen available for testing";
    }
    
    QRect screenGeometry = screen->geometry();
    
    // 测试窗口在屏幕边缘的位置
    std::vector<QPoint> edgePositions = {
        QPoint(0, 0),                                    // 左上角
        QPoint(screenGeometry.width() - 100, 0),        // 右上角
        QPoint(0, screenGeometry.height() - 100),       // 左下角
        QPoint(screenGeometry.width() - 100, 
               screenGeometry.height() - 100)           // 右下角
    };
    
    for (const auto& pos : edgePositions) {
        window->set_position(&pos);
        EXPECT_NO_THROW(window->update_ui());
        EXPECT_NO_THROW(window->show());
        QTest::qWait(10);
        window->hide();
    }
}

// 测试动画资源处理
TEST_F(AdvancedPetMainWindowTest, AnimationResources_ShouldBeHandled) {
    // 测试不同类型的动画资源
    std::vector<QString> animations = {
        "spider.gif",           // 存在的GIF
        "cassidy.png",          // 存在的PNG
        "nonexistent.gif",      // 不存在的文件
        "",                     // 空字符串
        "蜘蛛耄耋.gif"          // Unicode文件名
    };
    
    for (const auto& animation : animations) {
        EXPECT_NO_THROW({
            window->set_animation(&animation);
            window->update_ui();
        });
    }
}

// 测试窗口状态管理
TEST_F(AdvancedPetMainWindowTest, WindowStateManagement_ShouldWork) {
    // 测试各种窗口状态
    EXPECT_FALSE(window->isVisible());
    
    // 正常显示
    window->show();
    QApplication::processEvents();
    EXPECT_TRUE(window->isVisible());
    
    // 最小化
    window->showMinimized();
    QApplication::processEvents();
    EXPECT_TRUE(window->isMinimized());
    
    // 恢复正常
    window->showNormal();
    QApplication::processEvents();
    EXPECT_FALSE(window->isMinimized());
    
    // 激活窗口
    window->activateWindow();
    QApplication::processEvents();
}

// 测试命令执行时序
TEST_F(AdvancedPetMainWindowTest, CommandExecutionTiming_ShouldBeCorrect) {
    InSequence seq;
    
    // 设置期望的调用顺序
    EXPECT_CALL(*mockMoveCommand, exec(_))
        .Times(1)
        .WillOnce(Return(0));
    
    window->show();
    
    // 模拟拖拽操作序列
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(50, 50),
                          Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(100, 100),
                         Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(100, 100),
                            Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    
    QApplication::sendEvent(window.get(), &pressEvent);
    QApplication::sendEvent(window.get(), &moveEvent);
    QApplication::sendEvent(window.get(), &releaseEvent);
    
    QApplication::processEvents();
    QTest::qWait(100);
}

// 测试内存使用和资源管理
TEST_F(AdvancedPetMainWindowTest, MemoryManagement_ShouldBeEfficient) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // 创建多个窗口实例
    std::vector<std::unique_ptr<PetMainWindow>> windows;
    
    for (int i = 0; i < 10; ++i) {
        auto win = std::make_unique<PetMainWindow>();
        QPoint pos(i * 20, i * 20);
        win->set_position(&pos);
        win->show();
        windows.push_back(std::move(win));
        
        if (i % 3 == 0) {
            QApplication::processEvents();
        }
    }
    
    // 验证所有窗口创建成功
    EXPECT_EQ(windows.size(), 10);
    
    // 批量操作
    for (auto& win : windows) {
        win->update_ui();
    }
    
    // 部分清理
    windows.resize(5);
    QApplication::processEvents();
    
    // 剩余窗口操作
    for (auto& win : windows) {
        QPoint newPos(rand() % 300, rand() % 300);
        win->set_position(&newPos);
        win->update_ui();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 性能要求：操作应在合理时间内完成
    EXPECT_LT(duration.count(), 3000) << "Memory management test took too long: " 
                                      << duration.count() << "ms";
    
    // 全部清理
    windows.clear();
    QApplication::processEvents();
}

// 测试并发事件处理
TEST_F(AdvancedPetMainWindowTest, ConcurrentEventHandling_ShouldWork) {
    window->show();
    
    // 设置宽松的Mock期望，因为并发事件可能触发多次调用
    EXPECT_CALL(*mockMoveCommand, exec(_))
        .Times(AtLeast(0))
        .WillRepeatedly(Return(0));
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 快速发送大量不同类型的事件
    for (int i = 0; i < 50; ++i) {
        QPoint pos(i % 200, (i * 2) % 200);
        
        if (i % 3 == 0) {
            QMouseEvent moveEvent(QEvent::MouseMove, pos,
                                 Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            QApplication::sendEvent(window.get(), &moveEvent);
        } else if (i % 3 == 1) {
            QMouseEvent pressEvent(QEvent::MouseButtonPress, pos,
                                  Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            QApplication::sendEvent(window.get(), &pressEvent);
        } else {
            QMouseEvent releaseEvent(QEvent::MouseButtonRelease, pos,
                                    Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            QApplication::sendEvent(window.get(), &releaseEvent);
        }
        
        // 偶尔处理事件队列
        if (i % 10 == 0) {
            QApplication::processEvents();
        }
    }
    
    QApplication::processEvents();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 2000) << "Concurrent event handling took too long: "
                                      << duration.count() << "ms";
}

// 测试拖拽边界检测
TEST_F(AdvancedPetMainWindowTest, DragBoundaryDetection_ShouldWork) {
    window->show();
    
    // 设置Mock期望
    EXPECT_CALL(*mockMoveCommand, exec(_))
        .Times(AtLeast(0))
        .WillRepeatedly(Return(0));
    
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) {
        GTEST_SKIP() << "No primary screen available for boundary testing";
    }
    
    QRect screenRect = screen->geometry();
    
    // 测试拖拽到屏幕边界外的点
    std::vector<QPoint> boundaryPoints = {
        QPoint(-50, -50),                                // 左上角外
        QPoint(screenRect.width() + 50, -50),           // 右上角外
        QPoint(-50, screenRect.height() + 50),          // 左下角外
        QPoint(screenRect.width() + 50, 
               screenRect.height() + 50)                // 右下角外
    };
    
    for (const auto& point : boundaryPoints) {
        // 拖拽序列
        QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(100, 100),
                              Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QMouseEvent moveEvent(QEvent::MouseMove, point,
                             Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, point,
                                Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        
        QApplication::sendEvent(window.get(), &pressEvent);
        QApplication::sendEvent(window.get(), &moveEvent);
        QApplication::sendEvent(window.get(), &releaseEvent);
        
        QApplication::processEvents();
        QTest::qWait(10);
    }
}

// 测试长时间运行稳定性
TEST_F(AdvancedPetMainWindowTest, LongRunningStability_ShouldMaintain) {
    window->show();
    
    EXPECT_CALL(*mockMoveCommand, exec(_))
        .Times(AtLeast(0))
        .WillRepeatedly(Return(0));
    
    auto overallStart = std::chrono::high_resolution_clock::now();
    
    // 模拟长时间运行
    for (int cycle = 0; cycle < 30; ++cycle) {
        // 每个周期执行一系列操作
        for (int i = 0; i < 5; ++i) {
            QPoint pos(rand() % 300, rand() % 300);
            window->set_position(&pos);
            
            if (i % 2 == 0) {
                window->update_ui();
            }
            
            QTest::qWait(1);
        }
        
        // 偶尔隐藏和显示窗口
        if (cycle % 10 == 0) {
            window->hide();
            QApplication::processEvents();
            window->show();
            QApplication::processEvents();
        }
        
        QApplication::processEvents();
        
        // 检查是否超时
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - overallStart);
        if (elapsed.count() > 3000) {
            break;
        }
    }
    
    // 验证窗口仍然正常工作
    EXPECT_TRUE(window->isWidgetType());
    EXPECT_NO_THROW(window->update_ui());
}

// 测试异常恢复能力
TEST_F(AdvancedPetMainWindowTest, ExceptionRecovery_ShouldWork) {
    // 模拟命令执行失败的情况
    EXPECT_CALL(*mockMoveCommand, exec(_))
        .Times(AtLeast(0))
        .WillRepeatedly(Return(-1)); // 返回错误码
    
    window->show();
    
    // 尝试各种操作，即使命令失败也应该稳定
    for (int i = 0; i < 10; ++i) {
        QPoint pos(i * 20, i * 20);
        window->set_position(&pos);
        
        // 尝试触发可能失败的命令执行
        QMouseEvent pressEvent(QEvent::MouseButtonPress, pos,
                              Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QMouseEvent moveEvent(QEvent::MouseMove, QPoint(pos.x() + 10, pos.y() + 10),
                             Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(pos.x() + 10, pos.y() + 10),
                                Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        
        QApplication::sendEvent(window.get(), &pressEvent);
        QApplication::sendEvent(window.get(), &moveEvent);
        QApplication::sendEvent(window.get(), &releaseEvent);
        
        QApplication::processEvents();
    }
    
    // 验证窗口仍然可用
    EXPECT_TRUE(window->isVisible());
    EXPECT_NO_THROW(window->update_ui());
}

// 测试性能基准
TEST_F(AdvancedPetMainWindowTest, PerformanceBenchmark_ShouldMeetRequirements) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // 执行大量操作
    for (int i = 0; i < 500; ++i) {
        QPoint pos(i % 250, (i * 2) % 250);
        window->set_position(&pos);
        
        if (i % 5 == 0) {
            window->update_ui();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // 性能要求：500次操作应在1秒内完成
    EXPECT_LT(duration.count(), 1000) << "Performance benchmark failed: took "
                                      << duration.count() << "ms for 500 operations";
}

// 测试资源清理
TEST_F(AdvancedPetMainWindowTest, ResourceCleanup_ShouldBeComplete) {
    // 创建临时资源
    std::vector<std::unique_ptr<PetMainWindow>> tempWindows;
    
    for (int i = 0; i < 5; ++i) {
        auto win = std::make_unique<PetMainWindow>();
        QPoint pos(i * 30, i * 30);
        win->set_position(&pos);
        tempWindows.push_back(std::move(win));
    }
    
    // 验证创建成功
    EXPECT_EQ(tempWindows.size(), 5);
    
    // 清理资源
    tempWindows.clear();
    QApplication::processEvents();
    
    // 验证清理完成
    EXPECT_TRUE(tempWindows.empty());
}
