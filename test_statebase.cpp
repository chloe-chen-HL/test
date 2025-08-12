#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>
#include "statebase.h"
#include "mock_classes.h"

// Test fixture for StateBase
class StateBaseTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        stateMachine_ = std::make_shared<StateMachine>();
        stateMachineWeakPtr_ = stateMachine_;
    }

    void TearDown() override 
    {
        stateMachine_.reset();
    }

    std::shared_ptr<StateMachine> stateMachine_;
    std::weak_ptr<StateMachine> stateMachineWeakPtr_;
};

// Concrete implementation of StateBase for testing
class TestableStateBase : public StateBase 
{
public:
    TestableStateBase(StateType type, std::weak_ptr<StateMachine> stateMachine = {})
        : StateBase(type, stateMachine), enterCalled_(false), exitCalled_(false) {}

    bool enter() override 
    {
        enterCalled_ = true;
        return true;
    }

    bool exit() override 
    {
        exitCalled_ = true;
        return true;
    }

    // Accessors for testing
    bool wasEnterCalled() const { return enterCalled_; }
    bool wasExitCalled() const { return exitCalled_; }
    MoveStatus getCurrentStatus() const { return curStatus_; }
    StateType getType() const { return type_; }

    // Make protected methods public for testing
    using StateBase::event;
    using StateBase::clearContext;
    using StateBase::changeStatus;

private:
    bool enterCalled_;
    bool exitCalled_;
};

// Test 1: 构造函数测试
TEST_F(StateBaseTest, ConstructorTest) 
{
    // Test constructor with state type only
    TestableStateBase state1(StateType::IDLE);
    EXPECT_EQ(state1.getStateType(), StateType::IDLE);
    EXPECT_EQ(state1.getCurrentStatus(), MoveStatus::STOPING);
    EXPECT_EQ(state1.getStateMachine(), nullptr);

    // Test constructor with state type and state machine
    TestableStateBase state2(StateType::READY, stateMachineWeakPtr_);
    EXPECT_EQ(state2.getStateType(), StateType::READY);
    EXPECT_EQ(state2.getCurrentStatus(), MoveStatus::STOPING);
    EXPECT_NE(state2.getStateMachine(), nullptr);
}

// Test 2: getStateType方法测试
TEST_F(StateBaseTest, GetStateTypeTest) 
{
    TestableStateBase idleState(StateType::IDLE);
    TestableStateBase readyState(StateType::READY);
    TestableStateBase needleState(StateType::NEEDLE);

    EXPECT_EQ(idleState.getStateType(), StateType::IDLE);
    EXPECT_EQ(readyState.getStateType(), StateType::READY);
    EXPECT_EQ(needleState.getStateType(), StateType::NEEDLE);
}

// Test 3: setStateMachine和getStateMachine方法测试
TEST_F(StateBaseTest, StateMachineAccessTest) 
{
    TestableStateBase state(StateType::IDLE);
    
    // Initially no state machine
    EXPECT_EQ(state.getStateMachine(), nullptr);
    
    // Set state machine
    state.setStateMachine(stateMachineWeakPtr_);
    auto retrievedStateMachine = state.getStateMachine();
    EXPECT_NE(retrievedStateMachine, nullptr);
    EXPECT_EQ(retrievedStateMachine, stateMachine_);
    
    // Test with expired weak_ptr
    stateMachine_.reset();
    EXPECT_EQ(state.getStateMachine(), nullptr);
}

// Test 4: changeStatus方法测试（线程安全性）
TEST_F(StateBaseTest, ChangeStatusTest) 
{
    TestableStateBase state(StateType::IDLE);
    
    // Test initial status
    EXPECT_EQ(state.getCurrentStatus(), MoveStatus::STOPING);
    
    // Test status change
    state.changeStatus(MoveStatus::READY);
    EXPECT_EQ(state.getCurrentStatus(), MoveStatus::READY);
    
    state.changeStatus(MoveStatus::MOVING);
    EXPECT_EQ(state.getCurrentStatus(), MoveStatus::MOVING);
    
    state.changeStatus(MoveStatus::FINISHED);
    EXPECT_EQ(state.getCurrentStatus(), MoveStatus::FINISHED);
}

// Test 5: changeStatus线程安全测试
TEST_F(StateBaseTest, ChangeStatusThreadSafetyTest) 
{
    TestableStateBase state(StateType::IDLE);
    const int numThreads = 10;
    const int changesPerThread = 100;
    
    std::vector<std::thread> threads;
    
    // Launch multiple threads to change status concurrently
    for (int i = 0; i < numThreads; ++i) 
    {
        threads.emplace_back([&state, changesPerThread, i]() 
        {
            for (int j = 0; j < changesPerThread; ++j) 
            {
                MoveStatus status = static_cast<MoveStatus>((i + j) % 5);
                state.changeStatus(status);
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) 
    {
        thread.join();
    }
    
    // The final status should be one of the valid values
    MoveStatus finalStatus = state.getCurrentStatus();
    EXPECT_TRUE(finalStatus >= MoveStatus::READY && finalStatus <= MoveStatus::FINISHED);
}

// Test 6: clearContext方法测试
TEST_F(StateBaseTest, ClearContextTest) 
{
    TestableStateBase state(StateType::IDLE);
    
    // clearContext should not throw and should be callable
    EXPECT_NO_THROW(state.clearContext());
}

// Test 7: event方法测试 - 非CommandEvent
TEST_F(StateBaseTest, EventHandlingNonCommandEventTest) 
{
    TestableStateBase state(StateType::IDLE, stateMachineWeakPtr_);
    
    // Create a regular QEvent
    QEvent regularEvent(QEvent::None);
    
    // Should return false for non-CommandEvent
    EXPECT_FALSE(state.event(&regularEvent));
    EXPECT_FALSE(regularEvent.isAccepted());
}

// Test 8: event方法测试 - LightControlCommand
TEST_F(StateBaseTest, EventHandlingLightControlCommandTest) 
{
    TestableStateBase state(StateType::IDLE, stateMachineWeakPtr_);
    
    // Create a LightControlCommand
    auto lightCommand = std::make_shared<LightControlCommand>("RED");
    CommandEvent commandEvent(lightCommand);
    
    // Reset needle state
    stateMachine_->get_needle()->reset();
    
    // Handle the event
    bool result = state.event(&commandEvent);
    
    // Should return true and accept the event
    EXPECT_TRUE(result);
    EXPECT_TRUE(commandEvent.isAccepted());
    
    // Check that the needle color was set
    EXPECT_EQ(stateMachine_->get_needle()->getLastColor(), NeedleController::LightColor::RED);
    EXPECT_EQ(stateMachine_->get_needle()->getColorSetCount(), 1);
}

// Test 9: event方法测试 - 不同颜色的LightControlCommand
TEST_F(StateBaseTest, EventHandlingDifferentColorsTest) 
{
    TestableStateBase state(StateType::IDLE, stateMachineWeakPtr_);
    
    struct TestCase 
    {
        QString colorName;
        NeedleController::LightColor expectedColor;
    };
    
    std::vector<TestCase> testCases = {
        {"RED", NeedleController::LightColor::RED},
        {"GREEN", NeedleController::LightColor::GREEN},
        {"BLUE", NeedleController::LightColor::BLUE},
        {"WHITE", NeedleController::LightColor::WHITE},
        {"UNKNOWN", NeedleController::LightColor::OFF}
    };
    
    for (const auto& testCase : testCases) 
    {
        stateMachine_->get_needle()->reset();
        
        auto lightCommand = std::make_shared<LightControlCommand>(testCase.colorName);
        CommandEvent commandEvent(lightCommand);
        
        bool result = state.event(&commandEvent);
        
        EXPECT_TRUE(result);
        EXPECT_TRUE(commandEvent.isAccepted());
        EXPECT_EQ(stateMachine_->get_needle()->getLastColor(), testCase.expectedColor);
    }
}

// Test 10: event方法测试 - 没有StateMachine
TEST_F(StateBaseTest, EventHandlingNoStateMachineTest) 
{
    TestableStateBase state(StateType::IDLE); // No state machine
    
    auto lightCommand = std::make_shared<LightControlCommand>("RED");
    CommandEvent commandEvent(lightCommand);
    
    // Should still return true and accept event, but no action taken
    bool result = state.event(&commandEvent);
    EXPECT_TRUE(result);
    EXPECT_TRUE(commandEvent.isAccepted());
}

// Test 11: event方法测试 - StateMachine没有needle
TEST_F(StateBaseTest, EventHandlingStateMachineWithoutNeedleTest) 
{
    // Create a state machine without needle (this would be a different mock scenario)
    TestableStateBase state(StateType::IDLE, stateMachineWeakPtr_);
    
    auto lightCommand = std::make_shared<LightControlCommand>("RED");
    CommandEvent commandEvent(lightCommand);
    
    // Should return true and accept event
    bool result = state.event(&commandEvent);
    EXPECT_TRUE(result);
    EXPECT_TRUE(commandEvent.isAccepted());
}

// Test 12: event方法测试 - 非LightControl命令
TEST_F(StateBaseTest, EventHandlingNonLightControlCommandTest) 
{
    TestableStateBase state(StateType::IDLE, stateMachineWeakPtr_);
    
    // Create a mock command that's not LightControlCommand
    class MockOtherCommand : public Command 
    {
    public:
        CommandType get_type() const override { return CommandType::OTHER_COMMAND; }
    };
    
    auto otherCommand = std::make_shared<MockOtherCommand>();
    CommandEvent commandEvent(otherCommand);
    
    // Should call parent event handler and return false
    bool result = state.event(&commandEvent);
    EXPECT_FALSE(result);
    EXPECT_FALSE(commandEvent.isAccepted());
}

// Test 13: 虚函数测试
TEST_F(StateBaseTest, VirtualFunctionTest) 
{
    TestableStateBase state(StateType::READY);
    
    // Test that virtual functions can be called
    EXPECT_FALSE(state.wasEnterCalled());
    EXPECT_FALSE(state.wasExitCalled());
    
    EXPECT_TRUE(state.enter());
    EXPECT_TRUE(state.wasEnterCalled());
    
    EXPECT_TRUE(state.exit());
    EXPECT_TRUE(state.wasExitCalled());
}

// Test 14: 多个StateType的测试
TEST_F(StateBaseTest, MultipleStateTypesTest) 
{
    std::vector<StateType> stateTypes = {
        StateType::IDLE,
        StateType::READY,
        StateType::T1GEN,
        StateType::TOREADY,
        StateType::TOSURFACE,
        StateType::ADJUST,
        StateType::URDRAG,
        StateType::NEEDLE
    };
    
    for (StateType type : stateTypes) 
    {
        TestableStateBase state(type, stateMachineWeakPtr_);
        EXPECT_EQ(state.getStateType(), type);
        EXPECT_NE(state.getStateMachine(), nullptr);
    }
}

// Test 15: 移动语义测试
TEST_F(StateBaseTest, MoveSemanticTest) 
{
    auto stateMachine1 = std::make_shared<StateMachine>();
    auto stateMachine2 = std::make_shared<StateMachine>();
    
    TestableStateBase state(StateType::IDLE, stateMachine1);
    EXPECT_EQ(state.getStateMachine(), stateMachine1);
    
    // Move assignment
    state.setStateMachine(std::weak_ptr<StateMachine>(stateMachine2));
    EXPECT_EQ(state.getStateMachine(), stateMachine2);
    EXPECT_NE(state.getStateMachine(), stateMachine1);
}

// Main function
int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}