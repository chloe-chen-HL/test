#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <iostream>

// Forward declarations
class StateMachine;

// Mock enums and classes for testing
enum class StateType
{
    IDLE,
    READY,
    T1GEN,
    TOREADY,
    TOSURFACE,
    ADJUST,
    URDRAG,
    NEEDLE,
};

enum class MoveStatus
{
    READY,
    STOPING,
    MOVING,
    FINISHED_ONE_STEP,
    FINISHED
};

// Minimal mock implementations
class QEvent
{
public:
    enum Type { None = 0, User = 1000 };
    QEvent(Type type) : type_(type), accepted_(false) {}
    virtual ~QEvent() = default;
    Type type() const { return type_; }
    void accept() { accepted_ = true; }
    bool isAccepted() const { return accepted_; }
private:
    Type type_;
    bool accepted_;
};

class QObject
{
public:
    QObject() = default;
    virtual ~QObject() = default;
    virtual bool event(QEvent* e) { (void)e; return false; }
};

enum class CommandType
{
    ACTION_LIGHT_CONTROL,
    OTHER_COMMAND
};

class Command
{
public:
    virtual ~Command() = default;
    virtual CommandType get_type() const = 0;
    static std::string type2string(CommandType type)
    {
        return (type == CommandType::ACTION_LIGHT_CONTROL) ? "ACTION_LIGHT_CONTROL" : "UNKNOWN";
    }
};

class LightControlCommand : public Command
{
public:
    LightControlCommand(const std::string& param) : param_(param) {}
    CommandType get_type() const override { return CommandType::ACTION_LIGHT_CONTROL; }
    std::string get_param() const { return param_; }
private:
    std::string param_;
};

class CommandEvent : public QEvent
{
public:
    static const Type EventType = static_cast<Type>(User + 1);
    CommandEvent(std::shared_ptr<Command> cmd) : QEvent(EventType), command_(cmd) {}
    std::shared_ptr<Command> get_command() const { return command_; }
private:
    std::shared_ptr<Command> command_;
};

// Mock NeedleController namespace
namespace NeedleController
{
    enum class LightColor { RED, GREEN, BLUE, WHITE, OFF };
}

class MockNeedle
{
public:
    void setLightColor(NeedleController::LightColor color) 
    { 
        lastColor_ = color; 
        colorSetCount_++;
    }
    NeedleController::LightColor getLastColor() const { return lastColor_; }
    int getColorSetCount() const { return colorSetCount_; }
    void reset() { colorSetCount_ = 0; lastColor_ = NeedleController::LightColor::OFF; }
private:
    NeedleController::LightColor lastColor_ = NeedleController::LightColor::OFF;
    int colorSetCount_ = 0;
};

class StateMachine
{
public:
    StateMachine() : needle_(std::make_shared<MockNeedle>()) {}
    std::shared_ptr<MockNeedle> get_needle() { return needle_; }
private:
    std::shared_ptr<MockNeedle> needle_;
};

// Mock EnumRegistry
template<typename T>
class EnumRegistry
{
public:
    static EnumRegistry& instance()
    {
        static EnumRegistry inst;
        return inst;
    }
    T fromString(const std::string& str)
    {
        if (str == "RED") return static_cast<T>(0);
        if (str == "GREEN") return static_cast<T>(1);
        if (str == "BLUE") return static_cast<T>(2);
        if (str == "WHITE") return static_cast<T>(3);
        return static_cast<T>(4); // OFF
    }
};

// Mock qDebug functionality
struct QDebug {
    template<typename T> QDebug& operator<<(const T&) { return *this; }
};
QDebug qDebug() { return QDebug(); }

// Include mutex for thread safety
#include <mutex>

// StateBase class implementation
class StateBase : public QObject
{
public:
    StateBase(StateType type, std::weak_ptr<StateMachine> stateMachine = {})
        : stateMachine_(std::move(stateMachine)), type_(type), curStatus_(MoveStatus::STOPING) {}
    
    virtual ~StateBase() = default;
    virtual bool enter() = 0;
    virtual bool exit() = 0;
    
    StateType getStateType() const { return type_; }
    
    void setStateMachine(std::weak_ptr<StateMachine> stateMachine)
    {
        stateMachine_ = std::move(stateMachine);
    }
    
    std::shared_ptr<StateMachine> getStateMachine() const
    {
        return stateMachine_.lock();
    }

protected:
    virtual bool event(QEvent *e) override
    {
        if (e->type() == CommandEvent::EventType)
        {
            CommandEvent *event = static_cast<CommandEvent *>(e);
            auto com = event->get_command();
            qDebug() << "base: received com" << Command::type2string(com->get_type());

            if (com->get_type() == CommandType::ACTION_LIGHT_CONTROL)
            {
                auto light = std::dynamic_pointer_cast<LightControlCommand>(com);
                if (getStateMachine() && getStateMachine()->get_needle())
                {
                    auto name = light->get_param();
                    auto color = EnumRegistry<NeedleController::LightColor>::instance().fromString(name);
                    qDebug() << "the color will be set: " << name << " " << (int)color;
                    getStateMachine()->get_needle()->setLightColor(color);
                }
                e->accept();
                return true;
            }
        }
        return QObject::event(e);
    }
    
    virtual void clearContext() {}
    
    void changeStatus(MoveStatus st)
    {
        std::lock_guard<std::mutex> lk(mutex_);
        curStatus_ = st;
    }

    // For testing access
    MoveStatus getCurrentStatus() const
    {
        std::lock_guard<std::mutex> lk(mutex_);
        return curStatus_;
    }

protected:
    MoveStatus curStatus_;
    StateType type_;

private:
    std::weak_ptr<StateMachine> stateMachine_;
    mutable std::mutex mutex_;
};

// Test fixture and testable implementation
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
    
    // Make protected methods public for testing
    using StateBase::event;
    using StateBase::clearContext;
    using StateBase::changeStatus;
    using StateBase::getCurrentStatus;

private:
    bool enterCalled_;
    bool exitCalled_;
};

// Test 1: Constructor Test
TEST_F(StateBaseTest, ConstructorTest) 
{
    TestableStateBase state1(StateType::IDLE);
    EXPECT_EQ(state1.getStateType(), StateType::IDLE);
    EXPECT_EQ(state1.getCurrentStatus(), MoveStatus::STOPING);
    EXPECT_EQ(state1.getStateMachine(), nullptr);

    TestableStateBase state2(StateType::READY, stateMachineWeakPtr_);
    EXPECT_EQ(state2.getStateType(), StateType::READY);
    EXPECT_EQ(state2.getCurrentStatus(), MoveStatus::STOPING);
    EXPECT_NE(state2.getStateMachine(), nullptr);
}

// Test 2: getStateType Test
TEST_F(StateBaseTest, GetStateTypeTest) 
{
    TestableStateBase idleState(StateType::IDLE);
    TestableStateBase readyState(StateType::READY);
    TestableStateBase needleState(StateType::NEEDLE);

    EXPECT_EQ(idleState.getStateType(), StateType::IDLE);
    EXPECT_EQ(readyState.getStateType(), StateType::READY);
    EXPECT_EQ(needleState.getStateType(), StateType::NEEDLE);
}

// Test 3: StateMachine Access Test
TEST_F(StateBaseTest, StateMachineAccessTest) 
{
    TestableStateBase state(StateType::IDLE);
    
    EXPECT_EQ(state.getStateMachine(), nullptr);
    
    state.setStateMachine(stateMachineWeakPtr_);
    auto retrievedStateMachine = state.getStateMachine();
    EXPECT_NE(retrievedStateMachine, nullptr);
    EXPECT_EQ(retrievedStateMachine, stateMachine_);
    
    // Test with expired weak_ptr - create a local shared_ptr that goes out of scope
    {
        auto localStateMachine = std::make_shared<StateMachine>();
        state.setStateMachine(localStateMachine);
        EXPECT_NE(state.getStateMachine(), nullptr);
    } // localStateMachine goes out of scope here
    EXPECT_EQ(state.getStateMachine(), nullptr);
}

// Test 4: changeStatus Test
TEST_F(StateBaseTest, ChangeStatusTest) 
{
    TestableStateBase state(StateType::IDLE);
    
    EXPECT_EQ(state.getCurrentStatus(), MoveStatus::STOPING);
    
    state.changeStatus(MoveStatus::READY);
    EXPECT_EQ(state.getCurrentStatus(), MoveStatus::READY);
    
    state.changeStatus(MoveStatus::MOVING);
    EXPECT_EQ(state.getCurrentStatus(), MoveStatus::MOVING);
    
    state.changeStatus(MoveStatus::FINISHED);
    EXPECT_EQ(state.getCurrentStatus(), MoveStatus::FINISHED);
}

// Test 5: Thread Safety Test
TEST_F(StateBaseTest, ChangeStatusThreadSafetyTest) 
{
    TestableStateBase state(StateType::IDLE);
    const int numThreads = 10;
    const int changesPerThread = 100;
    
    std::vector<std::thread> threads;
    
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
    
    for (auto& thread : threads) 
    {
        thread.join();
    }
    
    MoveStatus finalStatus = state.getCurrentStatus();
    EXPECT_TRUE(finalStatus >= MoveStatus::READY && finalStatus <= MoveStatus::FINISHED);
}

// Test 6: Clear Context Test
TEST_F(StateBaseTest, ClearContextTest) 
{
    TestableStateBase state(StateType::IDLE);
    EXPECT_NO_THROW(state.clearContext());
}

// Test 7: Event Handling - Non CommandEvent
TEST_F(StateBaseTest, EventHandlingNonCommandEventTest) 
{
    TestableStateBase state(StateType::IDLE, stateMachineWeakPtr_);
    
    QEvent regularEvent(QEvent::None);
    
    EXPECT_FALSE(state.event(&regularEvent));
    EXPECT_FALSE(regularEvent.isAccepted());
}

// Test 8: Event Handling - Light Control Command
TEST_F(StateBaseTest, EventHandlingLightControlCommandTest) 
{
    TestableStateBase state(StateType::IDLE, stateMachineWeakPtr_);
    
    auto lightCommand = std::make_shared<LightControlCommand>("RED");
    CommandEvent commandEvent(lightCommand);
    
    stateMachine_->get_needle()->reset();
    
    bool result = state.event(&commandEvent);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(commandEvent.isAccepted());
    
    EXPECT_EQ(stateMachine_->get_needle()->getLastColor(), NeedleController::LightColor::RED);
    EXPECT_EQ(stateMachine_->get_needle()->getColorSetCount(), 1);
}

// Test 9: Different Colors Test
TEST_F(StateBaseTest, EventHandlingDifferentColorsTest) 
{
    TestableStateBase state(StateType::IDLE, stateMachineWeakPtr_);
    
    struct TestCase 
    {
        std::string colorName;
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

// Test 10: Virtual Function Test
TEST_F(StateBaseTest, VirtualFunctionTest) 
{
    TestableStateBase state(StateType::READY);
    
    EXPECT_FALSE(state.wasEnterCalled());
    EXPECT_FALSE(state.wasExitCalled());
    
    EXPECT_TRUE(state.enter());
    EXPECT_TRUE(state.wasEnterCalled());
    
    EXPECT_TRUE(state.exit());
    EXPECT_TRUE(state.wasExitCalled());
}

// GTest main is provided by libgtest_main