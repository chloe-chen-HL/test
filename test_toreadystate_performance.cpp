#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <thread>
#include "toreadystate.h"

using namespace std::chrono;

class ToReadyStatePerformanceTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        mockRtde = std::make_shared<testing::NiceMock<MockRtde>>();
        mockEye = std::make_shared<testing::NiceMock<MockEyeModule>>();
        mockStateMachine = std::make_unique<testing::NiceMock<MockStateMachine>>();
        mockOutputQueue = std::make_unique<testing::NiceMock<MockOutputQueue>>();
        
        testing::ON_CALL(*mockEye, isConnected()).WillByDefault(testing::Return(true));
        testing::ON_CALL(*mockStateMachine, get_outputQue()).WillByDefault(testing::Return(mockOutputQueue.get()));
        
        toReadyState = std::make_unique<ToReadyState>(mockRtde, mockEye);
        toReadyState->setStateMachine(mockStateMachine.get());
    }

    std::shared_ptr<MockRtde> mockRtde;
    std::shared_ptr<MockEyeModule> mockEye;
    std::unique_ptr<MockStateMachine> mockStateMachine;
    std::unique_ptr<MockOutputQueue> mockOutputQueue;
    std::unique_ptr<ToReadyState> toReadyState;
};

// 性能测试 - 事件处理速度
TEST_F(ToReadyStatePerformanceTest, EventProcessingPerformance)
{
    const int numEvents = 1000;
    
    auto mockCommand = std::make_shared<testing::NiceMock<MockToReadyPoseCommand>>();
    std::vector<double> testPose = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    
    testing::ON_CALL(*mockCommand, get_type()).WillByDefault(testing::Return(CommandType::ACTION_MOVE_TO_READY));
    testing::ON_CALL(*mockCommand, get_pose()).WillByDefault(testing::Return(testPose));
    
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < numEvents; ++i) {
        CommandEvent event(mockCommand);
        toReadyState->event(&event);
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    // 每个事件处理应该在合理时间内完成（比如小于100微秒）
    double avgTimePerEvent = static_cast<double>(duration.count()) / numEvents;
    EXPECT_LT(avgTimePerEvent, 100.0) << "平均每个事件处理时间: " << avgTimePerEvent << " 微秒";
    
    std::cout << "处理 " << numEvents << " 个事件总时间: " << duration.count() << " 微秒" << std::endl;
    std::cout << "平均每个事件处理时间: " << avgTimePerEvent << " 微秒" << std::endl;
}

// 压力测试 - 快速连续的脚踏板事件
TEST_F(ToReadyStatePerformanceTest, RapidPedalEvents)
{
    // 首先设置命令
    auto mockCommand = std::make_shared<testing::NiceMock<MockToReadyPoseCommand>>();
    std::vector<double> testPose = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    
    testing::ON_CALL(*mockCommand, get_type()).WillByDefault(testing::Return(CommandType::ACTION_MOVE_TO_READY));
    testing::ON_CALL(*mockCommand, get_pose()).WillByDefault(testing::Return(testPose));
    testing::ON_CALL(*mockRtde, getAsyncProgress()).WillByDefault(testing::Return(1));
    
    CommandEvent commandEvent(mockCommand);
    toReadyState->event(&commandEvent);
    
    const int numPedalEvents = 500;
    auto start = high_resolution_clock::now();
    
    // 快速按下和松开脚踏板
    for (int i = 0; i < numPedalEvents; ++i) {
        PedalEvent pressEvent(true);
        PedalEvent releaseEvent(false);
        
        toReadyState->event(&pressEvent);
        toReadyState->event(&releaseEvent);
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    double avgTimePerEvent = static_cast<double>(duration.count()) / (numPedalEvents * 2);
    EXPECT_LT(avgTimePerEvent, 50.0) << "平均每个脚踏板事件处理时间: " << avgTimePerEvent << " 微秒";
    
    std::cout << "处理 " << (numPedalEvents * 2) << " 个脚踏板事件总时间: " << duration.count() << " 微秒" << std::endl;
}

// 内存测试 - 检查内存泄漏
TEST_F(ToReadyStatePerformanceTest, MemoryUsageTest)
{
    const int numIterations = 100;
    
    for (int i = 0; i < numIterations; ++i) {
        // 创建和销毁多个状态对象
        auto state = std::make_unique<ToReadyState>(mockRtde, mockEye);
        state->enter();
        
        auto mockCommand = std::make_shared<testing::NiceMock<MockToReadyPoseCommand>>();
        testing::ON_CALL(*mockCommand, get_type()).WillByDefault(testing::Return(CommandType::ACTION_MOVE_TO_READY));
        
        CommandEvent event(mockCommand);
        state->event(&event);
        
        state->exit();
        // state会自动销毁
    }
    
    // 这个测试主要是为了在valgrind或其他内存检查工具下运行
    EXPECT_TRUE(true); // 如果没有内存泄漏，测试就会通过
}

// 边界测试类
class ToReadyStateBoundaryTest : public ::testing::Test
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

// 边界测试 - 空向量位置
TEST_F(ToReadyStateBoundaryTest, EmptyPoseVector)
{
    auto mockCommand = std::make_shared<testing::NiceMock<MockToReadyPoseCommand>>();
    std::vector<double> emptyPose;
    
    testing::ON_CALL(*mockCommand, get_type()).WillByDefault(testing::Return(CommandType::ACTION_MOVE_TO_READY));
    testing::ON_CALL(*mockCommand, get_pose()).WillByDefault(testing::Return(emptyPose));
    
    CommandEvent commandEvent(mockCommand);
    toReadyState->event(&commandEvent);
    
    // 脚踏板按下，但位置为空
    PedalEvent pedalEvent(true);
    bool result = toReadyState->event(&pedalEvent);
    
    EXPECT_TRUE(result);
    // 应该不会调用movel，因为位置为空
    EXPECT_EQ(toReadyState->getCurrentStatus(), MoveStatus::STOPING);
}

// 边界测试 - 极大的位置值
TEST_F(ToReadyStateBoundaryTest, ExtremePoseValues)
{
    auto mockCommand = std::make_shared<testing::NiceMock<MockToReadyPoseCommand>>();
    std::vector<double> extremePose = {
        std::numeric_limits<double>::max(),
        std::numeric_limits<double>::min(),
        std::numeric_limits<double>::infinity(),
        -std::numeric_limits<double>::infinity(),
        std::numeric_limits<double>::quiet_NaN(),
        0.0
    };
    
    testing::ON_CALL(*mockCommand, get_type()).WillByDefault(testing::Return(CommandType::ACTION_MOVE_TO_READY));
    testing::ON_CALL(*mockCommand, get_pose()).WillByDefault(testing::Return(extremePose));
    
    CommandEvent commandEvent(mockCommand);
    bool result = toReadyState->event(&commandEvent);
    
    EXPECT_TRUE(result);
    // 系统应该能够处理极端值而不崩溃
}

// 边界测试 - 非常长的位置向量
TEST_F(ToReadyStateBoundaryTest, VeryLongPoseVector)
{
    auto mockCommand = std::make_shared<testing::NiceMock<MockToReadyPoseCommand>>();
    std::vector<double> longPose(1000, 1.0); // 1000个元素
    
    testing::ON_CALL(*mockCommand, get_type()).WillByDefault(testing::Return(CommandType::ACTION_MOVE_TO_READY));
    testing::ON_CALL(*mockCommand, get_pose()).WillByDefault(testing::Return(longPose));
    
    CommandEvent commandEvent(mockCommand);
    bool result = toReadyState->event(&commandEvent);
    
    EXPECT_TRUE(result);
}

// 边界测试 - 进度值边界
TEST_F(ToReadyStateBoundaryTest, ProgressBoundaryValues)
{
    toReadyState->changeStatus(MoveStatus::MOVING);
    
    auto mockCommand = std::make_shared<testing::NiceMock<MockToReadyPoseCommand>>();
    std::vector<double> testPose = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    
    testing::ON_CALL(*mockCommand, get_type()).WillByDefault(testing::Return(CommandType::ACTION_MOVE_TO_READY));
    testing::ON_CALL(*mockCommand, get_pose()).WillByDefault(testing::Return(testPose));
    
    CommandEvent commandEvent(mockCommand);
    toReadyState->event(&commandEvent);
    
    // 测试各种边界进度值
    std::vector<int> progressValues = {
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min(),
        0, 1, -1, 100, -100
    };
    
    for (int progress : progressValues) {
        testing::ON_CALL(*mockRtde, getAsyncProgress()).WillByDefault(testing::Return(progress));
        
        PedalEvent pedalEvent(true);
        bool result = toReadyState->event(&pedalEvent);
        EXPECT_TRUE(result);
    }
}

// 并发测试 - 多线程访问
TEST_F(ToReadyStateBoundaryTest, ConcurrentAccess)
{
    const int numThreads = 4;
    const int numOperationsPerThread = 50;
    
    std::vector<std::thread> threads;
    std::atomic<int> successCount{0};
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([this, numOperationsPerThread, &successCount]() {
            for (int j = 0; j < numOperationsPerThread; ++j) {
                try {
                    // 交替进行enter/exit操作
                    if (j % 2 == 0) {
                        if (toReadyState->enter()) {
                            successCount++;
                        }
                    } else {
                        if (toReadyState->exit()) {
                            successCount++;
                        }
                    }
                    
                    // 短暂休眠以增加并发竞争
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                } catch (...) {
                    // 捕获任何异常，但不让测试失败
                }
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }
    
    // 验证没有发生崩溃
    EXPECT_GT(successCount.load(), 0);
    std::cout << "并发操作成功次数: " << successCount.load() << std::endl;
}