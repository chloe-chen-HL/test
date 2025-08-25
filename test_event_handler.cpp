#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

// 包含我们的头文件（注意顺序，qt_mock.h要在其他头文件之前）
#include "qt_mock.h"
#include "command_types.h"
#include "event_system.h"
#include "event_handler.h"

using ::testing::_;
using ::testing::Return;
using ::testing::StrictMock;

// Mock类定义
class MockCommand : public Command {
public:
    MOCK_METHOD(CommandType, get_type, (), (const, override));
};

class MockQEvent : public QEvent {
public:
    MOCK_METHOD(int, type, (), (const, override));
    MOCK_METHOD(void, accept, (), (override));
    MOCK_METHOD(void, ignore, (), (override));
};

// 测试夹具类
class EventHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        handler_ = std::make_unique<EventHandler>();
    }
    
    void TearDown() override {
        handler_.reset();
    }
    
    std::unique_ptr<EventHandler> handler_;
};

// 测试用例1: 测试正确处理ACTION_MOVE_TO_READY命令事件
TEST_F(EventHandlerTest, HandleActionMoveToReadyCommand) {
    // 创建ToReadyPoseCommand
    auto toReadyCommand = std::make_shared<ToReadyPoseCommand>();
    
    // 创建CommandEvent
    CommandEvent event(toReadyCommand);
    
    // 调用处理函数
    bool result = handler_->handleEvent(&event);
    
    // 验证结果
    EXPECT_TRUE(result);
    EXPECT_TRUE(event.isAccepted());
    
    // 验证toReady_被正确设置
    auto storedCommand = handler_->getToReady();
    ASSERT_NE(storedCommand, nullptr);
    
    // 验证pose被设置为ready pose
    Pose expectedPose = {1.0, 2.0, 3.0, 0.1, 0.2, 0.3};
    EXPECT_EQ(storedCommand->get_pose(), expectedPose);
}

// 测试用例2: 测试处理非CommandEvent类型的事件
TEST_F(EventHandlerTest, HandleNonCommandEvent) {
    MockQEvent mockEvent;
    EXPECT_CALL(mockEvent, type())
        .WillOnce(Return(999)); // 非CommandEvent::EventType
    
    bool result = handler_->handleEvent(&mockEvent);
    
    EXPECT_FALSE(result);
    EXPECT_EQ(handler_->getToReady(), nullptr);
}

// 测试用例3: 测试处理CommandEvent但命令类型不是ACTION_MOVE_TO_READY
TEST_F(EventHandlerTest, HandleCommandEventWithDifferentCommandType) {
    auto mockCommand = std::make_shared<MockCommand>();
    EXPECT_CALL(*mockCommand, get_type())
        .WillRepeatedly(Return(CommandType::OTHER_COMMAND));
    
    CommandEvent event(mockCommand);
    
    bool result = handler_->handleEvent(&event);
    
    EXPECT_FALSE(result);
    EXPECT_FALSE(event.isAccepted());
    EXPECT_EQ(handler_->getToReady(), nullptr);
}

// 测试用例4: 测试dynamic_pointer_cast失败的情况
TEST_F(EventHandlerTest, HandleCommandEventWithInvalidCommandCast) {
    // 创建一个MockCommand，它返回ACTION_MOVE_TO_READY类型，但实际不是ToReadyPoseCommand
    auto mockCommand = std::make_shared<MockCommand>();
    EXPECT_CALL(*mockCommand, get_type())
        .WillRepeatedly(Return(CommandType::ACTION_MOVE_TO_READY));
    
    CommandEvent event(mockCommand);
    
    bool result = handler_->handleEvent(&event);
    
    // 由于dynamic_pointer_cast会失败，toReady_会是nullptr，但事件仍会被接受
    EXPECT_TRUE(result);
    EXPECT_TRUE(event.isAccepted());
    // 注意：这里可能会导致程序崩溃，在实际代码中应该添加nullptr检查
}

// 测试用例5: 测试Command::type2string功能
TEST_F(EventHandlerTest, TestCommandTypeToString) {
    EXPECT_EQ(Command::type2string(CommandType::ACTION_MOVE_TO_READY), "ACTION_MOVE_TO_READY");
    EXPECT_EQ(Command::type2string(CommandType::OTHER_COMMAND), "OTHER_COMMAND");
}

// 测试用例6: 测试Pose结构体的相等比较
TEST_F(EventHandlerTest, TestPoseEquality) {
    Pose pose1 = {1.0, 2.0, 3.0, 0.1, 0.2, 0.3};
    Pose pose2 = {1.0, 2.0, 3.0, 0.1, 0.2, 0.3};
    Pose pose3 = {1.1, 2.0, 3.0, 0.1, 0.2, 0.3};
    
    EXPECT_EQ(pose1, pose2);
    EXPECT_NE(pose1, pose3);
}

// 测试用例7: 测试ToReadyPoseCommand的基本功能
TEST_F(EventHandlerTest, TestToReadyPoseCommandBasics) {
    ToReadyPoseCommand command;
    
    EXPECT_EQ(command.get_type(), CommandType::ACTION_MOVE_TO_READY);
    
    Pose testPose = {5.0, 6.0, 7.0, 0.5, 0.6, 0.7};
    command.set_pose(testPose);
    
    EXPECT_EQ(command.get_pose(), testPose);
}

// 主函数
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}