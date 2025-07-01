#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <QPoint>
#include <QString>
#include <QSize>
#include "../../../src/viewmodel/PetViewModel.h"
#include "../../../src/model/PetModel.h"
#include "../../../src/model/base/PetInfo.h"
#include "../../../src/common/PropertyIds.h"
#include "../../../src/common/CommandParameters.h"
#include "../../../src/common/CommandBase.h"

// Mock PetModel for testing
class MockPetModel : public PetModel {
public:
    MOCK_METHOD(const PetInfo*, get_info, (), (const, noexcept));
    MOCK_METHOD(PropertyTrigger&, get_trigger, (), (noexcept));
    MOCK_METHOD(void, change_position, (const QPoint& position), (noexcept));
    MOCK_METHOD(void, change_pet_type, (PetType type), (noexcept));
    MOCK_METHOD(void, change_size, (const QSize& size), (noexcept));
    MOCK_METHOD(void, change_state, (PetState state), (noexcept));
    MOCK_METHOD(void, change_animation, (const QString& animation), (noexcept));
    MOCK_METHOD(void, change_visibility, (bool visible), (noexcept));
    
    // 为测试提供真实的PetInfo对象
    PetInfo m_test_info;
    PropertyTrigger m_test_trigger;
    
    MockPetModel() {
        ON_CALL(*this, get_info()).WillByDefault(::testing::Return(&m_test_info));
        ON_CALL(*this, get_trigger()).WillByDefault(::testing::ReturnRef(m_test_trigger));
    }
};

class PetViewModelTest : public ::testing::Test {
protected:
    void SetUp() override {
        mockPetModel = std::make_shared<MockPetModel>();
        viewModel = std::make_unique<PetViewModel>();
        viewModel->set_pet_model(mockPetModel);
    }

    void TearDown() override {
        viewModel.reset();
        mockPetModel.reset();
    }

    std::shared_ptr<MockPetModel> mockPetModel;
    std::unique_ptr<PetViewModel> viewModel;
};

// 测试ViewModel的构造和初始化
TEST_F(PetViewModelTest, Constructor_ShouldInitializeCorrectly) {
    // 验证ViewModel构造时创建了命令对象
    EXPECT_NE(viewModel->get_move_command(), nullptr);
    EXPECT_NE(viewModel->get_switch_pet_command(), nullptr);
    
    // 验证触发器初始化
    EXPECT_NO_THROW(viewModel->get_trigger());
}

// 测试设置PetModel
TEST_F(PetViewModelTest, SetPetModel_ShouldEstablishConnection) {
    auto newMockModel = std::make_shared<MockPetModel>();
    
    // 测试设置新的模型
    viewModel->set_pet_model(newMockModel);
    
    // 验证模型已正确设置
    EXPECT_EQ(viewModel->get_pet_model(), newMockModel);
}

// 测试获取位置属性
TEST_F(PetViewModelTest, GetPosition_ShouldReturnModelPosition) {
    // 设置测试数据
    QPoint testPosition(100, 200);
    mockPetModel->m_test_info.position = testPosition;
    
    // 获取位置
    const QPoint* position = viewModel->get_position();
    
    // 验证返回正确的位置
    ASSERT_NE(position, nullptr);
    EXPECT_EQ(*position, testPosition);
}

// 测试获取当前动画属性
TEST_F(PetViewModelTest, GetCurrentAnimation_ShouldReturnModelAnimation) {
    // 设置测试数据
    QString testAnimation = ":/resources/gif/spider.gif";
    mockPetModel->m_test_info.currentAnimation = testAnimation;
    
    // 获取动画
    const QString* animation = viewModel->get_current_animation();
    
    // 验证返回正确的动画
    ASSERT_NE(animation, nullptr);
    EXPECT_EQ(*animation, testAnimation);
}

// 测试获取尺寸属性
TEST_F(PetViewModelTest, GetSize_ShouldReturnModelSize) {
    // 设置测试数据
    QSize testSize(150, 150);
    mockPetModel->m_test_info.size = testSize;
    
    // 获取尺寸
    const QSize* size = viewModel->get_size();
    
    // 验证返回正确的尺寸
    ASSERT_NE(size, nullptr);
    EXPECT_EQ(*size, testSize);
}

// 测试移动命令的获取
TEST_F(PetViewModelTest, GetMoveCommand_ShouldReturnValidCommand) {
    ICommandBase* moveCommand = viewModel->get_move_command();
    
    // 验证命令对象存在
    ASSERT_NE(moveCommand, nullptr);
}

// 测试切换宠物命令的获取
TEST_F(PetViewModelTest, GetSwitchPetCommand_ShouldReturnValidCommand) {
    ICommandBase* switchCommand = viewModel->get_switch_pet_command();
    
    // 验证命令对象存在
    ASSERT_NE(switchCommand, nullptr);
}

// 测试ViewModel的生命周期管理
TEST_F(PetViewModelTest, Lifecycle_ShouldHandleModelChanges) {
    // 获取初始模型
    auto initialModel = viewModel->get_pet_model();
    EXPECT_EQ(initialModel, mockPetModel);
    
    // 设置新模型
    auto newModel = std::make_shared<MockPetModel>();
    viewModel->set_pet_model(newModel);
    
    // 验证模型已更换
    EXPECT_EQ(viewModel->get_pet_model(), newModel);
    EXPECT_NE(viewModel->get_pet_model(), initialModel);
}

// 测试属性访问的一致性
TEST_F(PetViewModelTest, PropertyAccess_ShouldBeConsistent) {
    // 设置测试数据
    QPoint testPosition(300, 400);
    QString testAnimation = ":/resources/img/cassidy.png";
    QSize testSize(200, 200);
    
    mockPetModel->m_test_info.position = testPosition;
    mockPetModel->m_test_info.currentAnimation = testAnimation;
    mockPetModel->m_test_info.size = testSize;
    
    // 多次访问同一属性应返回相同结果
    const QPoint* pos1 = viewModel->get_position();
    const QPoint* pos2 = viewModel->get_position();
    EXPECT_EQ(pos1, pos2);
    EXPECT_EQ(*pos1, *pos2);
    
    const QString* anim1 = viewModel->get_current_animation();
    const QString* anim2 = viewModel->get_current_animation();
    EXPECT_EQ(anim1, anim2);
    EXPECT_EQ(*anim1, *anim2);
    
    const QSize* size1 = viewModel->get_size();
    const QSize* size2 = viewModel->get_size();
    EXPECT_EQ(size1, size2);
    EXPECT_EQ(*size1, *size2);
}

// 测试触发器功能
TEST_F(PetViewModelTest, Trigger_ShouldWork) {
    PropertyTrigger& trigger = viewModel->get_trigger();
    
    // 验证触发器对象存在且可用
    EXPECT_NO_THROW(trigger);
}

// 测试模型连接后的通知机制
TEST_F(PetViewModelTest, ModelConnection_ShouldEstablishNotificationCallback) {
    // 当设置模型时，应该建立通知回调连接
    auto newModel = std::make_shared<MockPetModel>();
    
    // 期望get_trigger被调用以建立连接
    EXPECT_CALL(*newModel, get_trigger())
        .Times(::testing::AtLeast(1))
        .WillRepeatedly(::testing::ReturnRef(newModel->m_test_trigger));
    
    viewModel->set_pet_model(newModel);
}