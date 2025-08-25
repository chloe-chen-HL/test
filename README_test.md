# GTest 单元测试说明

这个项目为给定的C++代码片段创建了完整的GTest单元测试环境，不使用CMake。

## 文件结构

```
├── command_types.h      # 命令类型定义和相关类
├── event_system.h       # 事件系统定义
├── event_handler.h      # 事件处理器（包含要测试的代码）
├── qt_mock.h           # Qt相关功能的模拟实现
├── test_event_handler.cpp # GTest测试用例
├── Makefile            # 编译和运行测试的Makefile
└── README_test.md      # 本说明文件
```

## 安装依赖

### Ubuntu/Debian系统
```bash
# 检查依赖是否已安装
make check-deps

# 安装GTest和GMock
make install-deps
# 或者手动安装
sudo apt-get update
sudo apt-get install -y libgtest-dev libgmock-dev
```

### 其他系统
请根据你的系统包管理器安装相应的gtest和gmock开发包。

## 编译和运行测试

```bash
# 编译测试程序
make

# 编译并运行测试
make test

# 运行测试并显示详细输出
make test-verbose

# 清理编译文件
make clean

# 重新编译
make rebuild
```

## 测试用例说明

测试文件包含以下测试用例：

1. **HandleActionMoveToReadyCommand** - 测试正确处理ACTION_MOVE_TO_READY命令事件
2. **HandleNonCommandEvent** - 测试处理非CommandEvent类型的事件
3. **HandleCommandEventWithDifferentCommandType** - 测试处理不同命令类型的CommandEvent
4. **HandleCommandEventWithInvalidCommandCast** - 测试dynamic_pointer_cast失败的情况
5. **TestCommandTypeToString** - 测试Command::type2string功能
6. **TestPoseEquality** - 测试Pose结构体的相等比较
7. **TestToReadyPoseCommandBasics** - 测试ToReadyPoseCommand的基本功能

## 代码覆盖的测试场景

- ✅ 事件类型检查 (`e->type() == CommandEvent::EventType`)
- ✅ 命令类型检查 (`com->get_type() == CommandType::ACTION_MOVE_TO_READY`)
- ✅ 智能指针转换 (`std::dynamic_pointer_cast<ToReadyPoseCommand>`)
- ✅ Pose设置和获取
- ✅ 事件接受 (`event->accept()`)
- ✅ 函数返回值测试
- ✅ 边界情况和异常情况

## 其他功能

```bash
# 代码格式化（需要clang-format）
make format

# 静态分析（需要cppcheck）
make analyze

# 显示帮助
make help
```

## 注意事项

1. 这个测试环境模拟了Qt的相关功能（如qDebug），不需要完整的Qt环境
2. 测试中发现了原代码中的一个潜在问题：当`dynamic_pointer_cast`失败时，`toReady_`会是`nullptr`，后续调用`set_pose`会导致程序崩溃
3. 建议在实际代码中添加nullptr检查：
   ```cpp
   toReady_ = std::dynamic_pointer_cast<ToReadyPoseCommand>(com);
   if (toReady_) {
       toReady_->set_pose(getReadyPose());
       qDebug() << "toReady pose is" << toReady_->get_pose();
       event->accept();
       return true;
   }
   ```

## 运行示例

```bash
$ make test
g++ -std=c++14 -Wall -Wextra -g -O0 -I. -I/usr/include/gtest -I/usr/include/gmock -c test_event_handler.cpp -o test_event_handler.o
g++ -std=c++14 -Wall -Wextra -g -O0 -o test_event_handler test_event_handler.o -lgtest -lgmock -lpthread
./test_event_handler
[==========] Running 7 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 7 tests from EventHandlerTest
[ RUN      ] EventHandlerTest.HandleActionMoveToReadyCommand
[       OK ] EventHandlerTest.HandleActionMoveToReadyCommand
[ RUN      ] EventHandlerTest.HandleNonCommandEvent
[       OK ] EventHandlerTest.HandleNonCommandEvent
...
[----------] 7 tests from EventHandlerTest (X ms total)
[==========] 7 tests from 1 test suite ran. (X ms total)
[  PASSED  ] 7 tests.
```