# CMakeLists.txt - 备选构建方案（仅供参考）
cmake_minimum_required(VERSION 3.16)
project(ToReadyStateTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt
find_package(Qt6 REQUIRED COMPONENTS Core Test)

# 查找Google Test和Google Mock
find_package(GTest REQUIRED)
find_package(GMock REQUIRED)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
)

# 源文件
set(SOURCES
    main.cpp
    toreadystate.cpp
    test_toreadystate.cpp
    test_toreadystate_private.cpp
    test_toreadystate_performance.cpp
    mocks/mock_statebase.cpp
    mocks/mock_rtde.cpp
    mocks/mock_eyemodule.cpp
    mocks/mock_command.cpp
)

# 头文件
set(HEADERS
    toreadystate.h
    mocks/mock_statebase.h
    mocks/mock_rtde.h
    mocks/mock_eyemodule.h
    mocks/mock_command.h
    mocks/mock_events.h
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 链接库
target_link_libraries(${PROJECT_NAME}
    Qt6::Core
    Qt6::Test
    GTest::GTest
    GTest::Main
    GMock::GMock
    GMock::Main
    pthread
)

# 编译定义
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_DEPRECATED_WARNINGS
)

# 启用测试
enable_testing()
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME})