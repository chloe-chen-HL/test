#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <iostream>
#include <string>

// 假设这些是你的实际头文件
// #include "CommandEvent.h"
// #include "Command.h"
// #include "ToReadyPoseCommand.h"
// #include "YourClass.h"

// 模拟你的实际类结构
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

enum class CommandType {
    ACTION_MOVE_TO_READY,
    OTHER_COMMAND
};

class Command {
public:
    virtual CommandType get_type() const = 0;
    virtual ~Command() = default;
};

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

// 模拟你的实际类
class YourClass {
public:
    YourClass() : toReady_(nullptr) {}
    
    // 这是你要测试的实际方法
    bool event(QEvent* e) {
        if (e == nullptr) {
            return false;
        }
        
        if (e->type() == CommandEvent::EventType) {
            CommandEvent* event = static_cast<CommandEvent*>(e);
            auto com = event->get_command();
            
            // 模拟qDebug输出
            std::cout << "toready received com " << commandTypeToString(com->get_type()) << std::endl;
            
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
    
    // 获取toReady_指针用于测试验证
    std::shared_ptr<ToReadyPoseCommand> getToReady() const {
        return toReady_;
    }
    
private:
    Pose getReadyPose() {
        return Pose(1.0, 2.0, 3.0);
    }
    
    std::string commandTypeToString(CommandType type) {
        switch (type) {
            case CommandType::ACTION_MOVE_TO_READY:
                return "ACTION_MOVE_TO_READY";
            case CommandType::OTHER_COMMAND:
                return "OTHER_COMMAND";
            default:
                return "UNKNOWN";
        }
    }
    
    std::shared_ptr<ToReadyPoseCommand> toReady_;
};

// 测试类
class YourClassTest : public ::testing::Test {
protected:
    void SetUp() override {
        yourClass_ = std::make_unique<YourClass>();
    }
    
    void TearDown() override {
        yourClass_.reset();
    }
    
    std::unique_ptr<YourClass> yourClass_;
};

// 测试用例1: 正常处理ACTION_MOVE_TO_READY命令
TEST_F(YourClassTest, HandleActionMoveToReadyCommand) {
    // 准备测试数据
    auto command = std::make_shared<ToReadyPoseCommand>();
    CommandEvent event(command);
    
    // 执行被测试的方法
    bool result = yourClass_->event(&event);
    
    // 验证结果
    EXPECT_TRUE(result);
    EXPECT_TRUE(event.isAccepted());
    
    auto toReady = yourClass_->getToReady();
    EXPECT_NE(toReady, nullptr);
    
    // 验证pose设置
    Pose expectedPose(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(toReady->get_pose().x, expectedPose.x);
    EXPECT_DOUBLE_EQ(toReady->get_pose().y, expectedPose.y);
    EXPECT_DOUBLE_EQ(toReady->get_pose().z, expectedPose.z);
}

// 测试用例2: 处理其他类型的命令
TEST_F(YourClassTest, HandleOtherCommandType) {
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
    bool result = yourClass_->event(&event);
    
    // 验证结果
    EXPECT_FALSE(result);
    EXPECT_FALSE(event.isAccepted());
    EXPECT_EQ(yourClass_->getToReady(), nullptr);
}

// 测试用例3: 处理非CommandEvent类型的事件
TEST_F(YourClassTest, HandleNonCommandEvent) {
    // 创建一个非CommandEvent类型的事件
    QEvent otherEvent(QEvent::MouseButtonPress);
    
    // 执行被测试的方法
    bool result = yourClass_->event(&otherEvent);
    
    // 验证结果
    EXPECT_FALSE(result);
    EXPECT_EQ(yourClass_->getToReady(), nullptr);
}

// 测试用例4: 处理空指针
TEST_F(YourClassTest, HandleNullEvent) {
    // 测试空指针
    bool result = yourClass_->event(nullptr);
    
    // 验证结果
    EXPECT_FALSE(result);
    EXPECT_EQ(yourClass_->getToReady(), nullptr);
}

// 测试用例5: 动态转换失败的情况
TEST_F(YourClassTest, HandleDynamicCastFailure) {
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
    bool result = yourClass_->event(&event);
    
    // 验证结果 - 动态转换应该失败
    EXPECT_FALSE(result);
    EXPECT_FALSE(event.isAccepted());
    EXPECT_EQ(yourClass_->getToReady(), nullptr);
}

// 测试用例6: 验证pose设置的正确性
TEST_F(YourClassTest, VerifyPoseSetting) {
    auto command = std::make_shared<ToReadyPoseCommand>();
    CommandEvent event(command);
    
    // 设置初始pose
    Pose initialPose(0.0, 0.0, 0.0);
    command->set_pose(initialPose);
    
    // 执行被测试的方法
    bool result = yourClass_->event(&event);
    
    // 验证结果
    EXPECT_TRUE(result);
    EXPECT_TRUE(event.isAccepted());
    
    auto toReady = yourClass_->getToReady();
    EXPECT_NE(toReady, nullptr);
    
    // 验证pose被正确设置
    Pose expectedPose(1.0, 2.0, 3.0);
    EXPECT_EQ(toReady->get_pose(), expectedPose);
    EXPECT_NE(toReady->get_pose(), initialPose); // 确保pose被更新了
}

// 测试用例7: 多次调用同一个事件
TEST_F(YourClassTest, HandleSameEventMultipleTimes) {
    auto command = std::make_shared<ToReadyPoseCommand>();
    CommandEvent event(command);
    
    // 第一次调用
    bool result1 = yourClass_->event(&event);
    EXPECT_TRUE(result1);
    EXPECT_TRUE(event.isAccepted());
    
    // 第二次调用
    bool result2 = yourClass_->event(&event);
    EXPECT_TRUE(result2);
    EXPECT_TRUE(event.isAccepted());
    
    // 验证toReady_指针仍然有效
    EXPECT_NE(yourClass_->getToReady(), nullptr);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}