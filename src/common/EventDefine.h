#ifndef __EVENT_DEFINE_H__
#define __EVENT_DEFINE_H__

#include <iostream>

struct TestEvent{
    int TestInt;
    std::string TestString;
};

struct AddExperienceEvent{
    int experience;
};

#endif