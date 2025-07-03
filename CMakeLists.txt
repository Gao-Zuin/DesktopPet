cmake_minimum_required(VERSION 3.14) # 提高版本要求以支持FetchContent
project(DesktopPet LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 移除硬编码的Qt路径，让CMake自动发现Qt
# set(CMAKE_PREFIX_PATH "D:/ProgramFiles/Qt/6.9.1/mingw_64") # Qt Kit Dir

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 COMPONENTS Widgets Test REQUIRED) # Qt COMPONENTS

# 使用FetchContent获取Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
)

# 防止覆盖父项目的编译器/链接器设置
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# 收集源文件
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.h"
    "src/*.ui"
)

# 添加资源文件
set(RESOURCES resources.qrc)

# Specify MSVC UTF-8 encoding   
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# MinGW编译选项
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall -Wextra)
    # MinGW使用静态链接库，不需要shared选项
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    # 修复MinGW链接问题 - 添加必要的链接器标志
    if(WIN32)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,windows -mwindows")
    endif()
endif()

add_executable(${PROJECT_NAME}
    WIN32 # If you need a terminal for debug, please comment this statement 
    ${SOURCES}
    ${RESOURCES}
) 
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets) # Qt6 Shared Library

# 为MinGW添加额外的链接库
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32)
endif()

# 创建库文件供测试使用（排除main.cpp）
file(GLOB_RECURSE LIB_SOURCES 
    "src/app/*.cpp" "src/app/*.h"
    "src/model/*.cpp" "src/model/*.h"
    "src/viewmodel/*.cpp" "src/viewmodel/*.h"
    "src/view/*.cpp" "src/view/*.h"
    "src/common/*.cpp" "src/common/*.h"
)

# 排除main.cpp文件
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_library(DesktopPetLib STATIC ${LIB_SOURCES} ${RESOURCES})
target_link_libraries(DesktopPetLib PUBLIC Qt6::Widgets)
target_include_directories(DesktopPetLib PUBLIC src)

# 启用测试
enable_testing()

# 检查单元测试文件
file(GLOB_RECURSE UNIT_TEST_SOURCES "test/unit/*.cpp" "test/unit/*.h")
# 检查集成测试文件
file(GLOB_RECURSE INTEGRATION_TEST_SOURCES "test/integration/*.cpp" "test/integration/*.h")

if(UNIT_TEST_SOURCES OR INTEGRATION_TEST_SOURCES)
    # 创建单元测试可执行文件
    if(UNIT_TEST_SOURCES)
        message(STATUS "Found unit test sources: ${UNIT_TEST_SOURCES}")
        add_executable(DesktopPetUnitTests ${UNIT_TEST_SOURCES})
        target_link_libraries(DesktopPetUnitTests 
            PRIVATE 
            DesktopPetLib
            gtest_main
            gmock_main
            Qt6::Widgets
            Qt6::Test
        )
        target_include_directories(DesktopPetUnitTests PRIVATE src)
        
        # 添加单元测试
        add_test(NAME UnitTests COMMAND DesktopPetUnitTests)
    endif()
    
    # 创建集成测试可执行文件
    if(INTEGRATION_TEST_SOURCES)
        message(STATUS "Found integration test sources: ${INTEGRATION_TEST_SOURCES}")
        add_executable(DesktopPetIntegrationTests ${INTEGRATION_TEST_SOURCES})
        target_link_libraries(DesktopPetIntegrationTests 
            PRIVATE 
            DesktopPetLib
            gtest_main
            gmock_main
            Qt6::Widgets
            Qt6::Test
        )
        target_include_directories(DesktopPetIntegrationTests PRIVATE src)
        
        # 添加集成测试
        add_test(NAME IntegrationTests COMMAND DesktopPetIntegrationTests)
    endif()
    
    # 创建统一的测试可执行文件（包含所有测试）
    if(UNIT_TEST_SOURCES AND INTEGRATION_TEST_SOURCES)
        set(ALL_TEST_SOURCES ${UNIT_TEST_SOURCES} ${INTEGRATION_TEST_SOURCES})
        add_executable(DesktopPetAllTests ${ALL_TEST_SOURCES})
        target_link_libraries(DesktopPetAllTests 
            PRIVATE 
            DesktopPetLib
            gtest_main
            gmock_main
            Qt6::Widgets
            Qt6::Test
        )
        target_include_directories(DesktopPetAllTests PRIVATE src)
        
        # 添加所有测试
        add_test(NAME AllTests COMMAND DesktopPetAllTests)
    endif()
else()
    message(STATUS "No test sources found")
endif()

# Windows平台自动DLL部署
if(WIN32)
    # 查找windeployqt工具
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)
    
    if(WINDEPLOYQT_EXECUTABLE)
        message(STATUS "Found windeployqt: ${WINDEPLOYQT_EXECUTABLE}")
        
        # 为主程序添加DLL部署
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Deploying Qt libraries for ${PROJECT_NAME}"
        )
        
        # 为测试程序添加DLL部署（如果存在）
        if(TARGET DesktopPetTests)
            add_custom_command(TARGET DesktopPetTests POST_BUILD
                COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:DesktopPetTests>
                COMMENT "Deploying Qt libraries for DesktopPetTests"
            )
        endif()
    else()
        message(WARNING "windeployqt not found. You may need to manually copy Qt DLLs.")
        message(STATUS "To deploy manually, run: windeployqt.exe <path_to_executable>")
    endif()
    
    # 创建批处理脚本用于手动部署（备用方案）
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/deploy-windows.bat.in"
        "${CMAKE_CURRENT_BINARY_DIR}/deploy-windows.bat"
        @ONLY
    )
endif()

# 添加自定义目标用于完整构建和部署
add_custom_target(deploy
    DEPENDS ${PROJECT_NAME}
    COMMENT "Building and deploying the application"
)