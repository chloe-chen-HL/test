#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "toreadystate.h"

// 测试友元类，用于访问ToReadyState的私有成员
class ToReadyStateTestHelper
{
public:
    static bool testTooNear(ToReadyState* state, std::vector<double>& p1, std::vector<double>& p2)
    {
        return state->tooNear(p1, p2);
    }
    
    static std::vector<double> testGetReadyPose(ToReadyState* state)
    {
        return state->getReadyPose();
    }
    
    static void testUrProgress(ToReadyState* state, int progress)
    {
        state->urProgress(progress);
    }
    
    static std::vector<std::string> testUrScript(ToReadyState* state)
    {
        return state->urScript();
    }
};

// 为了让这个工作，需要在ToReadyState类中添加友元声明
// friend class ToReadyStateTestHelper;

class ToReadyStatePrivateTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mockRtde = std::make_shared<testing::NiceMock<MockRtde>>();
        mockEye = std::make_shared<testing::NiceMock<MockEyeModule>>();
        toReadyState = std::make_unique<ToReadyState>(mockRtde, mockEye);
    }

    std::shared_ptr<MockRtde> mockRtde;
    std::shared_ptr<MockEyeModule> mockEye;
    std::unique_ptr<ToReadyState> toReadyState;
};

// 由于友元类的限制，我们创建一个继承类来测试保护和私有方法
class TestableToReadyState : public ToReadyState
{
public:
    TestableToReadyState(RtdePtr rtde, EyeModulePtr eye) 
        : ToReadyState(rtde, eye) {}
    
    // 公开私有方法用于测试
    using ToReadyState::tooNear;
    using ToReadyState::getReadyPose;
    using ToReadyState::urProgress;
    using ToReadyState::urScript;
    using ToReadyState::clearContext;
};

class ToReadyStateInheritanceTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mockRtde = std::make_shared<testing::NiceMock<MockRtde>>();
        mockEye = std::make_shared<testing::NiceMock<MockEyeModule>>();
        mockStateMachine = std::make_unique<testing::NiceMock<MockStateMachine>>();
        mockOutputQueue = std::make_unique<testing::NiceMock<MockOutputQueue>>();
        
        ON_CALL(*mockStateMachine, get_outputQue()).WillByDefault(testing::Return(mockOutputQueue.get()));
        
        testableState = std::make_unique<TestableToReadyState>(mockRtde, mockEye);
        testableState->setStateMachine(mockStateMachine.get());
    }

    std::shared_ptr<MockRtde> mockRtde;
    std::shared_ptr<MockEyeModule> mockEye;
    std::unique_ptr<MockStateMachine> mockStateMachine;
    std::unique_ptr<MockOutputQueue> mockOutputQueue;
    std::unique_ptr<TestableToReadyState> testableState;
};

// 测试tooNear方法 - 位置过近
TEST_F(ToReadyStateInheritanceTest, TooNearClose)
{
    std::vector<double> pos1 = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    std::vector<double> pos2 = {1.005, 2.005, 3.005, 4.0, 5.0, 6.0}; // 距离约0.0087米
    
    bool result = testableState->tooNear(pos1, pos2);
    EXPECT_TRUE(result);
}

// 测试tooNear方法 - 位置较远
TEST_F(ToReadyStateInheritanceTest, TooNearFar)
{
    std::vector<double> pos1 = {1.0, 2.0, 3.0};
    std::vector<double> pos2 = {2.0, 3.0, 4.0}; // 距离约1.73米
    
    bool result = testableState->tooNear(pos1, pos2);
    EXPECT_FALSE(result);
}

// 测试tooNear方法 - 向量大小不足
TEST_F(ToReadyStateInheritanceTest, TooNearInsufficientSize)
{
    std::vector<double> pos1 = {1.0, 2.0}; // 只有2个元素
    std::vector<double> pos2 = {2.0, 3.0, 4.0};
    
    bool result = testableState->tooNear(pos1, pos2);
    EXPECT_FALSE(result); // 默认距离10，大于0.01
}

// 测试tooNear方法 - 边界情况
TEST_F(ToReadyStateInheritanceTest, TooNearBoundary)
{
    std::vector<double> pos1 = {0.0, 0.0, 0.0};
    std::vector<double> pos2 = {0.01, 0.0, 0.0}; // 距离正好0.01米
    
    bool result = testableState->tooNear(pos1, pos2);
    EXPECT_TRUE(result); // <= 0.01
}

// 测试getReadyPose方法
TEST_F(ToReadyStateInheritanceTest, GetReadyPoseDefault)
{
    std::vector<double> pose = testableState->getReadyPose();
    
    EXPECT_EQ(pose.size(), 6);
    // 由于ConfigManager返回"0.0"，所有值应该为0
    for (double value : pose) {
        EXPECT_EQ(value, 0.0);
    }
}

// 测试urScript方法
TEST_F(ToReadyStateInheritanceTest, UrScript)
{
    std::vector<std::string> script = testableState->urScript();
    
    EXPECT_FALSE(script.empty());
    EXPECT_EQ(script[0], "# UR Script for ToReady State");
}

// 测试urProgress方法 - 运动进行中
TEST_F(ToReadyStateInheritanceTest, UrProgressInProgress)
{
    testableState->changeStatus(MoveStatus::MOVING);
    
    // 创建模拟命令
    auto mockCommand = std::make_shared<testing::NiceMock<MockToReadyPoseCommand>>();
    std::vector<double> testPose = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    
    testing::ON_CALL(*mockCommand, get_type()).WillByDefault(testing::Return(CommandType::ACTION_MOVE_TO_READY));
    testing::ON_CALL(*mockCommand, get_pose()).WillByDefault(testing::Return(testPose));
    
    CommandEvent commandEvent(mockCommand);
    testableState->event(&commandEvent);
    
    // 测试进度大于0的情况
    testableState->urProgress(50);
    EXPECT_EQ(testableState->getCurrentStatus(), MoveStatus::MOVING);
}

// 测试urProgress方法 - 运动完成
TEST_F(ToReadyStateInheritanceTest, UrProgressCompleted)
{
    testableState->changeStatus(MoveStatus::MOVING);
    
    // 创建模拟命令
    auto mockCommand = std::make_shared<testing::StrictMock<MockToReadyPoseCommand>>();
    std::vector<double> testPose = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    
    testing::ON_CALL(*mockCommand, get_type()).WillByDefault(testing::Return(CommandType::ACTION_MOVE_TO_READY));
    testing::ON_CALL(*mockCommand, get_pose()).WillByDefault(testing::Return(testPose));
    
    // 设置期望
    EXPECT_CALL(*mockCommand, set_status(true)).Times(1);
    EXPECT_CALL(*mockCommand, encode()).WillOnce(testing::Return("test_result"));
    EXPECT_CALL(*mockOutputQueue, put("test_result")).Times(1);
    
    CommandEvent commandEvent(mockCommand);
    testableState->event(&commandEvent);
    
    // 测试进度小于0的情况（运动完成）
    testableState->urProgress(-1);
    EXPECT_EQ(testableState->getCurrentStatus(), MoveStatus::FINISHED);
}

// 测试clearContext方法
TEST_F(ToReadyStateInheritanceTest, ClearContext)
{
    // 设置一些初始状态
    testableState->changeStatus(MoveStatus::MOVING);
    
    auto mockCommand = std::make_shared<testing::NiceMock<MockToReadyPoseCommand>>();
    testing::ON_CALL(*mockCommand, get_type()).WillByDefault(testing::Return(CommandType::ACTION_MOVE_TO_READY));
    
    CommandEvent commandEvent(mockCommand);
    testableState->event(&commandEvent);
    
    // 清除上下文
    testableState->clearContext();
    
    // 验证状态被重置
    EXPECT_EQ(testableState->getCurrentStatus(), MoveStatus::STOPING);
}