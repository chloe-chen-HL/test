# CommandEvent Handler 单元测试

这个项目包含了针对CommandEvent处理逻辑的gtest单元测试。

## 文件说明

- `test_command_event_handler_simple.cpp` - 简化版本的测试文件（不依赖Qt）
- `test_command_event_handler.cpp` - 完整版本的测试文件（依赖Qt）
- `Makefile.simple` - 简化版本的Makefile
- `Makefile` - 完整版本的Makefile（依赖Qt）

## 快速开始

### 1. 安装依赖

**Ubuntu/Debian:**
```bash
make -f Makefile.simple install-deps
```

**CentOS/RHEL/Fedora:**
```bash
make -f Makefile.simple install-deps-rpm
```

**Arch Linux:**
```bash
make -f Makefile.simple install-deps-arch
```

### 2. 编译测试

```bash
make -f Makefile.simple all
```

### 3. 运行测试

```bash
# 运行所有测试
make -f Makefile.simple test

# 运行测试并显示详细信息
make -f Makefile.simple test-verbose

# 运行特定测试
make -f Makefile.simple test-specific

# 运行测试并生成XML报告
make -f Makefile.simple test-xml

# 运行测试并显示颜色输出
make -f Makefile.simple test-color
```

## 测试用例说明

### 1. HandleActionMoveToReadyCommand
测试正常处理ACTION_MOVE_TO_READY命令的情况：
- 验证返回值为true
- 验证事件被接受
- 验证toReady_指针不为空
- 验证pose被正确设置

### 2. HandleOtherCommandType
测试处理其他类型命令的情况：
- 验证返回值为false
- 验证事件未被接受
- 验证toReady_指针为空

### 3. HandleNonCommandEvent
测试处理非CommandEvent类型事件的情况：
- 验证返回值为false
- 验证toReady_指针为空

### 4. HandleNullEvent
测试处理空指针的情况：
- 验证返回值为false
- 验证toReady_指针为空

### 5. HandleDynamicCastFailure
测试动态转换失败的情况：
- 验证返回值为false
- 验证事件未被接受
- 验证toReady_指针为空

### 6. VerifyPoseSetting
测试pose设置的正确性：
- 验证pose被正确更新
- 验证初始pose和最终pose不同

### 7. HandleSameEventMultipleTimes
测试多次调用同一个事件的情况：
- 验证多次调用都能成功
- 验证toReady_指针保持有效

## 测试覆盖的场景

1. **正常流程**: ACTION_MOVE_TO_READY命令被正确处理
2. **异常处理**: 其他类型的命令被忽略
3. **边界条件**: 空指针、非CommandEvent事件
4. **类型安全**: 动态转换失败的情况
5. **数据完整性**: pose设置的正确性
6. **重复调用**: 同一个事件的多次处理

## 如何适配到你的实际代码

1. **替换头文件包含**: 将模拟的类替换为你的实际类
2. **调整类名**: 根据你的实际类名调整测试代码
3. **修改方法名**: 根据你的实际方法名调整测试代码
4. **更新依赖**: 如果需要Qt，使用完整的Makefile

## 示例：适配到实际代码

```cpp
// 在你的实际代码中，将测试方法替换为：
bool YourClass::event(QEvent* e) {
    if (e->type() == CommandEvent::EventType) {
        CommandEvent* event = static_cast<CommandEvent*>(e);
        auto com = event->get_command();
        qDebug() << "toready received com" << Command::type2string(com->get_type());

        if (com->get_type() == CommandType::ACTION_MOVE_TO_READY) {
            toReady_ = std::dynamic_pointer_cast<ToReadyPoseCommand>(com);
            toReady_->set_pose(getReadyPose());
            qDebug() << "toReady pose is" << toReady_->get_pose();
            event->accept();
            return true;
        }
    }
    return false;
}
```

## 故障排除

### 编译错误
1. 确保已安装gtest和gmock
2. 检查编译器版本是否支持C++17
3. 确保所有依赖库都已正确安装

### 测试失败
1. 检查测试用例是否与你的实际代码逻辑匹配
2. 验证模拟类的行为是否与实际类一致
3. 检查边界条件和异常情况的处理

## 扩展测试

你可以根据需要添加更多测试用例：

1. **性能测试**: 测试大量事件的处理性能
2. **内存测试**: 测试内存泄漏情况
3. **并发测试**: 测试多线程环境下的行为
4. **集成测试**: 测试与其他组件的交互

## 有用的命令

```bash
# 查看所有可用的make目标
make -f Makefile.simple help

# 检查依赖是否已安装
make -f Makefile.simple check-deps

# 清理编译文件
make -f Makefile.simple clean
```