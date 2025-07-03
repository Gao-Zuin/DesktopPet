#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <QApplication>
#include <QTest>
#include <QEventLoop>
#include <QTimer>
#include <vector>
#include <chrono>
#include <set>

#include "../../src/model/PetModel.h"
#include "../../src/viewmodel/PetViewModel.h"
#include "../../src/view/PetMainWindow.h"
#include "../../src/common/PropertyIds.h"
#include "../../src/common/CommandParameters.h"

/**
 * 专门测试模块间通信机制的集成测试
 */
class CommunicationIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        ensureQApplication();
        
        model = std::make_shared<PetModel>();
        viewModel = std::make_shared<PetViewModel>();
        // view will be created in setupCommunicationChain with CommandManager
        
        // 重置通知记录
        notificationHistory.clear();
        
        setupCommunicationChain();
    }

    void TearDown() override {
        view.reset();
        viewModel.reset();
        model.reset();
        
        if (QApplication::instance()) {
            QApplication::processEvents();
        }
    }

    void ensureQApplication() {
        if (!QApplication::instance()) {
            static int argc = 1;
            static char appName[] = "CommunicationTest";
            static char* argv[] = { appName, nullptr };
            static QApplication app(argc, argv);
        }
    }

    void setupCommunicationChain() {
        // 建立完整的通信链
        viewModel->set_pet_model(model);
        
        // 创建View使用CommandManager
        view = std::make_unique<PetMainWindow>(viewModel->get_command_manager());
        
        view->set_position(viewModel->get_position());
        view->set_animation(viewModel->get_current_animation());
        view->set_size(viewModel->get_size());
        
        // 添加通知记录回调
        viewModel->get_trigger().add(&notificationRecorder, this);
    }

    // 静态回调函数用于记录通知
    static void notificationRecorder(uint32_t id, void* p) {
        CommunicationIntegrationTest* test = static_cast<CommunicationIntegrationTest*>(p);
        test->notificationHistory.push_back({id, std::chrono::steady_clock::now()});
    }

    void waitForNotifications(int timeoutMs = 100) {
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);
        timer.start(timeoutMs);
        
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        loop.exec();
        
        QApplication::processEvents();
    }

    // 通知记录结构
    struct NotificationRecord {
        uint32_t id;
        std::chrono::steady_clock::time_point timestamp;
    };

protected:
    std::shared_ptr<PetModel> model;
    std::shared_ptr<PetViewModel> viewModel;
    std::unique_ptr<PetMainWindow> view;
    std::vector<NotificationRecord> notificationHistory;
};

// 测试Model -> ViewModel -> View的通知链
TEST_F(CommunicationIntegrationTest, ModelToViewNotificationChain_ShouldWork) {
    // 清空历史记录
    notificationHistory.clear();
    
    // 通过Model触发变化
    model->change_position(QPoint(123, 456));
    waitForNotifications();
    
    // 验证收到了位置变化通知
    EXPECT_GE(notificationHistory.size(), 1);
    
    bool foundPositionNotification = false;
    for (const auto& record : notificationHistory) {
        if (record.id == PROP_ID_PET_POSITION) {
            foundPositionNotification = true;
            break;
        }
    }
    
    EXPECT_TRUE(foundPositionNotification);
    
    // 验证数据传播正确
    EXPECT_EQ(model->get_info()->position, QPoint(123, 456));
    EXPECT_EQ(*viewModel->get_position(), QPoint(123, 456));
}

// 测试View -> ViewModel -> Model的命令链
TEST_F(CommunicationIntegrationTest, ViewToModelCommandChain_ShouldWork) {
    QPoint initialPos = model->get_info()->position;
    QPoint targetPos(789, 101);
    
    // 清空通知历史
    notificationHistory.clear();
    
    // 通过View的命令执行移动（模拟用户拖拽）
    MoveCommandParameter param(targetPos);
    int result = viewModel->get_command_manager().execute_command(CommandType::MOVE_PET, &param);
    
    EXPECT_EQ(result, 0);
    waitForNotifications();
    
    // 验证命令执行导致的数据变化
    EXPECT_EQ(model->get_info()->position, targetPos);
    EXPECT_NE(model->get_info()->position, initialPos);
    
    // 验证通知被正确触发
    EXPECT_GE(notificationHistory.size(), 1);
    
    bool foundPositionNotification = false;
    for (const auto& record : notificationHistory) {
        if (record.id == PROP_ID_PET_POSITION) {
            foundPositionNotification = true;
            break;
        }
    }
    
    EXPECT_TRUE(foundPositionNotification);
}

// 测试双向通信的完整循环
TEST_F(CommunicationIntegrationTest, BidirectionalCommunicationCycle_ShouldWork) {
    notificationHistory.clear();
    
    // 第一阶段：通过命令改变数据
    SwitchPetCommandParameter switchParam(PetType::Cassidy);
    viewModel->get_command_manager().execute_command(CommandType::SWITCH_PET, &switchParam);
    waitForNotifications();
    
    // 记录第一次通知的数量
    size_t firstPhaseNotifications = notificationHistory.size();
    EXPECT_GE(firstPhaseNotifications, 1);
    
    // 验证数据变化
    EXPECT_EQ(model->get_info()->petType, PetType::Cassidy);
    
    // 第二阶段：直接通过Model改变数据
    model->change_size(QSize(300, 300));
    waitForNotifications();
    
    // 验证总通知数量增加
    EXPECT_GT(notificationHistory.size(), firstPhaseNotifications);
    
    // 验证最终状态
    EXPECT_EQ(model->get_info()->petType, PetType::Cassidy);
    EXPECT_EQ(model->get_info()->size, QSize(300, 300));
    EXPECT_EQ(*viewModel->get_size(), QSize(300, 300));
}

// 测试并发通信的稳定性
TEST_F(CommunicationIntegrationTest, ConcurrentCommunication_ShouldBeStable) {
    notificationHistory.clear();
    
    // 快速连续执行多个操作
    for (int i = 0; i < 10; ++i) {
        model->change_position(QPoint(i * 10, i * 20));
        
        MoveCommandParameter param(QPoint(i * 15, i * 25));
        viewModel->get_command_manager().execute_command(CommandType::MOVE_PET, &param);
        
        if (i % 2 == 0) {
            model->change_size(QSize(100 + i * 10, 100 + i * 10));
        }
    }
    
    waitForNotifications(200); // 给更多时间处理
    
    // 验证系统稳定性 - 不应该崩溃，且应该收到通知
    EXPECT_GE(notificationHistory.size(), 10);
    
    // 验证最终状态一致性
    EXPECT_EQ(*viewModel->get_position(), model->get_info()->position);
    EXPECT_EQ(*viewModel->get_size(), model->get_info()->size);
}

// 测试通知时序
TEST_F(CommunicationIntegrationTest, NotificationTiming_ShouldBeCorrect) {
    notificationHistory.clear();
    
    auto startTime = std::chrono::steady_clock::now();
    
    // 连续触发不同类型的变化
    model->change_position(QPoint(100, 200));
    model->change_size(QSize(150, 150));
    model->change_animation("test.gif");
    
    waitForNotifications();
    
    // 验证通知顺序和时间戳
    EXPECT_GE(notificationHistory.size(), 3);
    
    // 验证所有通知都在合理的时间范围内
    for (const auto& record : notificationHistory) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            record.timestamp - startTime);
        EXPECT_LT(duration.count(), 1000); // 应该在1秒内完成
    }
    
    // 验证包含了预期的通知类型
    std::set<uint32_t> receivedIds;
    for (const auto& record : notificationHistory) {
        receivedIds.insert(record.id);
    }
    
    EXPECT_TRUE(receivedIds.count(PROP_ID_PET_POSITION) > 0);
    EXPECT_TRUE(receivedIds.count(PROP_ID_PET_SIZE) > 0);
    EXPECT_TRUE(receivedIds.count(PROP_ID_PET_ANIMATION) > 0);
}

// 测试内存管理和生命周期
TEST_F(CommunicationIntegrationTest, LifecycleManagement_ShouldBeCorrect) {
    // 创建额外的组件来测试生命周期
    auto tempModel = std::make_shared<PetModel>();
    auto tempViewModel = std::make_shared<PetViewModel>();
    
    tempViewModel->set_pet_model(tempModel);
    
    // 执行一些操作
    tempModel->change_position(QPoint(999, 888));
    waitForNotifications();
    
    // 验证临时组件工作正常
    EXPECT_EQ(tempModel->get_info()->position, QPoint(999, 888));
    EXPECT_EQ(*tempViewModel->get_position(), QPoint(999, 888));
    
    // 释放临时组件（智能指针自动管理）
    tempViewModel.reset();
    tempModel.reset();
    
    // 验证原有组件仍然正常工作
    model->change_position(QPoint(111, 222));
    waitForNotifications();
    EXPECT_EQ(model->get_info()->position, QPoint(111, 222));
}
