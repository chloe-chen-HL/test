#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <QDebug>
#include <QEvent>

// 假设的头文件包含（根据你的实际项目调整）
// #include "CommandEvent.h"
// #include "Command.h"
// #include "ToReadyPoseCommand.h"

// Mock类定义
class MockCommandEvent : public QEvent {
public:
    enum EventType {
        CommandEventType = QEvent::User + 1
    };
    
    MockCommandEvent() : QEvent(static_cast<QEvent::Type>(CommandEventType)) {}
    
    virtual std::shared_ptr<Command> get_command() const = 0;
    virtual void accept() = 0;
};

class MockCommand {
public:
    virtual CommandType get_type() const = 0;
    virtual ~MockCommand() = default;
};

class MockToReadyPoseCommand : public MockCommand {
public:
    MockToReadyPoseCommand() : pose_() {}
    
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

// 测试类
class CommandEventHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 设置测试环境
    }
    
    void TearDown() override {
        // 清理测试环境
    }
    
    // 模拟的Pose类
    struct Pose {
        double x, y, z;
        Pose(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}
    };
    
    // 模拟的CommandType枚举
    enum class CommandType {
        ACTION_MOVE_TO_READY,
        OTHER_COMMAND
    };
    
    // 模拟的Command类
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
    
    // 被测试的方法（模拟你的实际代码）
    bool handleCommandEvent(QEvent* e) {
        if (e->type() == CommandEvent::EventType) {
            CommandEvent* event = static_cast<CommandEvent*>(e);
            auto com = event->get_command();
            
            // 模拟qDebug输出
            std::cout << "toready received com " << commandTypeToString(com->get_type()) << std::endl;
            
            if (com->get_type() == CommandType::ACTION_MOVE_TO_READY) {
                toReady_ = std::dynamic_pointer_cast<ToReadyPoseCommand>(com);
                toReady_->set_pose(getReadyPose());
                std::cout << "toReady pose is " << toReady_->get_pose().x 
                         << ", " << toReady_->get_pose().y 
                         << ", " << toReady_->get_pose().z << std::endl;
                event->accept();
                return true;
            }
        }
        return false;
    }
    
    Pose getReadyPose() {
        return Pose(1.0, 2.0, 3.0);
    }
    
    std::string commandTypeToString(CommandType type) {
        switch (type) {
            case CommandType::ACTION_MOVE_TO_READY:
                return "ACTION_MOVE_TO_READY";
            default:
                return "UNKNOWN";
        }
    }
    
    std::shared_ptr<ToReadyPoseCommand> toReady_;
};

// 测试用例
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

TEST_F(CommandEventHandlerTest, HandleNonCommandEvent) {
    // 创建一个非CommandEvent类型的事件
    QEvent otherEvent(QEvent::Type::MouseButtonPress);
    
    // 执行被测试的方法
    bool result = handleCommandEvent(&otherEvent);
    
    // 验证结果
    EXPECT_FALSE(result);
    EXPECT_EQ(toReady_, nullptr);
}

TEST_F(CommandEventHandlerTest, HandleNullEvent) {
    // 测试空指针
    bool result = handleCommandEvent(nullptr);
    
    // 验证结果
    EXPECT_FALSE(result);
    EXPECT_EQ(toReady_, nullptr);
}

// 测试动态转换失败的情况
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
    EXPECT_TRUE(result); // 因为类型匹配，所以返回true
    EXPECT_TRUE(event.isAccepted());
    EXPECT_EQ(toReady_, nullptr); // 动态转换失败，应该为nullptr
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}