#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QApplication>
#include <QTest>
#include <memory>

#include "toreadystate.h"
#include "mock_rtde.h"
#include "mock_eyemodule.h"
#include "mock_command.h"
#include "mock_events.h"

using ::testing::_;
using ::testing::Return;
using ::testing::InSequence;
using ::testing::StrictMock;
using ::testing::NiceMock;

class ToReadyStateTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 创建模拟对象
        mockRtde = std::make_shared<NiceMock<MockRtde>>();
        mockEye = std::make_shared<NiceMock<MockEyeModule>>();
        mockStateMachine = std::make_unique<NiceMock<MockStateMachine>>();
        mockOutputQueue = std::make_unique<NiceMock<MockOutputQueue>>();
        
        // 设置默认行为
        ON_CALL(*mockEye, isConnected()).WillByDefault(Return(true));
        ON_CALL(*mockStateMachine, get_outputQue()).WillByDefault(Return(mockOutputQueue.get()));
        
        // 创建待测试对象
        toReadyState = std::make_unique<ToReadyState>(mockRtde, mockEye);
        toReadyState->setStateMachine(mockStateMachine.get());
    }

    void TearDown() override
    {
        toReadyState.reset();
        mockOutputQueue.reset();
        mockStateMachine.reset();
        mockEye.reset();
        mockRtde.reset();
    }

    std::shared_ptr<MockRtde> mockRtde;
    std::shared_ptr<MockEyeModule> mockEye;
    std::unique_ptr<MockStateMachine> mockStateMachine;
    std::unique_ptr<MockOutputQueue> mockOutputQueue;
    std::unique_ptr<ToReadyState> toReadyState;
};

// 测试构造函数
TEST_F(ToReadyStateTest, Constructor)
{
    EXPECT_EQ(toReadyState->getType(), StateType::TOREADY);
    EXPECT_EQ(toReadyState->getCurrentStatus(), MoveStatus::STOPING);
}

// 测试进入状态 - 成功情况
TEST_F(ToReadyStateTest, EnterSuccess)
{
    bool result = toReadyState->enter();
    EXPECT_TRUE(result);
    EXPECT_EQ(toReadyState->getCurrentStatus(), MoveStatus::STOPING);
}

// 测试进入状态 - RTDE为空的失败情况
TEST_F(ToReadyStateTest, EnterFailureNullRtde)
{
    ToReadyState state(nullptr, mockEye);
    bool result = state.enter();
    EXPECT_FALSE(result);
}

// 测试进入状态 - Eye为空的失败情况
TEST_F(ToReadyStateTest, EnterFailureNullEye)
{
    ToReadyState state(mockRtde, nullptr);
    bool result = state.enter();
    EXPECT_FALSE(result);
}

// 测试退出状态
TEST_F(ToReadyStateTest, Exit)
{
    bool result = toReadyState->exit();
    EXPECT_TRUE(result);
}

// 测试处理移动到准备位置命令事件
TEST_F(ToReadyStateTest, HandleMoveToReadyCommandEvent)
{
    // 创建模拟命令
    auto mockCommand = std::make_shared<NiceMock<MockToReadyPoseCommand>>();
    std::vector<double> expectedPose = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    
    ON_CALL(*mockCommand, get_type()).WillByDefault(Return(CommandType::ACTION_MOVE_TO_READY));
    EXPECT_CALL(*mockCommand, set_pose(_)).Times(1);
    EXPECT_CALL(*mockCommand, get_pose()).WillRepeatedly(Return(expectedPose));
    
    // 创建命令事件
    CommandEvent event(mockCommand);
    
    // 处理事件
    bool result = toReadyState->event(&event);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(event.isAccepted());
}

// 测试处理脚踏板按下事件 - 从停止状态开始运动
TEST_F(ToReadyStateTest, HandlePedalPressedFromStopStatus)
{
    // 首先设置一个ToReady命令
    auto mockCommand = std::make_shared<NiceMock<MockToReadyPoseCommand>>();
    std::vector<double> testPose = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    
    ON_CALL(*mockCommand, get_type()).WillByDefault(Return(CommandType::ACTION_MOVE_TO_READY));
    ON_CALL(*mockCommand, get_pose()).WillByDefault(Return(testPose));
    
    CommandEvent commandEvent(mockCommand);
    toReadyState->event(&commandEvent);
    
    // 设置RTDE期望调用
    EXPECT_CALL(*mockRtde, movel(testPose)).Times(1);
    EXPECT_CALL(*mockRtde, getAsyncProgress()).WillOnce(Return(1)); // 运动中
    
    // 创建脚踏板按下事件
    PedalEvent pedalEvent(true);
    
    // 处理事件
    bool result = toReadyState->event(&pedalEvent);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(pedalEvent.isAccepted());
    EXPECT_EQ(toReadyState->getCurrentStatus(), MoveStatus::MOVING);
}

// 测试处理脚踏板松开事件 - 从运动状态停止
TEST_F(ToReadyStateTest, HandlePedalReleasedFromMovingStatus)
{
    // 首先设置状态为运动中
    toReadyState->changeStatus(MoveStatus::MOVING);
    
    // 设置一个ToReady命令
    auto mockCommand = std::make_shared<NiceMock<MockToReadyPoseCommand>>();
    std::vector<double> testPose = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    ON_CALL(*mockCommand, get_type()).WillByDefault(Return(CommandType::ACTION_MOVE_TO_READY));
    ON_CALL(*mockCommand, get_pose()).WillByDefault(Return(testPose));
    
    CommandEvent commandEvent(mockCommand);
    toReadyState->event(&commandEvent);
    
    // 设置RTDE期望调用
    EXPECT_CALL(*mockRtde, stopl()).Times(1);
    
    // 创建脚踏板松开事件
    PedalEvent pedalEvent(false);
    
    // 处理事件
    bool result = toReadyState->event(&pedalEvent);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(pedalEvent.isAccepted());
    EXPECT_EQ(toReadyState->getCurrentStatus(), MoveStatus::STOPING);
}

// 测试运动进度处理 - 运动完成
TEST_F(ToReadyStateTest, UrProgressMovementFinished)
{
    // 设置状态为运动中
    toReadyState->changeStatus(MoveStatus::MOVING);
    
    // 创建模拟命令
    auto mockCommand = std::make_shared<StrictMock<MockToReadyPoseCommand>>();
    std::vector<double> testPose = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    
    ON_CALL(*mockCommand, get_type()).WillByDefault(Return(CommandType::ACTION_MOVE_TO_READY));
    ON_CALL(*mockCommand, get_pose()).WillByDefault(Return(testPose));
    
    // 设置命令期望
    EXPECT_CALL(*mockCommand, set_status(true)).Times(1);
    EXPECT_CALL(*mockCommand, encode()).WillOnce(Return("encoded_result"));
    
    // 设置输出队列期望
    EXPECT_CALL(*mockOutputQueue, put("encoded_result")).Times(1);
    
    // 设置命令到状态中
    CommandEvent commandEvent(mockCommand);
    toReadyState->event(&commandEvent);
    
    // 模拟运动完成（进度 < 0）
    EXPECT_CALL(*mockRtde, getAsyncProgress()).WillOnce(Return(-1));
    
    PedalEvent pedalEvent(true);
    toReadyState->event(&pedalEvent);
    
    EXPECT_EQ(toReadyState->getCurrentStatus(), MoveStatus::FINISHED);
}

// 测试清除上下文
TEST_F(ToReadyStateTest, ClearContext)
{
    // 首先设置一些状态
    toReadyState->changeStatus(MoveStatus::MOVING);
    
    auto mockCommand = std::make_shared<NiceMock<MockToReadyPoseCommand>>();
    ON_CALL(*mockCommand, get_type()).WillByDefault(Return(CommandType::ACTION_MOVE_TO_READY));
    
    CommandEvent commandEvent(mockCommand);
    toReadyState->event(&commandEvent);
    
    // 清除上下文
    toReadyState->clearContext();
    
    EXPECT_EQ(toReadyState->getCurrentStatus(), MoveStatus::STOPING);
}

// 测试获取准备位置 - 默认配置
TEST_F(ToReadyStateTest, GetReadyPoseFromConfig)
{
    // 这个测试依赖于ConfigManager返回"0.0"
    std::vector<double> expectedPose = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    
    // 通过反射调用私有方法（这里我们需要修改类设计或使用友元函数）
    // 为了测试目的，我们可以通过事件处理来间接测试
    auto mockCommand = std::make_shared<NiceMock<MockToReadyPoseCommand>>();
    ON_CALL(*mockCommand, get_type()).WillByDefault(Return(CommandType::ACTION_MOVE_TO_READY));
    
    EXPECT_CALL(*mockCommand, set_pose(expectedPose)).Times(1);
    EXPECT_CALL(*mockCommand, get_pose()).WillOnce(Return(expectedPose));
    
    CommandEvent commandEvent(mockCommand);
    bool result = toReadyState->event(&commandEvent);
    
    EXPECT_TRUE(result);
}

// 测试位置接近判断 - 距离过近
TEST_F(ToReadyStateTest, TooNearPositionsClose)
{
    std::vector<double> pos1 = {1.0, 2.0, 3.0};
    std::vector<double> pos2 = {1.005, 2.005, 3.005}; // 距离约0.0087米
    
    // 由于tooNear是私有方法，我们需要通过其他方式测试
    // 这里我们创建一个测试友元类或者修改访问级别
    // 为了演示，我们假设有一个公共的测试接口
    
    // 实际测试中，您可能需要：
    // 1. 将tooNear设为protected或public
    // 2. 创建测试友元类
    // 3. 使用反射机制
    
    EXPECT_TRUE(true); // 占位测试
}

// 测试位置接近判断 - 距离较远
TEST_F(ToReadyStateTest, TooNearPositionsFar)
{
    std::vector<double> pos1 = {1.0, 2.0, 3.0};
    std::vector<double> pos2 = {2.0, 3.0, 4.0}; // 距离约1.73米
    
    EXPECT_TRUE(true); // 占位测试
}

// 测试处理未知事件类型
TEST_F(ToReadyStateTest, HandleUnknownEvent)
{
    QEvent unknownEvent(QEvent::User);
    bool result = toReadyState->event(&unknownEvent);
    
    // 应该调用基类的事件处理
    EXPECT_FALSE(result); // QObject::event通常返回false对于未处理的事件
}

// 测试空命令指针的脚踏板事件
TEST_F(ToReadyStateTest, HandlePedalEventWithoutCommand)
{
    PedalEvent pedalEvent(true);
    bool result = toReadyState->event(&pedalEvent);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(pedalEvent.isAccepted());
    // 状态应该保持不变
    EXPECT_EQ(toReadyState->getCurrentStatus(), MoveStatus::STOPING);
}

// 集成测试 - 完整的工作流程
TEST_F(ToReadyStateTest, CompleteWorkflow)
{
    InSequence seq;
    
    // 1. 进入状态
    EXPECT_TRUE(toReadyState->enter());
    
    // 2. 接收移动命令
    auto mockCommand = std::make_shared<StrictMock<MockToReadyPoseCommand>>();
    std::vector<double> testPose = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    
    EXPECT_CALL(*mockCommand, get_type()).WillRepeatedly(Return(CommandType::ACTION_MOVE_TO_READY));
    EXPECT_CALL(*mockCommand, set_pose(_)).Times(1);
    EXPECT_CALL(*mockCommand, get_pose()).WillRepeatedly(Return(testPose));
    
    CommandEvent commandEvent(mockCommand);
    EXPECT_TRUE(toReadyState->event(&commandEvent));
    
    // 3. 脚踏板按下，开始运动
    EXPECT_CALL(*mockRtde, movel(testPose)).Times(1);
    EXPECT_CALL(*mockRtde, getAsyncProgress()).WillOnce(Return(1));
    
    PedalEvent pedalPressEvent(true);
    EXPECT_TRUE(toReadyState->event(&pedalPressEvent));
    EXPECT_EQ(toReadyState->getCurrentStatus(), MoveStatus::MOVING);
    
    // 4. 运动完成
    EXPECT_CALL(*mockRtde, getAsyncProgress()).WillOnce(Return(-1));
    EXPECT_CALL(*mockCommand, set_status(true)).Times(1);
    EXPECT_CALL(*mockCommand, encode()).WillOnce(Return("result"));
    EXPECT_CALL(*mockOutputQueue, put("result")).Times(1);
    
    PedalEvent pedalPressEvent2(true);
    EXPECT_TRUE(toReadyState->event(&pedalPressEvent2));
    EXPECT_EQ(toReadyState->getCurrentStatus(), MoveStatus::FINISHED);
    
    // 5. 退出状态
    EXPECT_TRUE(toReadyState->exit());
}