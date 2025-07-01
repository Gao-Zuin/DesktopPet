#include <gtest/gtest.h>
#include "../../../src/common/PropertyTrigger.h"

// 测试夹具
class PropertyTriggerTest : public ::testing::Test {
protected:
    void SetUp() override {
        trigger = new PropertyTrigger();
    }

    void TearDown() override {
        delete trigger;
    }

    PropertyTrigger* trigger{nullptr};

    // 回调计数器
    static int callbackCounter;
    static uint32_t lastEventId;
    static void* lastUserData;

    // 测试回调函数
    static void TestCallback(uint32_t id, void* pv) {
        callbackCounter++;
        lastEventId = id;
        lastUserData = pv;
    }
};

// 初始化静态成员
int PropertyTriggerTest::callbackCounter = 0;
uint32_t PropertyTriggerTest::lastEventId = 0;
void* PropertyTriggerTest::lastUserData = nullptr;

// 测试用例
TEST_F(PropertyTriggerTest, InitialState) {
    EXPECT_EQ(trigger->add(nullptr, nullptr), 1);
    trigger->clear();
}

TEST_F(PropertyTriggerTest, AddAndFireSingleCallback) {
    callbackCounter = 0;
    int userData = 42;
    uintptr_t cookie = trigger->add(TestCallback, &userData);
    
    EXPECT_EQ(cookie, 1);
    trigger->fire(100);
    
    EXPECT_EQ(callbackCounter, 1);
    EXPECT_EQ(lastEventId, 100);
    EXPECT_EQ(lastUserData, &userData);
}

TEST_F(PropertyTriggerTest, AddMultipleCallbacks) {
    callbackCounter = 0;
    int data1 = 1, data2 = 2;
    uintptr_t cookie1 = trigger->add(TestCallback, &data1);
    uintptr_t cookie2 = trigger->add(TestCallback, &data2);
    
    EXPECT_EQ(cookie1, 1);
    EXPECT_EQ(cookie2, 2);
    trigger->fire(200);
    EXPECT_EQ(callbackCounter, 2);
    EXPECT_EQ(lastEventId, 200);
    EXPECT_EQ(lastUserData, &data2);
}

TEST_F(PropertyTriggerTest, RemoveCallback) {
    int data1 = 1, data2 = 2;
    uintptr_t cookie1 = trigger->add(TestCallback, &data1);
    uintptr_t cookie2 = trigger->add(TestCallback, &data2);
    
    trigger->remove(cookie1);
    callbackCounter = 0;  // 重置计数器
    
    trigger->fire(300);
    EXPECT_EQ(callbackCounter, 1);
    EXPECT_EQ(lastEventId, 300);
    EXPECT_EQ(lastUserData, &data2);
}

TEST_F(PropertyTriggerTest, ClearAllCallbacks) {
    int data = 10;
    trigger->add(TestCallback, &data);
    trigger->add(TestCallback, nullptr);
    
    trigger->clear();
    callbackCounter = 0;
    
    trigger->fire(400);
    EXPECT_EQ(callbackCounter, 0);
}

TEST_F(PropertyTriggerTest, AddAfterRemove) {
    int data1 = 1, data2 = 2;
    uintptr_t cookie1 = trigger->add(TestCallback, &data1);
    
    trigger->remove(cookie1);
    uintptr_t cookie2 = trigger->add(TestCallback, &data2);
    
    EXPECT_EQ(cookie2, 2);  // 索引继续递增
    callbackCounter = 0;
    
    trigger->fire(500);
    EXPECT_EQ(callbackCounter, 1);
    EXPECT_EQ(lastUserData, &data2);
}

TEST_F(PropertyTriggerTest, FireWithNullCallbacks) {
    // 添加后立即移除
    uintptr_t cookie = trigger->add(TestCallback, nullptr);
    trigger->remove(cookie);
    
    // 添加空回调
    trigger->add(nullptr, nullptr);
    
    callbackCounter = 0;
    trigger->fire(600);
    EXPECT_EQ(callbackCounter, 0);
}

TEST_F(PropertyTriggerTest, MultipleFireOperations) {
    int data = 7;
    trigger->add(TestCallback, &data);
    
    trigger->fire(700);
    EXPECT_EQ(callbackCounter, 1);
    EXPECT_EQ(lastEventId, 700);
    
    trigger->fire(701);
    EXPECT_EQ(callbackCounter, 2);
    EXPECT_EQ(lastEventId, 701);
}

TEST_F(PropertyTriggerTest, CookieStabilityAfterRemove) {
    int data1 = 1, data2 = 2, data3 = 3;
    uintptr_t cookie1 = trigger->add(TestCallback, &data1);
    uintptr_t cookie2 = trigger->add(TestCallback, &data2);
    
    trigger->remove(cookie1);
    uintptr_t cookie3 = trigger->add(TestCallback, &data3);
    
    // 原始 cookie 应保持有效
    trigger->remove(cookie2);  // 应成功移除第二个回调
    callbackCounter = 0;
    
    trigger->fire(800);
    EXPECT_EQ(callbackCounter, 1);
    EXPECT_EQ(lastUserData, &data3);
}