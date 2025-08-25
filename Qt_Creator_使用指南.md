# Qt Creator 使用 Google Test 进行单元测试指南

## 1. 环境准备

### 1.1 安装 Google Test
```bash
# Ubuntu/Debian
sudo apt-get install libgtest-dev
sudo apt-get install cmake
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp lib/*.a /usr/lib

# 或者使用包管理器
sudo apt-get install googletest
```

### 1.2 安装 Qt Creator
```bash
# 从 Qt 官网下载并安装 Qt Creator
# 或者使用包管理器
sudo apt-get install qtcreator
```

## 2. 项目配置

### 2.1 打开项目
1. 启动 Qt Creator
2. 选择 "File" → "Open File or Project"
3. 选择 `ToReadyStateTest.pro` 文件
4. 点击 "Open"

### 2.2 配置构建套件
1. 在项目视图中，选择项目
2. 在 "Build Settings" 中确保选择了正确的构建套件
3. 确保编译器支持 C++11 或更高版本

### 2.3 验证 Google Test 配置
在 `.pro` 文件中确认以下配置：
```qmake
LIBS += -lgtest -lgtest_main -lpthread
```

## 3. 编译和运行测试

### 3.1 编译项目
1. 按 `Ctrl+B` 或点击 "Build" → "Build Project"
2. 确保编译成功，没有错误

### 3.2 运行测试
1. 按 `Ctrl+R` 或点击 "Build" → "Run"
2. 测试将在控制台输出结果

### 3.3 调试测试
1. 在测试代码中设置断点
2. 按 `F5` 或点击 "Debug" → "Start Debugging"
3. 使用调试器逐步执行测试

## 4. 测试结果分析

### 4.1 控制台输出
测试运行后，控制台会显示类似以下输出：
```
[==========] Running 20 tests from 1 test case.
[----------] Global test environment set-up.
[----------] 1 test from ToReadyStateTest
[ RUN      ] ToReadyStateTest.Constructor
[       OK ] ToReadyStateTest.Constructor (0 ms)
[----------] 1 test from ToReadyStateTest
[ RUN      ] ToReadyStateTest.EnterSuccess
[       OK ] ToReadyStateTest.EnterSuccess (1 ms)
...
[----------] Global test environment tear-down
[==========] 20 tests from 1 test case ran. (15 ms total)
[  PASSED  ] 20 tests.
```

### 4.2 测试报告
- **PASSED**: 测试通过
- **FAILED**: 测试失败
- **SKIPPED**: 测试被跳过

## 5. 高级功能

### 5.1 测试过滤器
在 Qt Creator 的运行配置中，可以设置测试过滤器：
```
--gtest_filter=ToReadyStateTest.*
```

### 5.2 测试重复运行
```bash
--gtest_repeat=10
```

### 5.3 详细输出
```bash
--gtest_verbose
```

## 6. 常见问题解决

### 6.1 链接错误
如果遇到链接错误，检查：
1. Google Test 是否正确安装
2. `.pro` 文件中的 `LIBS` 配置
3. 库文件路径是否正确

### 6.2 编译错误
常见编译错误及解决方案：
1. **C++11 特性不支持**: 在 `.pro` 文件中添加 `CONFIG += c++11`
2. **头文件找不到**: 检查 `INCLUDEPATH` 配置
3. **Qt 模块缺失**: 确保 `QT += core testlib` 已添加

### 6.3 运行时错误
1. **段错误**: 检查指针初始化和空指针检查
2. **内存泄漏**: 使用 Valgrind 等工具检测
3. **测试超时**: 检查无限循环或阻塞操作

## 7. 最佳实践

### 7.1 测试组织
- 每个测试类对应一个被测试类
- 使用描述性的测试名称
- 按功能分组测试用例

### 7.2 Mock 对象使用
```cpp
// 使用 Google Mock 创建 Mock 对象
class MockRtde : public sgnioRTDEControl {
public:
    MOCK_METHOD(void, movel, (const std::vector<double>& pose), (override));
    MOCK_METHOD(void, stopl, (), (override));
    MOCK_METHOD(int, getAsyncProgress, (), (override));
};
```

### 7.3 测试夹具
```cpp
class ToReadyStateTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前的设置
    }
    
    void TearDown() override {
        // 每个测试后的清理
    }
    
    // 共享的测试对象
    std::unique_ptr<ToReadyState> toReadyState_;
};
```

### 7.4 断言使用
```cpp
// 基本断言
EXPECT_EQ(actual, expected);
EXPECT_TRUE(condition);
EXPECT_FALSE(condition);

// 浮点数比较
EXPECT_DOUBLE_EQ(actual, expected);

// 容器比较
EXPECT_EQ(vector1, vector2);

// 字符串比较
EXPECT_STREQ(str1, str2);
```

## 8. 持续集成

### 8.1 自动化测试
在 CI/CD 流程中集成测试：
```bash
# 编译
qmake ToReadyStateTest.pro
make

# 运行测试
./ToReadyStateTest --gtest_output=xml:test_results.xml
```

### 8.2 测试覆盖率
使用 gcov 和 lcov 生成覆盖率报告：
```bash
# 编译时添加覆盖率选项
CONFIG += coverage

# 生成覆盖率报告
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

## 9. 性能测试

### 9.1 基准测试
```cpp
TEST_F(ToReadyStateTest, PerformanceTest) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // 执行被测试的操作
    for (int i = 0; i < 1000; ++i) {
        toReadyState_->tooNear(p1, p2);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    EXPECT_LT(duration.count(), 1000); // 期望执行时间小于1ms
}
```

## 10. 总结

通过 Qt Creator 和 Google Test 的组合，可以：
1. 高效地编写和运行单元测试
2. 使用强大的调试工具
3. 集成到开发工作流程中
4. 提供详细的测试报告和覆盖率分析

这种组合为 C++ 项目的质量保证提供了强有力的支持。