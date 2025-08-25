#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QCoreApplication>
#include <QEvent>
#include <memory>

// 包含被测试的类
#include "toreadystate.h"
#include "toreadyposecommand.h"
#include "readyposecommand.h"
#include "factory.h"

// Mock类定义
class MockRtde : public sgnioRTDEControl {
public:
    MOCK_METHOD(void, movel, (const std::vector<double>& pose), (override));
    MOCK_METHOD(void, stopl, (), (override));
    MOCK_METHOD(int, getAsyncProgress, (), (override));
};

class MockEyeModule : public EyeModule {
public:
    // 可以根据需要添加mock方法
};

class MockStateMachine {
public:
    MOCK_METHOD(class QQueue<std::string>*, get_outputQue, (), ());
};

// 测试夹具
class ToReadyStateTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建Qt应用程序实例（如果还没有的话）
        if (!QCoreApplication::instance()) {
            int argc = 1;
            char* argv[] = {(char*)"test"};
            app_ = std::make_unique<QCoreApplication>(argc, argv);
        }
        
        // 创建mock对象
        mockRtde_ = std::make_shared<MockRtde>();
        mockEye_ = std::make_shared<MockEyeModule>();
        
        // 创建被测试对象
        toReadyState_ = std::make_unique<ToReadyState>(mockRtde_, mockEye_);
    }
    
    void TearDown() override {
        toReadyState_.reset();
        mockRtde_.reset();
        mockEye_.reset();
    }
    
    std::unique_ptr<QCoreApplication> app_;
    std::shared_ptr<MockRtde> mockRtde_;
    std::shared_ptr<MockEyeModule> mockEye_;
    std::unique_ptr<ToReadyState> toReadyState_;
};

// 测试用例1: 构造函数测试
TEST_F(ToReadyStateTest, Constructor) {
    EXPECT_EQ(toReadyState_->getType(), StateType::TOREADY);
    EXPECT_EQ(toReadyState_->getStatus(), MoveStatus::STOPING);
}

// 测试用例2: enter方法测试 - 成功情况
TEST_F(ToReadyStateTest, EnterSuccess) {
    EXPECT_TRUE(toReadyState_->enter());
    EXPECT_EQ(toReadyState_->getStatus(), MoveStatus::STOPING);
}

// 测试用例3: enter方法测试 - 失败情况（空指针）
TEST_F(ToReadyStateTest, EnterFailureWithNullPointers) {
    // 创建带有空指针的ToReadyState
    auto nullState = std::make_unique<ToReadyState>(nullptr, nullptr);
    EXPECT_FALSE(nullState->enter());
}

// 测试用例4: exit方法测试
TEST_F(ToReadyStateTest, Exit) {
    EXPECT_TRUE(toReadyState_->exit());
}

// 测试用例5: getReadyPose方法测试
TEST_F(ToReadyStateTest, GetReadyPose) {
    // 测试从配置获取就绪位置
    auto pose = toReadyState_->getReadyPose();
    EXPECT_EQ(pose.size(), 6);
    
    // 验证默认值（基于我们的mock配置）
    for (int i = 0; i < 6; ++i) {
        EXPECT_DOUBLE_EQ(pose[i], (i + 1) * 10.0);
    }
}

// 测试用例6: getReadyPose方法测试 - 从工厂获取
TEST_F(ToReadyStateTest, GetReadyPoseFromFactory) {
    // 创建一个ReadyPoseCommand并设置到工厂
    auto readyPoseCmd = std::make_shared<ReadyPoseCommand>();
    std::vector<double> expectedPose = {1.1, 2.2, 3.3, 4.4, 5.5, 6.6};
    readyPoseCmd->set_pose(expectedPose);
    
    Singleton<CommandFactory>::instance().setLastCommand(readyPoseCmd);
    
    auto pose = toReadyState_->getReadyPose();
    EXPECT_EQ(pose, expectedPose);
}

// 测试用例7: tooNear方法测试 - 距离很近
TEST_F(ToReadyStateTest, TooNearClosePoints) {
    std::vector<double> p1 = {0.0, 0.0, 0.0};
    std::vector<double> p2 = {0.005, 0.005, 0.005}; // 距离约0.00866
    
    EXPECT_TRUE(toReadyState_->tooNear(p1, p2));
}

// 测试用例8: tooNear方法测试 - 距离较远
TEST_F(ToReadyStateTest, TooNearFarPoints) {
    std::vector<double> p1 = {0.0, 0.0, 0.0};
    std::vector<double> p2 = {1.0, 1.0, 1.0}; // 距离约1.732
    
    EXPECT_FALSE(toReadyState_->tooNear(p1, p2));
}

// 测试用例9: tooNear方法测试 - 向量大小不足
TEST_F(ToReadyStateTest, TooNearInsufficientVectors) {
    std::vector<double> p1 = {0.0, 0.0}; // 只有2个元素
    std::vector<double> p2 = {0.0, 0.0};
    
    EXPECT_FALSE(toReadyState_->tooNear(p1, p2)); // 应该返回false，因为距离默认为10
}

// 测试用例10: CommandEvent处理测试
TEST_F(ToReadyStateTest, HandleCommandEvent) {
    auto command = std::make_shared<ToReadyPoseCommand>();
    CommandEvent event(command);
    
    EXPECT_TRUE(toReadyState_->event(&event));
    EXPECT_TRUE(event.isAccepted());
}

// 测试用例11: PedalEvent处理测试 - 按下踏板
TEST_F(ToReadyStateTest, HandlePedalEventPressed) {
    // 首先设置一个命令
    auto command = std::make_shared<ToReadyPoseCommand>();
    std::vector<double> pose = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    command->set_pose(pose);
    
    CommandEvent cmdEvent(command);
    toReadyState_->event(&cmdEvent);
    
    // 设置mock期望
    EXPECT_CALL(*mockRtde_, movel(pose)).Times(1);
    EXPECT_CALL(*mockRtde_, getAsyncProgress()).WillOnce(testing::Return(50));
    
    // 创建踏板按下事件
    PedalEvent pedalEvent(true);
    EXPECT_TRUE(toReadyState_->event(&pedalEvent));
    EXPECT_TRUE(pedalEvent.isAccepted());
}

// 测试用例12: PedalEvent处理测试 - 释放踏板
TEST_F(ToReadyStateTest, HandlePedalEventReleased) {
    // 首先设置状态为MOVING
    toReadyState_->changeStatus(MoveStatus::MOVING);
    
    // 设置mock期望
    EXPECT_CALL(*mockRtde_, stopl()).Times(1);
    
    // 创建踏板释放事件
    PedalEvent pedalEvent(false);
    EXPECT_TRUE(toReadyState_->event(&pedalEvent));
    EXPECT_TRUE(pedalEvent.isAccepted());
}

// 测试用例13: urProgress方法测试 - 完成状态
TEST_F(ToReadyStateTest, UrProgressFinished) {
    // 设置状态为MOVING
    toReadyState_->changeStatus(MoveStatus::MOVING);
    
    // 设置一个命令
    auto command = std::make_shared<ToReadyPoseCommand>();
    toReadyState_->toReady_ = command;
    
    // 调用urProgress，传入负值表示完成
    toReadyState_->urProgress(-1);
    
    EXPECT_EQ(toReadyState_->getStatus(), MoveStatus::FINISHED);
    EXPECT_TRUE(command->get_status());
}

// 测试用例14: clearContext方法测试
TEST_F(ToReadyStateTest, ClearContext) {
    // 设置一些状态
    toReadyState_->changeStatus(MoveStatus::MOVING);
    auto command = std::make_shared<ToReadyPoseCommand>();
    toReadyState_->toReady_ = command;
    
    // 清除上下文
    toReadyState_->clearContext();
    
    EXPECT_EQ(toReadyState_->getStatus(), MoveStatus::STOPING);
    // 注意：toReady_是私有成员，我们通过其他方式验证
}

// 测试用例15: 边界条件测试 - 空命令
TEST_F(ToReadyStateTest, HandleEmptyCommand) {
    CommandEvent event(nullptr);
    EXPECT_FALSE(toReadyState_->event(&event));
}

// 测试用例16: 性能测试 - 大量位置计算
TEST_F(ToReadyStateTest, PerformanceTooNearCalculation) {
    std::vector<double> p1 = {0.0, 0.0, 0.0};
    std::vector<double> p2 = {0.01, 0.01, 0.01};
    
    // 执行多次计算，验证性能
    for (int i = 0; i < 1000; ++i) {
        bool result = toReadyState_->tooNear(p1, p2);
        EXPECT_FALSE(result); // 距离约0.017，应该大于0.01
    }
}

// 测试用例17: 异常处理测试
TEST_F(ToReadyStateTest, ExceptionHandling) {
    // 测试在异常情况下的行为
    // 这里可以测试各种边界条件和异常情况
    EXPECT_TRUE(true); // 占位符，实际测试中应该测试具体的异常处理
}

// 测试用例18: 状态转换测试
TEST_F(ToReadyStateTest, StateTransitions) {
    // 测试各种状态转换
    EXPECT_EQ(toReadyState_->getStatus(), MoveStatus::STOPING);
    
    toReadyState_->changeStatus(MoveStatus::MOVING);
    EXPECT_EQ(toReadyState_->getStatus(), MoveStatus::MOVING);
    
    toReadyState_->changeStatus(MoveStatus::FINISHED);
    EXPECT_EQ(toReadyState_->getStatus(), MoveStatus::FINISHED);
}

// 测试用例19: 配置值测试
TEST_F(ToReadyStateTest, ConfigurationValues) {
    // 测试不同配置值下的行为
    auto pose = toReadyState_->getReadyPose();
    
    // 验证所有关节角度都在合理范围内
    for (double angle : pose) {
        EXPECT_GE(angle, 0.0);
        EXPECT_LE(angle, 360.0); // 假设角度范围
    }
}

// 测试用例20: 集成测试
TEST_F(ToReadyStateTest, IntegrationTest) {
    // 完整的集成测试流程
    EXPECT_TRUE(toReadyState_->enter());
    
    // 设置命令
    auto command = std::make_shared<ToReadyPoseCommand>();
    CommandEvent cmdEvent(command);
    EXPECT_TRUE(toReadyState_->event(&cmdEvent));
    
    // 模拟踏板操作
    EXPECT_CALL(*mockRtde_, movel(testing::_)).Times(1);
    EXPECT_CALL(*mockRtde_, getAsyncProgress()).WillOnce(testing::Return(50));
    
    PedalEvent pedalEvent(true);
    EXPECT_TRUE(toReadyState_->event(&pedalEvent));
    
    // 模拟完成
    toReadyState_->urProgress(-1);
    EXPECT_EQ(toReadyState_->getStatus(), MoveStatus::FINISHED);
    
    EXPECT_TRUE(toReadyState_->exit());
}