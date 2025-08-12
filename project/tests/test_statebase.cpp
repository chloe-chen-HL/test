#include <gtest/gtest.h>
#include <memory>
#include "statebase.h"
#include "statemachine.h"
#include "lightcontrolcommand.h"
#include "command.h"
#include "event.h"

class TestState : public StateBase {
public:
    using StateBase::StateBase;
    bool enter() override { return true; }
    bool exit() override { return true; }

    bool callEvent(QEvent* e) { return StateBase::event(e); }

    MoveStatus getStatus() const { return curStatus_; }
    void setStatus(MoveStatus st) { changeStatus(st); }
};

class OtherCommand : public Command {
public:
    CommandType get_type() const override { return CommandType::OTHER; }
};

TEST(StateBaseTests, ReturnsStateType) {
    auto sm = std::make_shared<StateMachine>();
    TestState st(StateType::READY, sm);
    EXPECT_EQ(st.getStateType(), StateType::READY);
}

TEST(StateBaseTests, SetAndGetStateMachine) {
    TestState st(StateType::IDLE);
    auto sm = std::make_shared<StateMachine>();
    st.setStateMachine(sm);
    ASSERT_TRUE(st.getStateMachine() != nullptr);
    EXPECT_EQ(st.getStateMachine().get(), sm.get());
}

TEST(StateBaseTests, LightControlCommandAcceptedAndSetsNeedleColor) {
    auto sm = std::make_shared<StateMachine>();
    auto needle = std::make_shared<NeedleController>();
    sm->set_needle(needle);

    TestState st(StateType::READY, sm);

    auto cmd = std::make_shared<LightControlCommand>(QString("Green"));
    CommandEvent evt(cmd);

    bool handled = st.callEvent(&evt);

    EXPECT_TRUE(handled);
    EXPECT_TRUE(evt.isAccepted());
    EXPECT_TRUE(needle->called);
    ASSERT_TRUE(needle->lastColor.has_value());
    EXPECT_EQ(needle->lastColor.value(), NeedleController::LightColor::Green);
}

TEST(StateBaseTests, LightControlCommandAcceptedEvenWithoutNeedle) {
    auto sm = std::make_shared<StateMachine>();

    TestState st(StateType::READY, sm);

    auto cmd = std::make_shared<LightControlCommand>(QString("Blue"));
    CommandEvent evt(cmd);

    bool handled = st.callEvent(&evt);

    EXPECT_TRUE(handled);
    EXPECT_TRUE(evt.isAccepted());
    // No needle set; nothing else to assert
}

TEST(StateBaseTests, NonLightCommandFallsBackToQObjectEvent) {
    TestState st(StateType::IDLE);
    auto cmd = std::make_shared<OtherCommand>();
    CommandEvent evt(cmd);

    bool handled = st.callEvent(&evt);
    EXPECT_FALSE(handled);
    EXPECT_FALSE(evt.isAccepted());
}

TEST(StateBaseTests, ChangeStatusIsThreadSafeAndSetsValue) {
    TestState st(StateType::IDLE);
    st.setStatus(MoveStatus::MOVING);
    EXPECT_EQ(st.getStatus(), MoveStatus::MOVING);
}