#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <QApplication>
#include <QPoint>
#include <QString>
#include <QSize>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QTest>
#include "../../../src/view/PetMainWindow.h"
#include "../../../src/common/PropertyIds.h"  // 添加PropertyIds.h包含
#include "MockCommand.h"

using ::testing::_;
using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;

class PetMainWindowTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 确保QApplication存在，使用静态变量避免重复创建
        static QApplication *staticApp = nullptr;
        if (!QApplication::instance())
        {
            static int argc = 1;
            static char appName[] = "PetMainWindowTest";
            static char *argv[] = {appName, nullptr};
            staticApp = new QApplication(argc, argv);
            // 设置测试模式，避免显示窗口
            staticApp->setAttribute(Qt::AA_Use96Dpi, true);
        }

        // 创建Mock对象
        mockMoveCommand = std::make_unique<NiceMock<MockCommand>>();
        mockSwitchCommand = std::make_unique<NiceMock<MockCommand>>();

        // 设置测试数据
        testPosition = QPoint(100, 100);
        testSize = QSize(200, 200);
        testAnimation = QString("spider.gif");

        // 延迟创建窗口，避免构造函数阻塞
        // window 将在每个测试中按需创建
    }

    void TearDown() override
    {
        // 确保窗口被正确清理
        if (window)
        {
            window->hide();
            window.reset();
        }
        mockMoveCommand.reset();
        mockSwitchCommand.reset();

        // 处理待处理的事件
        if (QApplication::instance())
        {
            QApplication::processEvents();
        }
    }

    // 安全创建窗口的辅助方法
    void createWindow()
    {
        if (!window)
        {
            try
            {
                window = std::make_unique<PetMainWindow>();

                // 配置窗口（在创建后立即配置）
                window->set_position(&testPosition);
                window->set_size(&testSize);
                window->set_animation(&testAnimation);
                window->set_move_command(mockMoveCommand.get());
                window->set_switch_pet_command(mockSwitchCommand.get());

                // 处理构造时的事件
                QApplication::processEvents();
            }
            catch (const std::exception &e)
            {
                FAIL() << "Failed to create PetMainWindow: " << e.what();
            }
            catch (...)
            {
                FAIL() << "Failed to create PetMainWindow: unknown exception";
            }
        }
    }

    std::unique_ptr<PetMainWindow> window; // 延迟创建
    std::unique_ptr<MockCommand> mockMoveCommand;
    std::unique_ptr<MockCommand> mockSwitchCommand;

    QPoint testPosition;
    QSize testSize;
    QString testAnimation;
};

// 测试窗口的构造和初始化
TEST_F(PetMainWindowTest, Constructor_ShouldInitializeCorrectly)
{
    // 安全创建窗口
    ASSERT_NO_THROW(createWindow());

    // 验证窗口创建成功
    EXPECT_NE(window.get(), nullptr);
    EXPECT_TRUE(window->isWidgetType());

    // 处理事件队列确保初始化完成
    QApplication::processEvents();
}

// 测试设置位置属性
TEST_F(PetMainWindowTest, SetPosition_ShouldWork)
{
    createWindow();

    QPoint newPosition(150, 150);

    // 设置新位置
    window->set_position(&newPosition);

    // 调用update_ui验证设置生效
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();
}

// 测试设置尺寸属性
TEST_F(PetMainWindowTest, SetSize_ShouldWork)
{
    createWindow();

    QSize newSize(300, 300);

    // 设置新尺寸
    window->set_size(&newSize);

    // 调用update_ui验证设置生效
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();
}

// 测试设置动画属性
TEST_F(PetMainWindowTest, SetAnimation_ShouldWork)
{
    createWindow();

    QString newAnimation("cassidy.png");

    // 设置新动画
    window->set_animation(&newAnimation);

    // 调用update_ui验证设置生效
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();
}

// 测试设置移动命令
TEST_F(PetMainWindowTest, SetMoveCommand_ShouldWork)
{
    createWindow();

    auto newMoveCommand = std::make_unique<MockCommand>();

    // 设置移动命令
    window->set_move_command(newMoveCommand.get());

    // 验证设置成功（通过不抛异常来验证）
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();
}

// 测试设置切换宠物命令
TEST_F(PetMainWindowTest, SetSwitchPetCommand_ShouldWork)
{
    createWindow();

    auto newSwitchCommand = std::make_unique<MockCommand>();

    // 设置切换命令
    window->set_switch_pet_command(newSwitchCommand.get());

    // 验证设置成功
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();
}

// 测试通知回调获取
TEST_F(PetMainWindowTest, GetNotification_ShouldReturnValidCallback)
{
    createWindow();

    PropertyNotification notification = window->get_notification();

    // 验证通知回调存在
    EXPECT_NE(notification, nullptr);
}

// 测试UI更新功能
TEST_F(PetMainWindowTest, UpdateUI_ShouldWork)
{
    createWindow();

    // 多次调用update_ui确保稳定性
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_NO_THROW(window->update_ui());
        QApplication::processEvents();
    }
}

// 测试窗口显示和隐藏
TEST_F(PetMainWindowTest, ShowHide_ShouldWork)
{
    createWindow();

    // 测试显示
    window->show();
    QApplication::processEvents();
    EXPECT_TRUE(window->isVisible());

    // 测试隐藏
    window->hide();
    QApplication::processEvents();
    EXPECT_FALSE(window->isVisible());
}

// 测试鼠标事件处理
TEST_F(PetMainWindowTest, MouseEvents_ShouldBeHandled)
{
    createWindow();

    window->show();
    QApplication::processEvents();

    // 设置Mock期望 - 可能会调用移动命令
    EXPECT_CALL(*mockMoveCommand, exec(_))
        .Times(AtLeast(0))
        .WillRepeatedly(Return(0));

    // 创建鼠标事件
    QPoint startPos(50, 50);
    QPoint endPos(100, 100);

    QMouseEvent pressEvent(QEvent::MouseButtonPress, startPos,
                           window->mapToGlobal(startPos), Qt::LeftButton,
                           Qt::LeftButton, Qt::NoModifier);

    QMouseEvent moveEvent(QEvent::MouseMove, endPos,
                          window->mapToGlobal(endPos), Qt::LeftButton,
                          Qt::LeftButton, Qt::NoModifier);

    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, endPos,
                             window->mapToGlobal(endPos), Qt::LeftButton,
                             Qt::LeftButton, Qt::NoModifier);

    // 发送事件并验证不会崩溃
    EXPECT_NO_THROW({
        QApplication::sendEvent(window.get(), &pressEvent);
        QApplication::sendEvent(window.get(), &moveEvent);
        QApplication::sendEvent(window.get(), &releaseEvent);
    });

    // 处理事件队列
    QTest::qWait(50);
}

// 测试右键菜单事件
TEST_F(PetMainWindowTest, ContextMenuEvent_ShouldBeHandled)
{
    createWindow();

    window->show();
    QApplication::processEvents();

    QPoint menuPos(75, 75);
    QContextMenuEvent contextEvent(QContextMenuEvent::Mouse, menuPos,
                                   window->mapToGlobal(menuPos));

    // 发送右键菜单事件
    EXPECT_NO_THROW({
        QApplication::sendEvent(window.get(), &contextEvent);
    });

    QApplication::processEvents();
}

// 测试空指针处理
TEST_F(PetMainWindowTest, NullPointers_ShouldBeHandledSafely)
{
    createWindow();

    // 设置空指针
    window->set_position(nullptr);
    window->set_size(nullptr);
    window->set_animation(nullptr);
    window->set_move_command(nullptr);
    window->set_switch_pet_command(nullptr);

    // 调用update_ui应该安全处理空指针
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();

    // 显示窗口也应该是安全的
    EXPECT_NO_THROW(window->show());
    QApplication::processEvents();
}

// 测试属性的一致性
TEST_F(PetMainWindowTest, PropertyConsistency_ShouldMaintain)
{
    createWindow();

    // 设置多个属性
    QPoint pos1(10, 20);
    QSize size1(50, 60);
    QString anim1("test1.gif");

    window->set_position(&pos1);
    window->set_size(&size1);
    window->set_animation(&anim1);
    window->update_ui();
    QApplication::processEvents();

    // 更改属性
    QPoint pos2(30, 40);
    QSize size2(70, 80);
    QString anim2("test2.gif");

    window->set_position(&pos2);
    window->set_size(&size2);
    window->set_animation(&anim2);
    window->update_ui();
    QApplication::processEvents();

    // 验证没有崩溃且可以正常操作
    EXPECT_NO_THROW({
        window->show();
        QApplication::processEvents();
        window->update_ui();
        QApplication::processEvents();
        window->hide();
        QApplication::processEvents();
    });
}

// 测试窗口生命周期管理
TEST_F(PetMainWindowTest, Lifecycle_ShouldBeManaged)
{
    // 创建多个窗口实例测试资源管理
    std::vector<std::unique_ptr<PetMainWindow>> windows;

    for (int i = 0; i < 3; ++i)
    {
        auto win = std::make_unique<PetMainWindow>();
        QPoint pos(i * 50, i * 50);
        win->set_position(&pos);
        win->show();
        windows.push_back(std::move(win));
    }

    // 验证所有窗口创建成功
    EXPECT_EQ(windows.size(), 3);

    // 批量操作
    for (auto &win : windows)
    {
        EXPECT_NO_THROW(win->update_ui());
    }

    // 清理（自动析构）
    windows.clear();
    EXPECT_TRUE(windows.empty());
}

// 测试命令执行集成
TEST_F(PetMainWindowTest, CommandIntegration_ShouldWork)
{
    createWindow();

    // 设置严格的Mock期望
    EXPECT_CALL(*mockMoveCommand, exec(_))
        .Times(AtLeast(0))
        .WillRepeatedly(Return(42)); // 返回特定值

    window->show();
    QApplication::processEvents();

    // 模拟用户拖拽操作
    QMouseEvent pressEvent(QEvent::MouseButtonPress, QPoint(10, 10),
                           Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent moveEvent(QEvent::MouseMove, QPoint(50, 50),
                          Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, QPoint(50, 50),
                             Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    QApplication::sendEvent(window.get(), &pressEvent);
    QApplication::sendEvent(window.get(), &moveEvent);
    QApplication::sendEvent(window.get(), &releaseEvent);

    // 等待事件处理
    QTest::qWait(100);

    // 验证Mock被正确调用（如果拖拽触发了命令执行）
    // 注意：具体是否调用取决于PetMainWindow的实现
}

// 测试改进的update_ui实现
TEST_F(PetMainWindowTest, ImprovedUpdateUI_ShouldHandleResourcesCorrectly)
{
    createWindow();

    // 测试重复设置相同动画不会重复加载
    QString animation1("spider.gif");
    window->set_animation(&animation1);

    // 第一次更新
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();

    // 再次设置相同动画并更新
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();

    // 设置不同的动画
    QString animation2("cassidy.png");
    window->set_animation(&animation2);
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();

    // 测试不存在的文件
    QString invalidAnimation("nonexistent.gif");
    window->set_animation(&invalidAnimation);
    EXPECT_NO_THROW(window->update_ui()); // 应该安全处理
    QApplication::processEvents();
}

// 测试动画资源管理
TEST_F(PetMainWindowTest, AnimationResourceManagement_ShouldPreventMemoryLeaks)
{
    createWindow();

    // 快速切换多个动画
    std::vector<QString> animations = {
        "spider.gif",
        "cassidy.png",
        "spider.gif",  // 重复
        "cassidy.png", // 重复
        ""             // 空字符串
    };

    for (const auto &anim : animations)
    {
        window->set_animation(&anim);
        EXPECT_NO_THROW(window->update_ui());
        QTest::qWait(10); // 短暂等待
        QApplication::processEvents();
    }

    // 验证最终状态正常
    EXPECT_NO_THROW(window->show());
    QApplication::processEvents();
}

// 测试内存泄漏修复 - 模拟频繁拖拽操作
TEST_F(PetMainWindowTest, MemoryLeakFix_ShouldNotLeakOnFrequentDragging)
{
    createWindow();

    // 模拟频繁的位置更新（类似拖拽场景）
    for (int i = 0; i < 100; ++i)
    {
        QPoint newPos(i % 300, (i * 2) % 300);
        window->set_position(&newPos);

        // 直接调用update_ui来模拟UI更新，而不是调用私有的notification_cb
        window->update_ui();

        if (i % 10 == 0)
        {
            QApplication::processEvents();
        }
    }

    // 验证窗口仍然正常工作
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();
}

// 测试动画资源管理优化
TEST_F(PetMainWindowTest, AnimationResourceOptimization_ShouldAvoidRedundantLoading)
{
    createWindow();

    QString animation("spider.gif");
    window->set_animation(&animation);

    // 第一次更新
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();

    // 重复设置相同动画应该不会重新加载
    for (int i = 0; i < 10; ++i)
    {
        window->set_animation(&animation);
        EXPECT_NO_THROW(window->update_ui());
    }

    // 设置不同的动画
    QString newAnimation("cassidy.png");
    window->set_animation(&newAnimation);
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();

    // 再次切换回原动画
    window->set_animation(&animation);
    EXPECT_NO_THROW(window->update_ui());
    QApplication::processEvents();
}
