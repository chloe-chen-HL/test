#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <iostream>
#include <string>

// 模拟的Pose类
struct Pose {
    double x, y, z;
    Pose(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
    
    bool operator==(const Pose& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    
    bool operator!=(const Pose& other) const {
        return !(*this == other);
    }
};

// 模拟的CommandType枚举
enum class CommandType {
    ACTION_MOVE_TO_READY,
    OTHER_COMMAND
};

// 模拟的Command基类
class Command {
public:
    virtual CommandType get_type() const = 0;
    virtual ~Command() = default;
};

// 模拟的ToReadyPoseCommand类
class ToReadyPoseCommand : public Command {
public:
    ToReadyPoseCommand() : pose_() {}
    
    CommandType get_type() const override {
        return CommandType::ACTION_MOVE_TO_READY;
    }
    
    void set_pose(const Pose& pose) {
        pose_ = pose;
    }
    
    Pose get_pose() const {
        return pose_;
    }
    
private:
    Pose pose_;
};

// 模拟的QEvent基类
class QEvent {
public:
    enum Type {
        User = 1000,
        MouseButtonPress = 2
    };
    
    QEvent(Type type) : type_(type) {}
    virtual ~QEvent() = default;
    
    Type type() const { return type_; }
    
private:
    Type type_;
};

// 模拟的CommandEvent类
class CommandEvent : public QEvent {
public:
    static const QEvent::Type EventType = static_cast<QEvent::Type>(QEvent::User + 1);
    
    CommandEvent(std::shared_ptr<Command> command) 
        : QEvent(EventType), command_(command) {}
    
    std::shared_ptr<Command> get_command() const {
        return command_;
    }
    
    void accept() {
        accepted_ = true;
    }
    
    bool isAccepted() const {
        return accepted_;
    }
    
private:
    std::shared_ptr<Command> command_;
    bool accepted_ = false;
};

// 模拟的Command::type2string函数
std::string CommandTypeToString(CommandType type) {
    switch (type) {
        case CommandType::ACTION_MOVE_TO_READY:
            return "ACTION_MOVE_TO_READY";
        case CommandType::OTHER_COMMAND:
            return "OTHER_COMMAND";
        default:
            return "UNKNOWN";
    }
}

// 测试类
class CommandEventHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        toReady_ = nullptr;
    }
    
    void TearDown() override {
        toReady_ = nullptr;
    }
    
    // 被测试的方法（模拟你的实际代码）
    bool handleCommandEvent(QEvent* e) {
        if (e == nullptr) {
            return false;
        }
        
        if (e->type() == CommandEvent::EventType) {
            CommandEvent* event = static_cast<CommandEvent*>(e);
            auto com = event->get_command();
            
            // 模拟qDebug输出
            std::cout << "toready received com " << CommandTypeToString(com->get_type()) << std::endl;
            
            if (com->get_type() == CommandType::ACTION_MOVE_TO_READY) {
                toReady_ = std::dynamic_pointer_cast<ToReadyPoseCommand>(com);
                if (toReady_) {
                    toReady_->set_pose(getReadyPose());
                    std::cout << "toReady pose is " << toReady_->get_pose().x 
                             << ", " << toReady_->get_pose().y 
                             << ", " << toReady_->get_pose().z << std::endl;
                    event->accept();
                    return true;
                }
            }
        }
        return false;
    }
    
    Pose getReadyPose() {
        return Pose(1.0, 2.0, 3.0);
    }
    
    std::shared_ptr<ToReadyPoseCommand> toReady_;
};

// 测试用例1: 处理ACTION_MOVE_TO_READY命令
TEST_F(CommandEventHandlerTest, HandleActionMoveToReadyCommand) {
    // 准备测试数据
    auto command = std::make_shared<ToReadyPoseCommand>();
    CommandEvent event(command);
    
    // 执行被测试的方法
    bool result = handleCommandEvent(&event);
    
    // 验证结果
    EXPECT_TRUE(result);
    EXPECT_TRUE(event.isAccepted());
    EXPECT_NE(toReady_, nullptr);
    
    // 验证pose设置
    Pose expectedPose = getReadyPose();
    EXPECT_DOUBLE_EQ(toReady_->get_pose().x, expectedPose.x);
    EXPECT_DOUBLE_EQ(toReady_->get_pose().y, expectedPose.y);
    EXPECT_DOUBLE_EQ(toReady_->get_pose().z, expectedPose.z);
}

// 测试用例2: 处理其他类型的命令
TEST_F(CommandEventHandlerTest, HandleOtherCommandType) {
    // 创建一个其他类型的命令
    class OtherCommand : public Command {
    public:
        CommandType get_type() const override {
            return CommandType::OTHER_COMMAND;
        }
    };
    
    auto command = std::make_shared<OtherCommand>();
    CommandEvent event(command);
    
    // 执行被测试的方法
    bool result = handleCommandEvent(&event);
    
    // 验证结果
    EXPECT_FALSE(result);
    EXPECT_FALSE(event.isAccepted());
    EXPECT_EQ(toReady_, nullptr);
}

// 测试用例3: 处理非CommandEvent类型的事件
TEST_F(CommandEventHandlerTest, HandleNonCommandEvent) {
    // 创建一个非CommandEvent类型的事件
    QEvent otherEvent(QEvent::MouseButtonPress);
    
    // 执行被测试的方法
    bool result = handleCommandEvent(&otherEvent);
    
    // 验证结果
    EXPECT_FALSE(result);
    EXPECT_EQ(toReady_, nullptr);
}

// 测试用例4: 处理空指针
TEST_F(CommandEventHandlerTest, HandleNullEvent) {
    // 测试空指针
    bool result = handleCommandEvent(nullptr);
    
    // 验证结果
    EXPECT_FALSE(result);
    EXPECT_EQ(toReady_, nullptr);
}

// 测试用例5: 动态转换失败的情况
TEST_F(CommandEventHandlerTest, HandleDynamicCastFailure) {
    // 创建一个Command但不是ToReadyPoseCommand
    class BaseCommand : public Command {
    public:
        CommandType get_type() const override {
            return CommandType::ACTION_MOVE_TO_READY;
        }
    };
    
    auto command = std::make_shared<BaseCommand>();
    CommandEvent event(command);
    
    // 执行被测试的方法
    bool result = handleCommandEvent(&event);
    
    // 验证结果 - 动态转换应该失败，toReady_应该为nullptr
    EXPECT_FALSE(result); // 因为动态转换失败，所以返回false
    EXPECT_FALSE(event.isAccepted());
    EXPECT_EQ(toReady_, nullptr); // 动态转换失败，应该为nullptr
}

// 测试用例6: 验证pose设置的正确性
TEST_F(CommandEventHandlerTest, VerifyPoseSetting) {
    auto command = std::make_shared<ToReadyPoseCommand>();
    CommandEvent event(command);
    
    // 设置初始pose
    Pose initialPose(0.0, 0.0, 0.0);
    command->set_pose(initialPose);
    
    // 执行被测试的方法
    bool result = handleCommandEvent(&event);
    
    // 验证结果
    EXPECT_TRUE(result);
    EXPECT_TRUE(event.isAccepted());
    EXPECT_NE(toReady_, nullptr);
    
    // 验证pose被正确设置
    Pose expectedPose = getReadyPose();
    EXPECT_EQ(toReady_->get_pose(), expectedPose);
    EXPECT_NE(toReady_->get_pose(), initialPose); // 确保pose被更新了
}

// 测试用例7: 多次调用同一个事件
TEST_F(CommandEventHandlerTest, HandleSameEventMultipleTimes) {
    auto command = std::make_shared<ToReadyPoseCommand>();
    CommandEvent event(command);
    
    // 第一次调用
    bool result1 = handleCommandEvent(&event);
    EXPECT_TRUE(result1);
    EXPECT_TRUE(event.isAccepted());
    
    // 第二次调用
    bool result2 = handleCommandEvent(&event);
    EXPECT_TRUE(result2);
    EXPECT_TRUE(event.isAccepted());
    
    // 验证toReady_指针仍然有效
    EXPECT_NE(toReady_, nullptr);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}