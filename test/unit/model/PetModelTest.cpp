#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../../../src/model/PetModel.h"
#include "../../../src/model/base/PetInfo.h"

using namespace testing;

// 测试夹具
class PetModelTest : public Test {
protected:
    void SetUp() override {
        // 重置回调计数
        callbackCount = 0;
        lastPropertyId = 0;
        
        // 添加回调到模型
        model.get_trigger().add([](uint32_t id, void* pv) {
            auto self = static_cast<PetModelTest*>(pv);
            self->callbackCount++;
            self->lastPropertyId = id;
        }, this);
    }

    PetModel model;
    
    // 回调状态
    int callbackCount = 0;
    uint32_t lastPropertyId = 0;
    
    // 辅助常量
    const QPoint TEST_POS{100, 200};
    const QSize TEST_SIZE{50, 50};
    const QString TEST_ANIM = "walking";
    const PetState TEST_STATE = PetState::Sleeping;
    const PetType TEST_TYPE = PetType::Cassidy;
};

// 初始状态测试
TEST_F(PetModelTest, InitialState) {
    const auto* info = model.get_info();
    
    EXPECT_EQ(info->position, QPoint(100, 100));
    EXPECT_EQ(info->size, QSize(200, 200));
    EXPECT_EQ(info->currentAnimation, "");
    EXPECT_EQ(info->state, PetState::Idle);
    EXPECT_EQ(info->isVisible, true);
    EXPECT_EQ(info->petType, PetType::Spider);
    
    EXPECT_EQ(callbackCount, 0);
}

// 位置变更测试
TEST_F(PetModelTest, PositionChange) {
    // 首次变更应触发
    model.change_position(TEST_POS);
    EXPECT_EQ(model.get_info()->position, TEST_POS);
    EXPECT_EQ(callbackCount, 1);
    
    // 微小移动不触发 (小于阈值)
    callbackCount = 0;
    model.change_position(TEST_POS + QPoint(1, 1));
    EXPECT_EQ(callbackCount, 0);
    
    // 超过阈值应触发
    model.change_position(TEST_POS + QPoint(3, 0));
    EXPECT_EQ(callbackCount, 1);
}

// 状态变更测试
TEST_F(PetModelTest, StateChange) {
    // 变更应触发
    model.change_state(TEST_STATE);
    EXPECT_EQ(model.get_info()->state, TEST_STATE);
    EXPECT_EQ(callbackCount, 1);
    
    // 相同状态不触发
    callbackCount = 0;
    model.change_state(TEST_STATE);
    EXPECT_EQ(callbackCount, 0);
}

// 动画变更测试
TEST_F(PetModelTest, AnimationChange) {
    // 变更应触发
    model.change_animation(TEST_ANIM);
    EXPECT_EQ(model.get_info()->currentAnimation, TEST_ANIM);
    EXPECT_EQ(callbackCount, 1);
    
    // 相同动画不触发
    callbackCount = 0;
    model.change_animation(TEST_ANIM);
    EXPECT_EQ(callbackCount, 0);
}

// 可见性变更测试
TEST_F(PetModelTest, VisibilityChange) {
    // 变更应触发
    model.change_visibility(false);
    EXPECT_FALSE(model.get_info()->isVisible);
    EXPECT_EQ(callbackCount, 1);
    
    // 相同状态不触发
    callbackCount = 0;
    model.change_visibility(false);
    EXPECT_EQ(callbackCount, 0);
    
    // 变回可见应触发
    model.change_visibility(true);
    EXPECT_TRUE(model.get_info()->isVisible);
    EXPECT_EQ(callbackCount, 1);
}

// 大小变更测试
TEST_F(PetModelTest, SizeChange) {
    // 变更应触发
    model.change_size(TEST_SIZE);
    EXPECT_EQ(model.get_info()->size, TEST_SIZE);
    EXPECT_EQ(callbackCount, 1);
    
    // 相同大小不触发
    callbackCount = 0;
    model.change_size(TEST_SIZE);
    EXPECT_EQ(callbackCount, 0);
}

// 类型变更测试
TEST_F(PetModelTest, TypeChange) {
    // 变更应触发
    model.change_pet_type(TEST_TYPE);
    EXPECT_EQ(model.get_info()->petType, TEST_TYPE);
    EXPECT_EQ(callbackCount, 1);
    
    // 相同类型不触发
    callbackCount = 0;
    model.change_pet_type(TEST_TYPE);
    EXPECT_EQ(callbackCount, 0);
}

// 多属性组合变更测试
TEST_F(PetModelTest, MultipleChanges) {
    // 同时变更多个属性
    model.change_position(TEST_POS);
    model.change_state(TEST_STATE);
    model.change_animation(TEST_ANIM);
    
    // 应触发3次回调
    EXPECT_EQ(callbackCount, 3);
    
    // 验证状态
    EXPECT_EQ(model.get_info()->position, TEST_POS);
    EXPECT_EQ(model.get_info()->state, TEST_STATE);
    EXPECT_EQ(model.get_info()->currentAnimation, TEST_ANIM);
}

// 无变化不触发测试
TEST_F(PetModelTest, NoChangeNoTrigger) {
    // 设置初始状态
    model.change_position(TEST_POS);
    model.change_state(TEST_STATE);
    callbackCount = 0;
    
    // 尝试设置相同值
    model.change_position(TEST_POS);
    model.change_state(TEST_STATE);
    model.change_animation(""); // 初始空字符串
    
    // 不应触发任何回调
    EXPECT_EQ(callbackCount, 0);
}

// 边界值测试
TEST_F(PetModelTest, BoundaryValues) {
    // 位置边界
    model.change_position(QPoint(INT_MAX, INT_MIN));
    EXPECT_EQ(model.get_info()->position, QPoint(INT_MAX, INT_MIN));
    EXPECT_EQ(callbackCount, 1);
    
    // 大小边界
    callbackCount = 0;
    model.change_size(QSize(1, 1));
    EXPECT_EQ(callbackCount, 1);
    
    // 最小大小
    callbackCount = 0;
    model.change_size(QSize(0, 0));
    EXPECT_EQ(callbackCount, 1);
}