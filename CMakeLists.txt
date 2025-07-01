cmake_minimum_required(VERSION 3.5) # CMake install : https://cmake.org/download/
project(DesktopPet LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "D:/ProgramFiles/Qt/6.9.1/mingw_64") # Qt Kit Dir
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

find_package(Qt6 COMPONENTS Widgets REQUIRED) # Qt COMPONENTS
find_package(GTest CONFIG REQUIRED)
find_package(GMock CONFIG REQUIRED)

# 收集源文件（排除main.cpp，为了测试）
file(GLOB_RECURSE APP_SOURCES 
    "src/app/*.cpp"
    "src/app/*.h"
    "src/common/*.cpp"
    "src/common/*.h"
    "src/model/*.cpp"
    "src/model/*.h"
    "src/view/*.cpp"
    "src/view/*.h"
    "src/viewmodel/*.cpp"
    "src/viewmodel/*.h"
)

# 单独处理main.cpp
set(MAIN_SOURCE "src/app/main.cpp")

# 添加资源文件
set(RESOURCES resources.qrc)

# Specify MSVC UTF-8 encoding   
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# 主应用程序
add_executable(${PROJECT_NAME}
    WIN32 # If you need a terminal for debug, please comment this statement 
    ${APP_SOURCES}
    ${MAIN_SOURCE}
    ${RESOURCES}
) 
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets) # Qt6 Shared Library

# 创建一个库，包含除main.cpp之外的所有源文件，供测试使用
add_library(${PROJECT_NAME}_lib STATIC
    ${APP_SOURCES}
    ${RESOURCES}
)
target_link_libraries(${PROJECT_NAME}_lib PRIVATE Qt6::Widgets)

# 测试相关
file(GLOB_RECURSE TEST_SOURCES 
    "test/unit/*.cpp"
    "test/unit/*.h"
)

if(TEST_SOURCES)
    add_executable(${PROJECT_NAME}_tests
        ${TEST_SOURCES}
    )
    
    target_link_libraries(${PROJECT_NAME}_tests PRIVATE 
        ${PROJECT_NAME}_lib
        Qt6::Widgets
        GTest::gtest 
        GTest::gtest_main
        GTest::gmock
        GTest::gmock_main
    )
    
    # 包含测试目录
    target_include_directories(${PROJECT_NAME}_tests PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    
    # 注册测试
    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_tests)
endif()