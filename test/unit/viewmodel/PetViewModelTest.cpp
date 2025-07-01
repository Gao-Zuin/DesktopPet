#include <gtest/gtest.h>
#include <memory>
#include <QPoint>
#include <QSize>
#include <QString>
#include "../../../src/viewmodel/PetViewModel.h"
#include "../../../src/model/PetModel.h"
#include "../../../src/common/PropertyIds.h"

class PetViewModelTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        viewModel = std::make_unique<PetViewModel>();
        petModel = std::make_shared<PetModel>();
    }

    void TearDown() override
    {
        viewModel.reset();
        petModel.reset();
    }

    std::unique_ptr<PetViewModel> viewModel;
    std::shared_ptr<PetModel> petModel;
};

// 测试构造函数
TEST_F(PetViewModelTest, Constructor)
{
    EXPECT_NE(viewModel, nullptr);
    EXPECT_EQ(viewModel->get_pet_model(), nullptr);
}

// 测试设置和获取PetModel
TEST_F(PetViewModelTest, SetAndGetPetModel)
{
    viewModel->set_pet_model(petModel);

    EXPECT_EQ(viewModel->get_pet_model(), petModel);
    EXPECT_NE(viewModel->get_pet_model(), nullptr);
}

// 测试属性访问器
TEST_F(PetViewModelTest, PropertyAccessors)
{
    viewModel->set_pet_model(petModel);

    // 测试位置属性
    const QPoint *position = viewModel->get_position();
    EXPECT_NE(position, nullptr);

    // 测试动画属性
    const QString *animation = viewModel->get_current_animation();
    EXPECT_NE(animation, nullptr);

    // 测试尺寸属性
    const QSize *size = viewModel->get_size();
    EXPECT_NE(size, nullptr);
}

// 测试命令获取
TEST_F(PetViewModelTest, CommandAccessors)
{
    // 测试移动命令
    ICommandBase *moveCommand = viewModel->get_move_command();
    EXPECT_NE(moveCommand, nullptr);

    // 测试切换宠物命令
    ICommandBase *switchCommand = viewModel->get_switch_pet_command();
    EXPECT_NE(switchCommand, nullptr);

    // 确保每次调用返回相同的实例
    EXPECT_EQ(moveCommand, viewModel->get_move_command());
    EXPECT_EQ(switchCommand, viewModel->get_switch_pet_command());
}

// 测试触发器获取
TEST_F(PetViewModelTest, TriggerAccessor)
{
    PropertyTrigger &trigger = viewModel->get_trigger();

    // 测试触发器引用的有效性
    // 通过获取两次引用来验证它们指向同一个对象
    PropertyTrigger &trigger2 = viewModel->get_trigger();
    EXPECT_EQ(&trigger, &trigger2);
}

// 测试ViewModel与Model的连接
TEST_F(PetViewModelTest, ViewModelModelConnection)
{
    viewModel->set_pet_model(petModel);

    // 验证模型被正确设置
    EXPECT_EQ(viewModel->get_pet_model(), petModel);

    // 验证属性访问器返回有效指针
    EXPECT_NE(viewModel->get_position(), nullptr);
    EXPECT_NE(viewModel->get_current_animation(), nullptr);
    EXPECT_NE(viewModel->get_size(), nullptr);
}

// 测试触发器功能
TEST_F(PetViewModelTest, TriggerFunctionality)
{
    int callbackCount = 0;

    // 添加回调到ViewModel的触发器
    viewModel->get_trigger().add([](uint32_t id, void *p)
                                 {
        int* counter = static_cast<int*>(p);
        (*counter)++; }, &callbackCount);

    // 手动触发事件来测试触发器
    viewModel->get_trigger().fire(PROP_ID_PET_POSITION);

    EXPECT_EQ(callbackCount, 1);

    // 再次触发不同的事件
    viewModel->get_trigger().fire(PROP_ID_PET_ANIMATION);

    EXPECT_EQ(callbackCount, 2);
}

// 测试多个回调的触发器
TEST_F(PetViewModelTest, MultipleCallbackTrigger)
{
    int callback1Count = 0;
    int callback2Count = 0;

    // 添加多个回调
    viewModel->get_trigger().add([](uint32_t id, void *p)
                                 {
        int* counter = static_cast<int*>(p);
        (*counter)++; }, &callback1Count);

    viewModel->get_trigger().add([](uint32_t id, void *p)
                                 {
        int* counter = static_cast<int*>(p);
        (*counter)++; }, &callback2Count);

    // 触发事件
    viewModel->get_trigger().fire(PROP_ID_PET_SIZE);

    // 两个回调都应该被调用
    EXPECT_EQ(callback1Count, 1);
    EXPECT_EQ(callback2Count, 1);
}

// 测试不同属性ID的处理
TEST_F(PetViewModelTest, DifferentPropertyIdHandling) {
    int counters[4] = {0, 0, 0, 0}; // positionCount, animationCount, sizeCount, otherCount
    
    // 添加一个能区分不同属性ID的回调
    viewModel->get_trigger().add([](uint32_t id, void* p) {
        int* counters = static_cast<int*>(p);
        
        switch (id) {
        case PROP_ID_PET_POSITION:
            counters[0]++;
            break;
        case PROP_ID_PET_ANIMATION:
            counters[1]++;
            break;
        case PROP_ID_PET_SIZE:
            counters[2]++;
            break;
        default:
            counters[3]++;
            break;
        }
    }, counters);
    
    // 触发不同的属性变化
    viewModel->get_trigger().fire(PROP_ID_PET_POSITION);
    viewModel->get_trigger().fire(PROP_ID_PET_ANIMATION);
    viewModel->get_trigger().fire(PROP_ID_PET_SIZE);
    viewModel->get_trigger().fire(999); // 未知ID
    
    EXPECT_EQ(counters[0], 1); // positionCount
    EXPECT_EQ(counters[1], 1); // animationCount
    EXPECT_EQ(counters[2], 1); // sizeCount
    EXPECT_EQ(counters[3], 1); // otherCount
}