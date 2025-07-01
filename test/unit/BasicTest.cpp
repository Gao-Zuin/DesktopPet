#include <gtest/gtest.h>
#include <gmock/gmock.h>

// 简单的测试验证Google Test和Google Mock是否正常工作
class BasicTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// 基本功能测试
TEST_F(BasicTest, GoogleTest_ShouldWork) {
    EXPECT_EQ(1 + 1, 2);
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
}

// Google Mock测试
class MockInterface {
public:
    MOCK_METHOD(int, getValue, (), (const));
    MOCK_METHOD(void, setValue, (int value), ());
};

TEST_F(BasicTest, GoogleMock_ShouldWork) {
    MockInterface mockObj;
    
    // 设置期望
    EXPECT_CALL(mockObj, getValue())
        .WillOnce(::testing::Return(42));
    
    // 验证Mock对象工作
    int result = mockObj.getValue();
    EXPECT_EQ(result, 42);
}