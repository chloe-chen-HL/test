# StateBase单元测试说明

## 项目结构
```
workspace/
├── test_statebase.pro      # Qt Creator项目文件
├── test_statebase.cpp      # 主要的单元测试文件
├── statebase.h            # StateBase头文件（测试版本）
├── statebase.cpp          # StateBase实现文件（测试版本）
├── mock_classes.h         # Mock类头文件
├── mock_classes.cpp       # Mock类实现文件
├── util/
│   └── utils.h           # 工具类头文件
└── README_TESTING.md     # 本说明文件
```

## 在Qt Creator中设置和运行测试

### 1. 安装GTest框架
在Ubuntu/Debian系统中：
```bash
sudo apt-get install libgtest-dev
sudo apt-get install cmake
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp *.a /usr/lib
```

### 2. 在Qt Creator中打开项目
1. 启动Qt Creator
2. 选择 "File" -> "Open File or Project"
3. 导航到项目目录并选择 `test_statebase.pro` 文件
4. 点击 "Open" 打开项目

### 3. 配置项目
1. Qt Creator会自动检测项目配置
2. 确保选择了正确的构建套件(Kit)
3. 项目会自动配置为控制台应用程序

### 4. 构建项目
1. 点击左下角的"构建"按钮（锤子图标）
2. 或者使用快捷键 Ctrl+B
3. 检查"编译输出"窗口确保没有错误

### 5. 运行测试
1. 点击左下角的"运行"按钮（绿色三角形）
2. 或者使用快捷键 Ctrl+R
3. 测试结果会显示在"应用程序输出"窗口中

## 测试覆盖范围

本测试套件包含了15个全面的测试用例：

### 基础功能测试
1. **ConstructorTest** - 构造函数测试
2. **GetStateTypeTest** - getStateType方法测试
3. **StateMachineAccessTest** - StateMachine访问器测试
4. **VirtualFunctionTest** - 虚函数测试

### 状态管理测试
5. **ChangeStatusTest** - changeStatus方法测试
6. **ChangeStatusThreadSafetyTest** - 线程安全性测试
7. **ClearContextTest** - clearContext方法测试

### 事件处理测试
8. **EventHandlingNonCommandEventTest** - 非CommandEvent处理
9. **EventHandlingLightControlCommandTest** - 灯光控制命令处理
10. **EventHandlingDifferentColorsTest** - 不同颜色测试
11. **EventHandlingNoStateMachineTest** - 无StateMachine的事件处理
12. **EventHandlingStateMachineWithoutNeedleTest** - 无needle的StateMachine
13. **EventHandlingNonLightControlCommandTest** - 非灯光控制命令

### 高级功能测试
14. **MultipleStateTypesTest** - 多种状态类型测试
15. **MoveSemanticTest** - 移动语义测试

## 预期输出

成功运行时，您应该看到类似以下的输出：
```
[==========] Running 15 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 15 tests from StateBaseTest
[ RUN      ] StateBaseTest.ConstructorTest
[       OK ] StateBaseTest.ConstructorTest (0 ms)
[ RUN      ] StateBaseTest.GetStateTypeTest
[       OK ] StateBaseTest.GetStateTypeTest (0 ms)
...
[----------] 15 tests from StateBaseTest (X ms total)

[----------] Global test environment tear-down
[==========] 15 tests from 1 test suite ran. (X ms total)
[  PASSED  ] 15 tests.
```

## 故障排除

### 常见问题：

1. **链接错误 - 找不到GTest库**
   - 确保已正确安装GTest
   - 检查.pro文件中的库路径是否正确

2. **编译错误 - 找不到头文件**
   - 确保所有必要的头文件都在项目目录中
   - 检查INCLUDEPATH设置

3. **运行时错误**
   - 确保所有依赖项都已正确链接
   - 检查Mock类的实现是否完整

### 调试建议：
1. 使用Qt Creator的调试器(F5)来逐步执行测试
2. 在测试失败时查看"问题"窗口的详细错误信息
3. 使用qDebug()输出来跟踪执行流程

## 扩展测试

要添加新的测试用例：
1. 在`test_statebase.cpp`中添加新的TEST_F函数
2. 如果需要新的Mock对象，在`mock_classes.h`中添加
3. 重新构建并运行测试

## 注意事项

- 这个测试框架使用了Mock对象来模拟Qt和其他依赖项
- UNIT_TEST_BUILD宏用于区分测试环境和生产环境
- 测试是独立的，每个测试都有自己的Setup和TearDown
- 线程安全测试可能在不同系统上有不同的执行时间