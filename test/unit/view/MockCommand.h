#ifndef MOCK_COMMAND_H
#define MOCK_COMMAND_H

#include <gmock/gmock.h>
#include "../../../src/common/CommandBase.h"

// Mock Command for View testing
class MockCommand : public ICommandBase {
public:
    MOCK_METHOD(int, exec, (ICommandParameter* p), (override));
};

// Mock Command Parameter for testing
class MockCommandParameter : public ICommandParameter {
public:
    MOCK_METHOD(int, getValue, (), (const));
    MOCK_METHOD(void, setValue, (int value), ());
    
    // Test data
    int m_test_value = 0;
    
    MockCommandParameter(int value = 0) : m_test_value(value) {
        ON_CALL(*this, getValue()).WillByDefault(::testing::Return(m_test_value));
        ON_CALL(*this, setValue(::testing::_)).WillByDefault([this](int value) {
            m_test_value = value;
        });
    }
};

#endif // MOCK_COMMAND_H
