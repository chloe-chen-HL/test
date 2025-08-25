# ToReadyState 单元测试项目

这个项目使用Qt Creator和Google Test框架对ToReadyState类进行全面的单元测试。

## 项目结构

```
ToReadyStateTest/
├── main.cpp                           # 主程序入口
├── ToReadyStateTest.pro               # Qt项目文件
├── toreadystate.h                     # ToReadyState类头文件
├── toreadystate.cpp                   # ToReadyState类实现
├── test_toreadystate.cpp              # 主要单元测试
├── test_toreadystate_private.cpp      # 私有方法测试
├── test_toreadystate_performance.cpp  # 性能和边界测试
├── mocks/                             # 模拟对象目录
│   ├── mock_statebase.h              # StateBase模拟实现
│   ├── mock_rtde.h                   # RTDE控制器模拟
│   ├── mock_eyemodule.h              # 视觉模块模拟
│   ├── mock_command.h                # 命令系统模拟
│   ├── mock_events.h                 # 事件系统模拟
│   └── *.cpp                         # 对应的实现文件
├── CMakeLists.txt                    # CMake构建文件（备选）
└── README.md                         # 本文件
```

## 在Qt Creator中设置项目

### 1. 安装依赖

首先确保系统中安装了Google Test和Google Mock：

#### Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install libgtest-dev libgmock-dev
sudo apt-get install cmake build-essential

# 编译安装gtest
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp *.a /usr/lib

# 编译安装gmock
cd /usr/src/gmock
sudo cmake .
sudo make
sudo cp *.a /usr/lib
```

#### CentOS/RHEL:
```bash
sudo yum install gtest-devel gmock-devel
```

#### macOS (使用Homebrew):
```bash
brew install googletest
```

### 2. 在Qt Creator中打开项目

1. 启动Qt Creator
2. 选择 "File" -> "Open File or Project"
3. 选择 `ToReadyStateTest.pro` 文件
4. 配置构建套件（Kit）
5. 点击 "Configure Project"

### 3. 构建和运行测试

#### 方法1: 使用Qt Creator界面
1. 在项目面板中选择构建配置（Debug/Release）
2. 点击绿色的"运行"按钮或按Ctrl+R
3. 测试结果将显示在"应用程序输出"面板中

#### 方法2: 使用命令行
```bash
cd /path/to/project
qmake ToReadyStateTest.pro
make
./bin/ToReadyStateTest
```

## 测试覆盖范围

### 1. 基本功能测试 (`test_toreadystate.cpp`)
- ✅ 构造函数测试
- ✅ 进入/退出状态测试
- ✅ 事件处理测试（命令事件、脚踏板事件）
- ✅ 状态转换测试
- ✅ 错误处理测试
- ✅ 集成测试（完整工作流程）

### 2. 私有方法测试 (`test_toreadystate_private.cpp`)
- ✅ `tooNear()` 方法测试
- ✅ `getReadyPose()` 方法测试
- ✅ `urProgress()` 方法测试
- ✅ `urScript()` 方法测试
- ✅ `clearContext()` 方法测试

### 3. 性能和边界测试 (`test_toreadystate_performance.cpp`)
- ✅ 事件处理性能测试
- ✅ 快速连续事件压力测试
- ✅ 内存使用测试
- ✅ 边界值测试
- ✅ 并发访问测试

## 代码分析

### ToReadyState类的主要功能：

1. **状态管理**: 继承自StateBase，实现状态机模式
2. **设备控制**: 通过RTDE接口控制UR机器人
3. **视觉集成**: 与EyeModule视觉系统集成
4. **事件处理**: 处理命令事件和脚踏板事件
5. **运动控制**: 执行线性运动到准备位置

### 关键方法解析：

#### `enter()` - 进入状态
- 验证必要组件（RTDE、Eye）是否已初始化
- 清除之前的上下文状态
- 返回初始化结果

#### `event()` - 事件处理
- 处理`CommandEvent`: 接收移动到准备位置的命令
- 处理`PedalEvent`: 响应脚踏板按下/松开事件
- 状态转换: STOPING -> MOVING -> FINISHED

#### `urProgress()` - 运动进度处理
- 监控机器人运动进度
- 处理运动完成事件
- 发送反馈到输出队列

#### `getReadyPose()` - 获取准备位置
- 从配置文件读取默认关节角度
- 检查是否有最新的位置设置命令
- 返回6轴关节角度向量

#### `tooNear()` - 位置接近判断
- 计算两个位置的欧几里得距离
- 判断是否小于安全阈值（0.01米）
- 用于避免机器人位置冲突

## 测试结果解读

### 成功的测试输出示例：
```
[==========] Running 25 tests from 4 test suites.
[----------] Global test environment set-up.
[----------] 12 tests from ToReadyStateTest
[ RUN      ] ToReadyStateTest.Constructor
[       OK ] ToReadyStateTest.Constructor (1 ms)
[ RUN      ] ToReadyStateTest.EnterSuccess
[       OK ] ToReadyStateTest.EnterSuccess (0 ms)
...
[----------] 12 tests from ToReadyStateTest (15 ms total)
[==========] 25 tests from 4 test suites ran. (89 ms total)
[  PASSED  ] 25 tests.
```

### 性能测试输出示例：
```
处理 1000 个事件总时间: 15234 微秒
平均每个事件处理时间: 15.234 微秒
处理 1000 个脚踏板事件总时间: 8567 微秒
并发操作成功次数: 387
```

## 故障排除

### 常见问题和解决方案：

1. **链接错误 - 找不到gtest库**
   ```
   解决方案: 确保在.pro文件中正确设置了库路径
   LIBS += -lgtest -lgmock -pthread
   ```

2. **MOC错误 - Qt元对象编译失败**
   ```
   解决方案: 确保Q_OBJECT宏在类声明中，并且类继承自QObject
   ```

3. **测试运行时崩溃**
   ```
   解决方案: 检查模拟对象的设置，确保所有必要的期望都已配置
   ```

4. **性能测试失败**
   ```
   解决方案: 调整性能阈值，考虑系统负载的影响
   ```

## 扩展测试

要添加新的测试用例：

1. 在相应的测试文件中添加新的TEST_F宏
2. 设置必要的模拟对象期望
3. 执行被测试的功能
4. 验证结果

示例：
```cpp
TEST_F(ToReadyStateTest, NewTestCase)
{
    // 设置
    EXPECT_CALL(*mockRtde, someMethod()).Times(1);
    
    // 执行
    bool result = toReadyState->someFunction();
    
    // 验证
    EXPECT_TRUE(result);
}
```

## 持续集成

这个测试套件可以集成到CI/CD管道中：

```yaml
# .github/workflows/test.yml 示例
name: Unit Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install qt6-base-dev libgtest-dev libgmock-dev
    - name: Run tests
      run: |
        qmake ToReadyStateTest.pro
        make
        ./bin/ToReadyStateTest
```

## 贡献指南

1. 为新功能添加对应的测试用例
2. 确保所有测试都能通过
3. 保持测试覆盖率在90%以上
4. 遵循命名约定和代码风格

## 许可证

本项目仅用于学习和测试目的。