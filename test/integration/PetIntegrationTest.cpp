#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <QApplication>
#include <QPoint>
#include <QString>
#include <QSize>
#include <QTest>
#include <QTimer>
#include <QEventLoop>

// 引入所有需要测试的模块
#include "../../src/model/PetModel.h"
#include "../../src/viewmodel/PetViewModel.h"
#include "../../src/view/PetMainWindow.h"
#include "../../src/app/PetApp.h"
#include "../../src/common/PropertyIds.h"
#include "../../src/common/CommandParameters.h"
#include "../../src/common/Types.h"

using ::testing::_;
using ::testing::Return;
using ::testing::AtLeast;

/**
 * 集成测试类 - 测试Model、ViewModel、View之间的完整通信流程
 */
class PetIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 确保QApplication存在
        ensureQApplication();
        
        // 创建所有组件
        model = std::make_shared<PetModel>();
        viewModel = std::make_shared<PetViewModel>();
        view = std::make_unique<PetMainWindow>();
        
        // 建立完整的绑定关系
        setupBindings();
        
        // 初始化测试数据
        setupTestData();
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
            static char appName[] = "PetIntegrationTest";
            static char* argv[] = { appName, nullptr };
            static QApplication app(argc, argv);
        }
    }

    void setupBindings() {
        // 1. ViewModel绑定Model
        viewModel->set_pet_model(model);
        
        // 2. View绑定ViewModel的属性
        view->set_position(viewModel->get_position());
        view->set_animation(viewModel->get_current_animation());
        view->set_size(viewModel->get_size());
        
        // 3. View绑定ViewModel的命令
        view->set_move_command(viewModel->get_move_command());
        view->set_switch_pet_command(viewModel->get_switch_pet_command());
        
        // 4. 建立通知机制
        viewModel->get_trigger().add(view->get_notification(), view.get());
    }

    void setupTestData() {
        // 设置初始数据
        model->change_position(QPoint(100, 100));
        model->change_animation(":/resources/gif/spider.gif");
        model->change_size(QSize(150, 150));
        model->change_pet_type(PetType::Spider);
        
        // 更新UI
        view->update_ui();
    }

    // 等待事件处理完成
    void waitForEvents(int timeoutMs = 100) {
        QEventLoop loop;
        QTimer timer;
        timer.setSingleShot(true);
        timer.start(timeoutMs);
        
        QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        loop.exec();
        
        QApplication::processEvents();
    }

protected:
    std::shared_ptr<PetModel> model;
    std::shared_ptr<PetViewModel> viewModel;
    std::unique_ptr<PetMainWindow> view;
};

// 测试完整的初始化流程
TEST_F(PetIntegrationTest, CompleteInitializationFlow_ShouldWork) {
    // 验证Model初始化
    EXPECT_NE(model, nullptr);
    EXPECT_NE(model->get_info(), nullptr);
    
    // 验证ViewModel初始化和绑定
    EXPECT_NE(viewModel, nullptr);
    EXPECT_EQ(viewModel->get_pet_model(), model);
    EXPECT_NE(viewModel->get_move_command(), nullptr);
    EXPECT_NE(viewModel->get_switch_pet_command(), nullptr);
    
    // 验证View初始化和绑定
    EXPECT_NE(view, nullptr);
    // 注意：notification_cb是私有函数，这里不直接测试
    
    // 验证属性绑定
    EXPECT_EQ(viewModel->get_position(), &(model->get_info()->position));
    EXPECT_EQ(viewModel->get_current_animation(), &(model->get_info()->currentAnimation));
    EXPECT_EQ(viewModel->get_size(), &(model->get_info()->size));
}

// 测试用户拖拽操作的完整流程
TEST_F(PetIntegrationTest, DragOperationFlow_ShouldWork) {
    QPoint initialPos = model->get_info()->position;
    QPoint newPos(200, 200);
    
    // 记录初始位置
    EXPECT_EQ(initialPos, QPoint(100, 100));
    
    // 模拟用户拖拽：通过Command执行移动
    MoveCommandParameter moveParam(newPos);
    int result = viewModel->get_move_command()->exec(&moveParam);
    
    // 验证命令执行成功
    EXPECT_EQ(result, 0);
    
    // 等待通知传播
    waitForEvents();
    
    // 验证Model数据更新
    EXPECT_EQ(model->get_info()->position, newPos);
    
    // 验证ViewModel指针指向正确数据
    EXPECT_EQ(*viewModel->get_position(), newPos);
    
    // 注意：View的实际窗口位置更新需要在真实的UI环境中测试
    // 这里我们主要验证数据流转是否正确
}

// 测试切换宠物的完整流程
TEST_F(PetIntegrationTest, SwitchPetFlow_ShouldWork) {
    // 初始状态应该是Spider
    EXPECT_EQ(model->get_info()->petType, PetType::Spider);
    EXPECT_EQ(model->get_info()->currentAnimation, ":/resources/gif/spider.gif");
    
    // 切换到Cassidy
    SwitchPetCommandParameter switchParam(PetType::Cassidy);
    int result = viewModel->get_switch_pet_command()->exec(&switchParam);
    
    // 验证命令执行成功
    EXPECT_EQ(result, 0);
    
    // 等待通知传播
    waitForEvents();
    
    // 验证Model数据更新
    EXPECT_EQ(model->get_info()->petType, PetType::Cassidy);
    EXPECT_EQ(model->get_info()->currentAnimation, ":/resources/img/cassidy.png");
    
    // 验证ViewModel指针指向正确数据
    EXPECT_EQ(*viewModel->get_current_animation(), ":/resources/img/cassidy.png");
}

// 测试多个属性同时变化的通知机制
TEST_F(PetIntegrationTest, MultiplePropertyChanges_ShouldTriggerCorrectNotifications) {
    // 简化的通知测试 - 主要验证操作不会崩溃
    
    // 同时改变多个属性
    model->change_position(QPoint(300, 300));
    waitForEvents(50);
    
    model->change_size(QSize(200, 200));
    waitForEvents(50);
    
    model->change_pet_type(PetType::Spider);
    waitForEvents(50);
    
    // 验证最终状态
    EXPECT_EQ(model->get_info()->position, QPoint(300, 300));
    EXPECT_EQ(model->get_info()->size, QSize(200, 200));
    EXPECT_EQ(model->get_info()->petType, PetType::Spider);
    
    // 验证ViewModel数据一致性
    EXPECT_EQ(*viewModel->get_position(), QPoint(300, 300));
    EXPECT_EQ(*viewModel->get_size(), QSize(200, 200));
}

// 测试命令执行的错误处理
TEST_F(PetIntegrationTest, CommandErrorHandling_ShouldWork) {
    // 测试传入nullptr参数
    int result1 = viewModel->get_move_command()->exec(nullptr);
    EXPECT_EQ(result1, -1); // 应该返回错误码
    
    // 测试传入错误类型的参数
    SwitchPetCommandParameter wrongParam(PetType::Cassidy);
    int result2 = viewModel->get_move_command()->exec(&wrongParam);
    EXPECT_EQ(result2, -1); // 应该返回错误码
    
    // 模型状态应该没有改变
    EXPECT_EQ(model->get_info()->position, QPoint(100, 100)); // 初始位置
}

// 测试View的命令集成
TEST_F(PetIntegrationTest, ViewCommandIntegration_ShouldWork) {
    // 验证View可以正确设置命令（使用传统接口）
    EXPECT_NO_THROW({
        view->set_move_command(viewModel->get_move_command());
        view->set_switch_pet_command(viewModel->get_switch_pet_command());
    });
    
    // 测试通过ViewModel执行命令
    QPoint newPos(400, 400);
    
    // 直接通过ViewModel的命令执行
    MoveCommandParameter moveParam(newPos);
    int result = viewModel->get_move_command()->exec(&moveParam);
    
    EXPECT_EQ(result, 0);
    waitForEvents();
    
    EXPECT_EQ(model->get_info()->position, newPos);
}

// 测试完整的应用程序生命周期
TEST_F(PetIntegrationTest, ApplicationLifecycle_ShouldWork) {
    // 创建完整的应用程序实例
    PetApp app;
    
    // 测试初始化
    EXPECT_TRUE(app.initialize());
    
    // 注意：这里不实际显示窗口，因为在测试环境中可能没有显示器
    // app.show_main_window();
    
    // 验证应用程序组件都已正确创建和初始化
    // 这个测试主要验证PetApp::initialize()不会崩溃
    waitForEvents();
}

// 测试属性绑定的一致性
TEST_F(PetIntegrationTest, PropertyBindingConsistency_ShouldWork) {
    // 通过Model改变数据
    QPoint newPos(500, 500);
    QSize newSize(250, 250);
    QString newAnimation = ":/resources/img/cassidy.png";
    
    model->change_position(newPos);
    model->change_size(newSize);
    model->change_animation(newAnimation);
    
    waitForEvents();
    
    // 验证ViewModel的指针仍然指向正确的数据
    EXPECT_EQ(*viewModel->get_position(), newPos);
    EXPECT_EQ(*viewModel->get_size(), newSize);
    EXPECT_EQ(*viewModel->get_current_animation(), newAnimation);
    
    // 验证数据一致性
    EXPECT_EQ(viewModel->get_position(), &(model->get_info()->position));
    EXPECT_EQ(viewModel->get_size(), &(model->get_info()->size));
    EXPECT_EQ(viewModel->get_current_animation(), &(model->get_info()->currentAnimation));
}
