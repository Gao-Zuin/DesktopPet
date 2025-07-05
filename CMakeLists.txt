cmake_minimum_required(VERSION 3.14)
project(DesktopPet LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用并行编译
if(MSVC)
    add_compile_options(/MP)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # MinGW并行编译优化
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe")
endif()

# Qt自动化工具
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 - 只需要Widgets组件，不需要Test
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# 收集源文件 - 只包含主程序源文件，排除测试文件
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
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

# 为MinGW添加额外的链接库
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32)
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