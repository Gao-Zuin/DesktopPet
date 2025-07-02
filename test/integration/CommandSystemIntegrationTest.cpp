#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <QApplication>
#include <QTest>

#include "../../src/model/PetModel.h"
#include "../../src/viewmodel/PetViewModel.h"
#include "../../src/view/PetMainWindow.h"
#include "../../src/common/CommandParameters.h"
#include "../../src/viewmodel/commands/MovePetCommand.h"
#include "../../src/viewmodel/commands/SwitchPetCommand.h"

/**
 * 专门测试命令系统集成的测试类
 */
class CommandSystemIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        ensureQApplication();
        
        model = std::make_shared<PetModel>();
        viewModel = std::make_shared<PetViewModel>();
        view = std::make_unique<PetMainWindow>();
        
        setupCommandSystem();
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
            static char appName[] = "CommandSystemTest";
            static char* argv[] = { appName, nullptr };
            static QApplication app(argc, argv);
        }
    }

    void setupCommandSystem() {
        // 建立Model-ViewModel连接
        viewModel->set_pet_model(model);
        
        // 设置View属性绑定
        view->set_position(viewModel->get_position());
        view->set_animation(viewModel->get_current_animation());
        view->set_size(viewModel->get_size());
        
        // 设置View命令绑定
        view->set_move_command(viewModel->get_move_command());
        view->set_switch_pet_command(viewModel->get_switch_pet_command());
    }

protected:
    std::shared_ptr<PetModel> model;
    std::shared_ptr<PetViewModel> viewModel;
    std::unique_ptr<PetMainWindow> view;
};

// 测试命令的基本功能
TEST_F(CommandSystemIntegrationTest, Commands_BasicFunctionality_ShouldWork) {
    // 测试命令对象存在
    EXPECT_NE(viewModel->get_move_command(), nullptr);
    EXPECT_NE(viewModel->get_switch_pet_command(), nullptr);
    
    // 测试移动命令执行
    QPoint initialPos = model->get_info()->position;
    QPoint targetPos(300, 400);
    
    MoveCommandParameter moveParam(targetPos);
    int result = viewModel->get_move_command()->exec(&moveParam);
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(model->get_info()->position, targetPos);
    EXPECT_NE(model->get_info()->position, initialPos);
}

// 测试切换宠物命令的完整流程
TEST_F(CommandSystemIntegrationTest, SwitchPetCommand_CompleteFlow_ShouldWork) {
    // 初始状态
    PetType initialType = model->get_info()->petType;
    QString initialAnimation = model->get_info()->currentAnimation;
    
    // 执行切换命令
    SwitchPetCommandParameter switchParam(PetType::Cassidy);
    int result = viewModel->get_switch_pet_command()->exec(&switchParam);
    
    EXPECT_EQ(result, 0);
    
    // 验证宠物类型变化
    EXPECT_EQ(model->get_info()->petType, PetType::Cassidy);
    EXPECT_NE(model->get_info()->petType, initialType);
    
    // 验证动画也相应变化
    EXPECT_EQ(model->get_info()->currentAnimation, ":/resources/img/cassidy.png");
    EXPECT_NE(model->get_info()->currentAnimation, initialAnimation);
    
    // 验证ViewModel的数据一致性
    EXPECT_EQ(*viewModel->get_current_animation(), ":/resources/img/cassidy.png");
    
    // 切换回Spider
    SwitchPetCommandParameter switchBackParam(PetType::Spider);
    result = viewModel->get_switch_pet_command()->exec(&switchBackParam);
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(model->get_info()->petType, PetType::Spider);
    EXPECT_EQ(model->get_info()->currentAnimation, ":/resources/gif/spider.gif");
}

// 测试命令的错误处理
TEST_F(CommandSystemIntegrationTest, CommandErrorHandling_ShouldBeRobust) {
    // 测试空参数
    int result2 = viewModel->get_move_command()->exec(nullptr);
    EXPECT_EQ(result2, -1);
    
    // 测试错误类型的参数
    SwitchPetCommandParameter wrongParam(PetType::Cassidy);
    int result3 = viewModel->get_move_command()->exec(&wrongParam);
    EXPECT_EQ(result3, -1);
    
    // 验证错误操作不会影响模型状态
    QPoint currentPos = model->get_info()->position;
    
    // 执行正确的命令验证系统仍然正常
    MoveCommandParameter validParam(QPoint(500, 600));
    int result4 = viewModel->get_move_command()->exec(&validParam);
    EXPECT_EQ(result4, 0);
    EXPECT_EQ(model->get_info()->position, QPoint(500, 600));
}

// 测试View的命令集成
TEST_F(CommandSystemIntegrationTest, ViewCommandIntegration_ShouldWork) {
    // 验证View可以使用命令接口
    EXPECT_NO_THROW({
        view->set_move_command(viewModel->get_move_command());
        view->set_switch_pet_command(viewModel->get_switch_pet_command());
    });
    
    // 测试通过ViewModel接口执行命令的效果
    QPoint testPos(777, 888);
    MoveCommandParameter param(testPos);
    int result = viewModel->get_move_command()->exec(&param);
    
    EXPECT_EQ(result, 0);
    EXPECT_EQ(model->get_info()->position, testPos);
}

// 测试命令的并发执行
TEST_F(CommandSystemIntegrationTest, ConcurrentCommandExecution_ShouldBeStable) {
    // 快速执行多个命令
    std::vector<QPoint> positions;
    std::vector<PetType> petTypes = {PetType::Spider, PetType::Cassidy, PetType::Spider};
    
    for (int i = 0; i < 20; ++i) {
        QPoint pos(i * 10, i * 15);
        positions.push_back(pos);
        
        MoveCommandParameter moveParam(pos);
        int moveResult = viewModel->get_move_command()->exec(&moveParam);
        EXPECT_EQ(moveResult, 0);
        
        if (i % 5 == 0) {
            PetType type = petTypes[i % petTypes.size()];
            SwitchPetCommandParameter switchParam(type);
            int switchResult = viewModel->get_switch_pet_command()->exec(&switchParam);
            EXPECT_EQ(switchResult, 0);
        }
    }
    
    // 验证最终状态
    EXPECT_EQ(model->get_info()->position, positions.back());
    
    // 验证数据一致性
    EXPECT_EQ(*viewModel->get_position(), model->get_info()->position);
}

// 测试命令参数的完整性
TEST_F(CommandSystemIntegrationTest, CommandParameterIntegrity_ShouldBeCorrect) {
    // 测试移动命令参数
    QPoint testPos(-100, -200); // 测试负坐标
    MoveCommandParameter moveParam(testPos);
    
    int result1 = viewModel->get_move_command()->exec(&moveParam);
    EXPECT_EQ(result1, 0);
    EXPECT_EQ(model->get_info()->position, testPos);
    
    // 测试切换宠物命令参数
    for (auto petType : {PetType::Spider, PetType::Cassidy}) {
        SwitchPetCommandParameter switchParam(petType);
        int result = viewModel->get_switch_pet_command()->exec(&switchParam);
        EXPECT_EQ(result, 0);
        EXPECT_EQ(model->get_info()->petType, petType);
        
        // 验证相应的动画设置
        if (petType == PetType::Spider) {
            EXPECT_EQ(model->get_info()->currentAnimation, ":/resources/gif/spider.gif");
        } else if (petType == PetType::Cassidy) {
            EXPECT_EQ(model->get_info()->currentAnimation, ":/resources/img/cassidy.png");
        }
    }
}
